#pragma once

#include <iterator>
#include <stdexcept>

template <typename E>
class LongMap {

    private:
        static const int DEFAULT_INITIAL_CAPACITY = 128;
        static constexpr float DEFAULT_LOAD_FACTOR = 0.80f;

        template <typename T>
        struct Entry {
            long key;
            T value;
            Entry<T>* next;
        };

        Entry<E>** data;
        int lengthMinusOne;
        int count;
        int threshold;
        float loadFactor;
        Entry<E>* poolHead;
        long currIteratorKey;

        void rehash();

        Entry<E>* getEntryFromPool(long key, E value, Entry<E>* next);

        void releaseEntryBackToPool(Entry<E>* e);

        int toArrayIndex(long key);

    public:
        LongMap();
        LongMap(int initialCapacity);
        LongMap(int initialCapacity, float loadFactor);
        ~LongMap();

        // Copy constructor and assignment operator are deleted to avoid unintended behavior
        LongMap(const LongMap&) = delete;
        LongMap& operator=(const LongMap&) = delete;

        int size() const;
        bool isEmpty() const;
        bool contains(const E& value) const;
        bool containsKey(long key) const;
        E get(long key) const;
        E put(long key, const E& value);
        E remove(long key);
        void clear();

        class ReusableIterator : public std::iterator<std::input_iterator_tag, E> {

            private:
                int size;
                int index;
                int dataIndex;
                Entry<E>* prev;
                Entry<E>* next;
                Entry<E>* entry;
                bool wasRemoved;

            public:
                ReusableIterator();
                ~ReusableIterator() = default;

                void reset();
                bool hasNextPtr() const;
                E nextPtr();
                void remove();
        };

        ReusableIterator iterator();
};