/**
 * @file audio_buffer.hpp
 * @author Owen Cochell (owencochell@gmail.com)
 * @brief Components for AudioBuffers
 * @version 0.1
 * @date 2022-11-16
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#pragma once

#include <vector>
#include <iterator>

/// A Typedef representing a buffer of audio samples.
typedef std::vector<long double> AudioChannel;

/**
 * @brief Base class for AudioIterators
 * 
 * This class implements some useful operations and features
 * that are required for iterators.
 * We implement random access iterators,
 * so all of the operators are implemented and working.
 * 
 * Any iterators using this class are STL compatable!
 * You can pass any iterators to STL methods that expect them.
 * One useful application of this is the algorithms library,
 * which defines many operations that are useful for working with arrays and sequences.
 * 
 * This is a framework class!
 * It will not work properly on it's own.
 * We use upside down inheritance to add this functionality without virtual functions!
 *  
 * @tparam C The class that derives from this class.
 * @tparam T The typename this iterator will iterate over
 */
template <class C, typename T>
class BaseAudioIterator {

    private:

        /// Current pointer we are on
        T* point=nullptr;

        /// The current sample we are on
        int index=0;

    protected:

        /**
         * @brief Sets the pointer
         * 
         * This will set the pointer to the underlying sample.
         * This should be called by the child class when they have a pointer 
         * to work with.
         * 
         * @param ptr Pointer to current sample
         */
        void set_pointer(long double* ptr) { point = ptr; }

    public:

        // Tags for identifying this iterator

        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        /**
         * @brief Sets the index to the given value.
         * 
         * This will set the index to the given value,
         * and determine the current pointer.
         * How this is done differs from iterator to iterator.
         * This is done upon setting the index for performance reasons,
         * meaning that the pointer is determined once on index change
         * and is then kept for future use.
         * 
         * Please remember, this index is the position in relation
         * to the squished audio data!
         * The squished format differs between iterators.
         *
         * For example, assume the squished audio data is this:
         * 
         * 1,2,3,4,5,6,7
         * 
         * If you want value '3', then you want index 2.
         * 
         * We highly recommend using the helper methods in
         * the child iterator classes to help with seeking 
         * so specific values.
         * 
         * @param index New index to set
         */
        void set_index(int index) { this->index = index; static_cast<C*>(this)->resolve_pointer(); }

        /**
         * @brief Gets the current index
         * 
         * Returns the current index of the iterator.
         * Please keep in mind, the index is in relation
         * to the squished buffer!
         * 
         * @return int Current index
         */
        int get_index() const { return this->index; }

        /**
         * @brief Pre-increments the iterator
         * 
         * @return C& 
         */
        C& operator++() { this->set_index(this->get_index()+1); return static_cast<C&>(*this); }

        /**
         * @brief Post-increments the iterator
         * 
         * @return C 
         */
        C operator++(int) { C tmp = static_cast<C&>(*this); ++(*this); return tmp; }

        /**
         * @brief Pre-decrements the iterator
         * 
         * @return C& 
         */
        C& operator--() { this->set_index(this->get_index()-1); return static_cast<C&>(*this); }

        /**
         * @brief Post-decrements the iterator
         * 
         * @return C 
         */
        C operator--(int) { C tmp = static_cast<C&>(*this); --(*this); return tmp; }

        /**
         * @brief Adds the given number to this iterator
         * 
         * @param num Number to add to current index
         * @return C& This iterator
         */
        C& operator+=(const int& num) { this->set_index(this->get_index() + num); return static_cast<C&>(*this);}

        /**
         * @brief Subtracts the given number from this iterator
         * 
         * @param num Number to add to the current index
         * @return C& This iterator
         */
        C& operator-=(const int& num) { this->set_index(this->get_index() - num); return static_cast<C&>(*this);}

        /**
         * @brief Creates a new iterator by adding the given number to our index
         * 
         * @param num Number to add to the current index
         * @return C A new iterator with the new index
         */
        C operator+(const int& num) { C tmp = static_cast<C&>(*this); tmp += num; return tmp; }

        /**
         * @brief Creates a new iterator by adding ourselves to the given iterator
         * 
         * We simply take the index of the given iterator
         * and add it to ours.
         * 
         * @param iter Iterator to add
         * @return C A new iterator with the new index
         */
        C operator+(const C& iter) { C tmp = static_cast<C&>(*this); tmp += iter.get_index(); return tmp; }

        /**
         * @brief Creates a new iterator by subtracting the given number from our index
         * 
         * @param num Number to subtract our index from
         * @return C A new iterator with the new index
         */
        C operator-(const int& num) { C tmp = static_cast<C&>(*this); tmp -= num; return tmp; }

        /**
         * @brief Creates a new iterator by subtracting the given iterator from ourselves
         * 
         * We simply take the index of the given iterator and subtract it from ours.
         * 
         * @param iter Iterator to subtract
         * @return C A new iterator with the new index
         */
        C operator-(const C& iter) { C tmp = static_cast<C&>(*this); tmp -= iter.get_index(); return tmp; }

        /**
         * @brief Converts this iterator to an integer
         * 
         * We simply return our current index.
         * 
         * @return int The current index
         */
        operator int() const { return this->get_index(); }

        /**
         * @brief Determines if the given iterator is equivalent
         * 
         * We check if they are pointing to the same index.
         * 
         * @param a Iterator A
         * @return true If the two iterators are equivalent
         * @return false If the two iterators are not equivalent
         */
        bool operator==(const C& a) { return this->get_index() == a.get_index(); }

        /**
         * @brief Determines if the given iterator is not equivalent
         * 
         * We check if the iterator is pointing to a different index
         * 
         * @param a Iterator to check
         * @return true If the two iterators are not equivalent
         * @return false If the two iterators are equivalent
         */
        bool operator!=(const C& a) { return !(*this == a); }

        /**
         * @brief Determines if the we are less than the given iterator
         * 
         * @param second Iterator to check
         * @return true If we are less than the given iterator
         * @return false If we are equal or greater than the given iterator 
         */
        bool operator<(const C& second) { return this->get_index() < second.get_index(); }

        /**
         * @brief Determines if we are greater than the given iterator 
         * 
         * @param second Iterator to check
         * @return true If we are greater than the given iterator
         * @return false If we are less than or equal to the given iterator
         */
        bool operator>(const C& second) { return this->get_index() > second.get_index(); }

        /**
         * @brief Determines if we are less than or equal to the given iterator
         * 
         * @param second Iterator to check
         * @return true If we are less than or equal to the given iterator
         * @return false If we are greater than the given iterator
         */
        bool operator<=(const C& second) { return this->get_index() <= second.get_index(); }

        /**
         * @brief Determines if we are greater than or equal to the given iterator
         * 
         * @param second Iterator to check
         * @return true If we are greater than or equal to the given iterator
         * @return false If we are less than or equal to the given iterator
         */
        bool operator>=(const C& second) { return this->get_index() >= second.get_index(); }

        /**
         * @brief Subscripting operator
         * 
         * We basically just set the index to the given value,
         * and return the current value.
         * 
         * @param val New index to set
         * @return reference Current value
         */
        reference operator[](int val) { this->set_index(val); return *(this->point); }

        /**
         * @brief Gets the current sample
         * 
         * @return reference 
         */
        reference operator*() const { return *(this->point); }

        /**
         * @brief Gets the current pointer
         * 
         * @return pointer 
         */
        pointer operator->() const { return this->base(); }

        /**
         * @brief Gets the pointer to the current sample
         * 
         * @return pointer 
         */
        pointer base() const { return this->point; }

};

/**
 * @brief Class for holding audio data
 * 
 * This class allows for arbitrary audio data to be stored,
 * worked with, and recalled for later use.
 * 
 * We will support some common buffer operations,
 * and will provide a standardized method for accessing
 * and working with audio data.
 * TODO: Add more once implemented
 * 
 * Please note, that this class contains vectors that represents different channels
 * of audio data, allowing for multi-channel audio to be stored and worked with.
 * We try to encapsulate this data as much as possible, and provide easy entry points into it.
 * These audio channels are simply vectors of long doubles.
 * 
 * Multi channel audio can be represented in a number of ways.
 * Conventionally, it can be represented as a vector of vectors,
 * with each sub-vector representing a channel.
 * We will call this the 'split' format, as each channel is split into its own vector.
 * 
 * Suppose we have the following audio data:
 * 
 * [1] - 1,2,3
 * [2] - 4,5,6
 * [3] - 7,8,9
 * 
 * Where [n] is the nth channel, and the numbers are the samples.
 * Each channel is concurrent, meaning that each sample is encountered 
 * in each channel at the same time.
 * To represent this data using in a split format, we would have:
 * 
 * [
 *  [1,2,3],
 *  [4,5,6],
 *  [7,8,9]
 * ]
 * 
 * Where each [] represents a vector. Again, numbers are samples.
 * To access the 2nd sample of the 3rd channel, we would index the vector as follows:
 * 
 * data[2][1]
 * 
 * Where 'data' is the audio in split format,
 * i.e the vector of vectors.
 * 
 * Another method of representing audio data is the 'interleaved' format,
 * where multiple channels are represented (squished) as one stream:
 * 
 * [1,4,7,2,5,8,3,6,9]
 * 
 * Note that the sample in each channel from the same time
 * is represented before samples from later times.
 * To access the 2nd sample of the 3rd channel, we would index the vector as follows:
 * 
 * data[7]
 * 
 * Where 'data' is the audio in squished format,
 * i.e the vector of samples.
 * Their are many ways to represent squished audio data!
 * The most common way to represent squished audio data is the
 * interleaved format, where the samples that occur at the same index
 * in each channel are grouped together in the order channels are encountered.
 * The example above is in interleaved format.
 * Another popular format is the sequential format, where the samples
 * are separated by channel:
 * 
 * [1,2,3,4,5,6,7,8,9]
 * 
 * Squished format is popular when outputting audio data,
 * be it a speaker, file, or other method.
 * However, this method can be difficult to work with,
 * as it is not immediately clear which samples belong to which channel.
 * Therefore, it is not recommended to use this format when working with audio data in the chain.
 * 
 * You can convert between these formats using 'squishers' (for squishing multiple channels into one stream)
 * and splitters (for splitting a single stream into multiple channels).
 * You can find more info on those operations elsewhere in the documentation TODO: Add link once implemented
 * 
 * By default, we keep our audio data in split format.
 * The size of the channels MUST be the same!
 * Otherwise, we will run into lot's of problems.
 * 
 * Sometimes it is useful to access the raw data
 * without any regard to channels.
 * This class offers some methods for accessing the raw audio data
 * without having to worry about the complexity of channels.
 * 
 * Some of these iterators are:
 * 
 * sequential_iterator - Iterates over each channel sequentially (see below)
 * interlaced_iterator - Iterates over each channel in an interleaved manner (again, see below)
 * group_iterator - Iterates over each channel and provdes a group of samples from each channel (please, see below)
 * 
 * Each iterator can be useful for different reasons and purposes.
 * Please understand the differences between each iterator and how
 * each operation could be useful!
 * 
 * Finally, this class contains some info about the audio data itself,
 * such as sample rate, format (TODO: Figure this out),
 * number of samples, weather we are interlaced, ect.
 * 
 * TODO:
 * 
 * Function names for iterators are kinda weird?
 * Like why get_position instead of get_sample()?
 * 
 * Need to really hash out terminology here.
 * 
 * Hash out reverse iterators!
 * Also implement reverse constant iterators?
 * 
 */
class AudioBuffer {

    private:

        /// Sample rate in Hertz
        double sample_rate = 44100;

    protected:

        /// The underlying vector of audio data
        std::vector<AudioChannel> buff;

    public:

        /**
         * @brief An iterator that iterates over audio data sequentially
         *  
         * This iterator iterates over the audio data sequentially,
         * meaning that it gets iterates over all audio data in a channel 
         * before moving on to the next channel, in order of channels.
         * 
         * For example, if we have the following audio data:
         * 
         * [0] - 1, 2, 3,
         * [1] - 4, 5, 6,
         * [2] - 7, 8, 9,
         * 
         * Where [n] represents the nth channel, and the numbers
         * are the samples in each channel.
         * 
         * This iterator will return the following values:
         * 
         * 1, 2, 3, 4, 5, 6, 7, 8, 9
         * 
         * We talk about the index of this iterator in the documentation.
         * This iterator uses that value to determine which sample to return.
         * You can think of this as the index of the sample in the final squished vector.
         * Fro example, if I wanted to get sample '8' in channel 2 index 1,
         * then I would use index 7 (see squished vector above).
         * 
         * This iterator contains some useful helper methods
         * for determining the index of the iterator,
         * so you don't have to do the calculations yourself.
         * 
         * This iterator is useful if we need to apply the same operation to each channel,
         * and the order of each channel is important, or if we need the 'pure' audio data
         * without data from other channels mixed in.
         */
        template <typename T>
        class SeqIterator : public BaseAudioIterator<SeqIterator<T>, T> {

            public:

                /**
                 * Constructor
                 * 
                 * We need to know the AudioBuffer we are iterating over,
                 * and the position to start at.
                 * 
                 * @param buff The AudioBuffer we are iterating over
                 * @param pos The index to start at
                */
                SeqIterator(AudioBuffer *buff, int pos=0) { this->buff = buff; this->set_index(pos); }

                /**
                 * @brief Get the Channel we are on
                 * 
                 * This returns the channel we are on.
                 * 
                 * @return int Channel we are on
                 */
                int get_channel() const { return int(this->get_index() / this->buff->buff[0].size()); }

                /**
                 * @brief Set the Channel we are on
                 * 
                 * This will alter the index of the iterator
                 * to the beginning of the specified channel.
                 * 
                 * This is bi-directional!
                 * We can move to any channel in any direction at any time.
                 * 
                 * @param channel Channel to move to.
                 */
                void set_channel(int channel) { this->set_index(channel * this->buff->buff[0].size()); }

                /**
                 * @brief Determines the pointer for this iterator
                 * 
                 * This method will automatically determine the correct pointer
                 * for this iterator at the current index.
                 * 
                 * This method is automatically called where appropriate,
                 * but you can call this method anytime if you wish
                 * to re-determine the underlying pointer in use by this iterator.
                 * 
                 * This method keeps the pointer for future use.
                 * When the value is requested,
                 * we simply return this value.
                 * This allows us to determine this value once upon index change
                 * and return it many times.
                 * 
                 */
                void resolve_pointer() { this->set_pointer((((this->buff->buff.begin() + this->get_channel())->begin()) + this->get_position()).base()); }

                /**
                 * @brief Sets the position of this iterator
                 * 
                 * This method will use the given values to set the index
                 * to the given position.
                 * The index if the sample in the squished value,
                 * while the position is the location of the sample in relation
                 * to the channels and samples within them.
                 * 
                 * Using the example above, if our index is 7,
                 * then our position is channel 2, sample 1.
                 * 
                 * This method can set the index to the given position,
                 * which can be useful if you wish to jump to a specific place
                 * and don't want to do the calculations yourself.
                 * 
                 * Here is the equation for determining the index:
                 * 
                 * index = (channel * size) + sample
                 * 
                 * Where 'size' is the size of each channel.
                 * 
                 * @param channel Channel of the position
                 * @param sample Index of the position within the given channel
                 */
                void set_position(int channel, int sample) { this->set_index(channel * this->buff->buff[0].size() + sample); }

                /**
                 * @brief Gets the position of this iterator
                 * 
                 * As stated above, the position is the location of the sample
                 * in relation to the channels and samples within them.
                 * 
                 * This method will return the position of the iterator 
                 * in the current channel.
                 * Using the above example, if your index is 7,
                 * then this method will return 1, as that is the index
                 * of the sample in the channel we are working with.
                 * 
                 * To get the channel we are working with, use GetChannel().
                 * This is needed if you want a full understanding of the position!
                 * 
                 * TODO: Is this too complicated?
                 * Maybe return a pair with channel and sample instead?
                 * Calling another function to get the channel might be too much.
                 * 
                 * ALSO FIGURE OUT NAMING SYSTEM!
                 * 
                 * WHat is a position? Sample? Index?
                 * The only thing I know for sure is channel...
                 * 
                 * @return int The position of the iterator in the current channel
                 */
                int get_position() const { return int(this->get_index() % this->buff->buff[0].size()); }

            private:

                /// Audio Buffer we are iterating over
                AudioBuffer* buff;

        };

        /**
         * @brief An iterator that iterates over audio data in an interleaved manner
         * 
         * This iterator iterates over audio data in an interleaved manner,
         * meaning that it iterates over each sample in each channel that occurs at the same time
         * before moving on to the next sample.
         * 
         * For example, if we have the following audio data:
         * 
         * [1] - 1, 2, 3,
         * [2] - 4, 5, 6,
         * [3] - 7, 8, 9,
         * 
         * Where [n] represents the nth channel, and the numbers
         * are the samples in each channel.
         * 
         * This iterator will return the following values:
         * 
         * 1, 4, 7, 2, 5, 8, 3, 6, 9
         * 
         * We talk about the index of this iterator in the documentation.
         * This iterator uses that value to determine which sample to return.
         * You can think of this as the index of the sample in the final squished vector.
         * Fro example, if I wanted to get sample '8' in channel 2 index 1,
         * then I would use index 5 (see squished vector above).
         * 
         * This iterator contains some useful helper methods
         * for determining the index of the iterator,
         * so you don't have to do the calculations yourself.
         * 
         * This iterator is useful if we need to apply the same operation to each channel,
         * and the order of each channel is not important.
         * This format is a very popular format for outputting audio data,
         * as many libraries represent audio data in this format.
         * 
         * TODO: See if we should add extra methods, like in SeqIterator...
         */
        template <typename T>
        class InterIterator : public BaseAudioIterator<InterIterator<T>, T> {

            public:

                /**
                 * @brief Construct a new Inter Iterator object
                 * 
                 * We need to know the AudioBuffer to iterate over,
                 * and the starting position
                 * 
                 * @param buff AudioBuffer we are iterating over
                 * @param pos Starting position
                 */
                InterIterator(AudioBuffer *buff, int pos=0) { this->buff = buff; this->set_index(pos); }

                /**
                 * @brief Determines the pointer for this iterator
                 * 
                 * This method will automatically determine the correct pointer
                 * for this iterator at the current index.
                 * 
                 * This method is automatically called where appropriate,
                 * but you can call this method anytime if you wish
                 * to re-determine the underlying pointer in use by this iterator.
                 * 
                 * This method keeps the pointer for future use.
                 * When the value is requested,
                 * we simply return this value.
                 * This allows us to determine this value once upon index change
                 * and return it many times.
                 * 
                 */
                void resolve_pointer() { this->set_pointer((this->buff->buff.at(this->get_channel()).begin() + this->get_sample()).base()); }

                /**
                 * @brief Gets the current channel we are on
                 * 
                 * Gets the current channel number we are on.
                 * 
                 * @return int The current channel
                 */
                int get_channel() const { return this->get_index() % this->buff->buff.size(); }

                /**
                 * @brief Get the current sample we are on
                 * 
                 * Gets the current sample we are on.
                 * 
                 * @return int Current sample
                 */
                int get_sample() const { return int(this->get_index() / this->buff->buff.size()); }

                /**
                 * @brief Sets the sample we are on
                 * 
                 * This will alter the index to the start of the current sample.
                 * 
                 * It is probably easier to show rather than tell.
                 * Consider this audio data:
                 * 
                 * [1]: 1, 2, 3
                 * [2]: 4, 5, 6
                 * [3]: 7, 8, 9
                 * 
                 * And here is that data in interleaved format:
                 * 
                 * 1, 4, 7, 2, 5, 8, 3, 6, 9
                 * 
                 * If you wish to seek to sample 2, 
                 * then we will set the index to 3.
                 * If you wish to seek to sample 3,
                 * then we will set the index to 6.
                 * 
                 * As you can see, each 'section' is the Nth sample
                 * of each channel in order.
                 * So the 2nd section of the interleaved format will be these values:
                 * 
                 * 2, 5, 8
                 * 
                 * Here is the formula for determining this index:
                 * 
                 * index = sample * channels
                 * 
                 * Where channels is the number of channels.
                 * 
                 * @param sample Sample to set this iterator to
                 */
                void set_sample(int sample) { this->set_index(this->buff->get_channel_count() * sample); }

                /**
                 * @brief Sets the position of this iterator
                 * 
                 * This method will use the given value to determine
                 * the position of this iterator.
                 * If the index is the location in the squished data,
                 * then the position is the location in relation to
                 * the channels and the samples within them.
                 * 
                 * You can use this method to seek to any channel and sample
                 * in the squished data if you don't want to do the calculations yourself.
                 * 
                 * For example, if we seek to channel 2 sample 2,
                 * we will end up at index 4, with value '5'.
                 * 
                 * Here is the formula for determining the index:
                 * 
                 * index = sample * channels + channel
                 * 
                 * Where channels are the number of channels.
                 * 
                 * @param channel Channel to seek to
                 * @param sample Sample to seek to
                 */
                void set_position(int channel, int sample) { this->set_index(this->buff->get_channel_count() * sample + channel); }

            private:

                /// Buffer we are iterating over
                AudioBuffer *buff;
        };

        /**
         * @brief No default constructor!
         * 
         * We require at least the channel number and buffer size be specified
         * 
         */
        AudioBuffer()=delete;

        /**
         * @brief Construct a new Audio Buffer object
         * 
         * @param size The size of each channel, AKA the number of samples per channel
         * @param channels The number of channels in this buffer, by default 1
         */
        AudioBuffer(int size, int channels=1);

        /**
         * @brief Construct a new Audio Buffer object
         * 
         * We create an AudioBuffer via the given vector.
         * This vector is one dimensional, 
         * so we assume that there is only one channel
         * 
         * We automatically determine the size from the given vector,
         * and assume there is only one channel.
         * 
         * @param vect Vector of samples
         */
        AudioBuffer(std::vector<long double> vect);

        /**
         * @brief Construct a new Audio Buffer object
         * 
         * We create an AudioBuffer via the given vector.
         * This vector is two dimensional,
         * meaning that it is a vector of channels
         * that contain samples.
         * 
         * We automatically determine the channels number and size this
         * AudioBuffer using the given vector
         * 
         * @param vect Vector of channels
         */
        AudioBuffer(std::vector<AudioChannel> vect) { this->buff = vect; }

        /**
         * @brief Set the Sample Rate of this buffer
         * 
         * Sets the sample rate for this AudioBuffer.
         * 
         * Please note, we will not be doing checks of any kind
         * to ensure this is accurate!
         * Any component using this buffer will most likely 
         * use this sample rate at face value.
         * With that being said,
         * please only set the sample rate to accurate values.
         * If your component has nothing to do with
         * resampling, then please do not change this value.
         * 
         * @param rate New sample rate to set
         */
        void set_samplerate(double rate) { this->sample_rate = rate; }

        /**
         * @brief Get the samplerate of this buffer
         * 
         * Returns the reported sample rate of this buffer.
         * It can be assumed that the reported sample rate 
         * is accurate to the audio buffer.
         * 
         * @return double The current samplerate
         */
        double get_samplerate() const { return this->sample_rate; }

        /**
         * @brief Pre-allocates the buffer to a certain size
         * 
         * This tells the underlying vector to allocate memory
         * before hand, which leads to faster performance
         * when writing (and reading, but to a lesser degree...),
         * so it is recommended to allocate the vector before any operations.
         * 
         * See: https://cplusplus.com/reference/vector/vector/reserve/
         * For more info.
         * 
         * @param size Size to pre-allocate
         */
        void reserve(int size) { buff.reserve(size); }

        /**
         * @brief Shrinks the vector to it's current size
         * 
         * This tells the underlying vector to un-allocate 
         * it's memory to fit it's current contents.
         * This can save some memory, if we have more memory allocated then values in our buffer.
         * 
         * See: https://cplusplus.com/reference/vector/vector/shrink_to_fit/
         * For more info.
         * 
         */
        void shrink();

        /**
         * @brief Gets the size of this buffer.
         * 
         * We report the size of the individual channels,
         * not the number of channels.
         * 
         * @return int Size of each channel
         */
        unsigned int size() { return this->buff.at(0).size(); }

        /**
         * @brief Gets a channel in this buffer
         * 
         * Returns a channel in this buffer for use.
         * You can alter and edit this channel as you see fit,
         * but please remember the size rule!
         * 
         * @param pos Channel number to retrieve
         * @return AudioChannel AudioChannel at the given position
         */
        AudioChannel* at(int pos) { return &(this->buff.at(pos)); }

        /**
         * @brief Returns the number of channels in this buffer
         * 
         * @return int Number of channels
         */
        int get_channel_count() const { return this->buff.size(); }

        /**
         * @brief Gets the start channel iterator for this buffer
         * 
         * Returns the start iterator for proper channel iteration.
         * A channel iterator is an iterator pointing to an AudioChannel type,
         * which is a vector of long doubles.
         * 
         * If you need access to each channel for iteration, 
         * then this iterator is for you!
         * However, if you wish to access the raw audio data
         * without any regard to the underlying channels,
         * then you probably want to use the TODO: Fill in methods once written!
         * 
         * @return std::vector<long double>::iterator 
         */
        std::vector<AudioChannel>::iterator chbegin() { return this->buff.begin(); }

        /**
         * @brief Gets the end channel iterator for this buffer
         * 
         * Returns the end iterator for proper channel iteration.
         * This is useful for determining when to stop iterating over channels.
         * 
         * As stated above, this iterator is for iterating over audio channels!
         * If you wish to work with audio data without regard to channels,
         * then this is not the iterator for you!
         * 
         * @return std::vector<long double>::iterator 
         */
        std::vector<AudioChannel>::iterator chend() { return this->buff.end(); }

        /**
         * @brief Gets the start sequential iterator for this buffer
         * 
         * Returns the start iterator for proper sequential sample iteration.
         * We iterate over each sample in each channel sequentially,
         * meaning that we iterate over each channel in order until we move onto the next.
         * See the documentation for the SeqIterator class for more info.
         * 
         * This iterator is useful for iterating over the raw audio data,
         * without regard to the underlying channels.
         * 
         * @return AudioBuffer::SeqIterator<long double>
         */
        AudioBuffer::SeqIterator<long double> sbegin() { return AudioBuffer::SeqIterator<long double>(this); }

        /**
         * @brief Gets the end sequential iterator for this buffer
         * 
         * Returns the end iterator for sequential iteration.
         * This is useful for determining when to stop iterating over samples.
         * 
         * @return AudioBuffer::SeqIterator<long double>
         */
        AudioBuffer::SeqIterator<long double> send() { return AudioBuffer::SeqIterator<long double>(this, this->buff[0].size() * this->buff.size()); }

        /**
         * @brief Gets the start reverse iterator for this buffer
         * 
         * Returns the reverse start iterator for proper sequential sample iteration.
         * We iterate over samples sequentially, but in reverse order,
         * meaning that this start iterator will initially point to the last value
         * in this buffer.
         * 
         * @return std::reverse_iterator<AudioBuffer::SeqIterator<long double>> 
         */
        std::reverse_iterator<AudioBuffer::SeqIterator<long double>> srbegin() { return std::reverse_iterator(AudioBuffer::SeqIterator<long double>(this, (this->buff[0].size() * this->buff.size())-1)); }

        /**
         * @brief Gets the end reverse iterator for this buffer
         * 
         * Returns the reverse sequential end iterator for this buffer.
         * Useful for determining when to stop iterating over samples in reverse.
         * 
         * @return std::reverse_iterator<AudioBuffer::SeqIterator<long double>> 
         */
        std::reverse_iterator<AudioBuffer::SeqIterator<long double>> srend() { return std::reverse_iterator(AudioBuffer::SeqIterator<long double>(this)); }

        /**
         * @brief Gets the constant start sequential iterator for this buffer
         * 
         * Returns the constant start iterator for proper sequential sample iteration.
         * We are identical to the normal sequential iterator,
         * but we are marked as constant, so the contents of the buffer can't be altered using this iterator.
         * 
         * @return AudioBuffer::SeqIterator<const long double> 
         */
        AudioBuffer::SeqIterator<const long double> scbegin() { return AudioBuffer::SeqIterator<const long double>(this); }

        /**
         * @brief Gets the end constant sequential iterator for this buffer
         * 
         * Returns the constant end iterator for sequential iteration.
         * Useful for determining when to stop iterating over samples.
         * 
         * Again, we are constant, so values can't be edited using this iterator.
         * 
         * @return AudioBuffer::SeqIterator<const long double> 
         */
        AudioBuffer::SeqIterator<const long double> scend() { return AudioBuffer::SeqIterator<const long double>(this, this->buff[0].size() * this->buff.size()); }

        /**
         * @brief Gets the start interleaved iterator for this buffer
         * 
         * Returns the start iterator for proper interleaved sample iteration.
         * We iterate over each sample in each channel in an interleaved manner,
         * meaning tht we iterate over each sample that occurs at the same time in order
         * of channels before moving onto the next sample.
         * 
         * This iterator is useful for iterating over the raw audio data,
         * without regard to the underlying channels.
         * 
         * @return AudioBuffer::InterIterator 
         */
        AudioBuffer::InterIterator<long double> ibegin() { return AudioBuffer::InterIterator<long double>(this); }

        /**
         * @brief Gets the end interleaved iterator for this buffer
         * 
         * Returns the end iterator for interleaved iteration.
         * This is useful for determining when to stop iterating over samples.
         * 
         * @return AudioBuffer::InterIterator 
         */
        AudioBuffer::InterIterator<long double> iend() { return AudioBuffer::InterIterator<long double>(this, this->buff[0].size() * this->buff.size()); }

        /**
         * @brief Gets the start reverse sequential iterator for this buffer
         * 
         * Returns the reverse start interleaved iterator for this buffer.
         * We iterate over samples in an interleaved manner, but in reverse order,
         * meaning that this iterator will initially point to the last value in the buffer.
         * 
         * @return std::reverse_iterator<AudioBuffer::SeqIterator<long double>> 
         */
        std::reverse_iterator<AudioBuffer::InterIterator<long double>> irbegin() { return std::reverse_iterator(AudioBuffer::InterIterator<long double>(this, (this->buff[0].size() * this->buff.size())-1)); }

        /**
         * @brief Gets the end interleaved reverse iterator for this buffer
         * 
         * Returns the end reversed interleaved iterator for this buffer.
         * Useful for determining when to stop iterating.
         * 
         * @return std::reverse_iterator<AudioBuffer::SeqIterator<long double>> 
         */
        std::reverse_iterator<AudioBuffer::InterIterator<long double>> irend() { return std::reverse_iterator(AudioBuffer::InterIterator<long double>(this)); }

        /**
         * @brief Gets the constant start interleaved iterator for this buffer
         * 
         * Returns the start iterator for proper interleaved sample iteration.
         * We are identical to the normal interleaved iterator,
         * but we are marked as constant, so the contents of the buffer can't be altered using this iterator.
         * 
         * @return AudioBuffer::InterIterator<const long double> 
         */
        AudioBuffer::InterIterator<const long double> icbegin() { return AudioBuffer::InterIterator<const long double>(this); }

        /**
         * @brief Gets the end constant iterator for this buffer
         * 
         * Returns the constant end iterator for interleaved iteration.
         * Useful for determining when to stop iterating over samples.
         * 
         * Again, we are constant, so values can't be edited using this iterator.
         * 
         * @return AudioBuffer::InterIterator<const long double> 
         */
        AudioBuffer::InterIterator<const long double> icend() { return AudioBuffer::InterIterator<const long double>(this, this->buff[0].size() * this->buff.size()); }

        /**
         * @brief Default start iterator
         * 
         * We simply return the start sequential iterator.
         * 
         * @return AudioBuffer::SeqIterator<long double> 
         */
        AudioBuffer::SeqIterator<long double> begin() { return this->sbegin(); }

        /**
         * @brief Default stop iterator
         * 
         * We simply return the stop sequential iterator
         * 
         * @return AudioBuffer::SeqIterator<long double> 
         */
        AudioBuffer::SeqIterator<long double> end() { return this->send(); }
};
