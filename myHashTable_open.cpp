#include <functional>
#include <iterator>
#include <initializer_list>
#include <list>
#include <stdexcept>
#include <utility>
#include <vector>



template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
private:
    int SIZE_OF_TABLE;
    std::list<std::pair<const KeyType, ValueType>> elems;
    std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator> data;
    std::vector<std::pair<bool, bool>> flags;
    Hash hasher;
    size_t _size;

public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

    HashMap(Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                (SIZE_OF_TABLE);
        flags = std::vector<std::pair<bool, bool>> (SIZE_OF_TABLE, {false, false});
    }


    template <typename Iter>
    HashMap(Iter begin, Iter end, Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                (SIZE_OF_TABLE);
        flags = std::vector<std::pair<bool, bool>> (SIZE_OF_TABLE, {false, false});
        while (begin != end) {
            insert(*begin);
            begin++;
        }
    }

    HashMap& operator= (const HashMap& other) {
        HashMap old_other (other.begin(), other.end(), other.hasher);
        clear();
        hasher = old_other.hasher;
        for (const auto& el : old_other.elems) {
            insert(el);
        }
        return *this;
    }

    HashMap(const HashMap& other)
            : hasher(other.hasher)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                (SIZE_OF_TABLE);
        flags = std::vector<std::pair<bool, bool>> (SIZE_OF_TABLE, {false, false});
        for (const auto& el : other.elems) {
            insert(el);
        }
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> l, Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                (SIZE_OF_TABLE);
        flags = std::vector<std::pair<bool, bool>> (SIZE_OF_TABLE, {false, false});
        for (const auto& elem : l)
            insert(elem);
    }

    size_t size() const {
        return _size;
    }

    bool empty() const {
        return _size == 0;
    }

    Hash hash_function() const {
        return hasher;
    }

    void resize() {
        std::list<std::pair<const KeyType, ValueType>> prev_elems = elems;
        SIZE_OF_TABLE = 2 * SIZE_OF_TABLE + 1;
        data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                (SIZE_OF_TABLE);
        flags = std::vector<std::pair<bool, bool>> (SIZE_OF_TABLE, {false, false});
        elems.clear();
        _size = 0;
        for (const auto& el : prev_elems) {
            insert(el);
        }
    }

    void insert(std::pair<const KeyType, ValueType> elem) {
        size_t index = hasher(elem.first) % SIZE_OF_TABLE;
        bool to_add = true;
        for (int ii = 0; ii < SIZE_OF_TABLE; ++ii) {
            if (!flags[index].second) {
                to_add = true;
                break;
            }
            if (flags[index].first) {
                if (data[index]->first == elem.first) {
                    to_add = false;
                    break;
                }
            } else {
                to_add = true;
                break;
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
        if (to_add) {
            elems.push_back(elem);
            data[index] = std::prev(elems.end());
            flags[index] = {true, true};
            _size++;
        }
        if (_size >= static_cast<int>(2 * SIZE_OF_TABLE / 3))
            resize();
    }

    void erase(const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (int ii = 0; ii < SIZE_OF_TABLE; ++ii) {
            if (!flags[index].second) {
                break;
            }
            if (flags[index].first) {
                if (data[index]->first == key) {
                    elems.erase(data[index]);
                    flags[index].first = false;
                    _size--;
                    break;
                }
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
    }

    iterator begin() {
        return elems.begin();
    }

    iterator end() {
        return elems.end();
    }

    const_iterator begin() const {
        return elems.cbegin();
    }

    const_iterator end() const {
        return elems.cend();
    }

    const_iterator find(const KeyType& key) const {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (int i = 0; i < SIZE_OF_TABLE; ++i) {
            if (!flags[index].second)
                break;
            if (flags[index].first) {
                if (data[index]->first == key)
                    return data[index];
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
        return end();
    }

    iterator find(const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (int i = 0; i < SIZE_OF_TABLE; ++i) {
            if (!flags[index].second)
                break;
            if (flags[index].first) {
                if (data[index]->first == key)
                    return data[index];
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
        return end();
    }

    ValueType& operator[] (const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (int i = 0; i < SIZE_OF_TABLE; ++i) {
            if (!flags[index].second)
                break;
            if (flags[index].first) {
                if (data[index]->first == key)
                    return data[index]->second;
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
        insert({key, ValueType()});
        return (*this)[key];
    }

    const ValueType& at(const KeyType& key) const {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (int i = 0; i < SIZE_OF_TABLE; ++i) {
            if (!flags[index].second)
                break;
            if (flags[index].first) {
                if (data[index]->first == key)
                    return data[index]->second;
            }
            index = (index + 1) % SIZE_OF_TABLE;
        }
        throw std::out_of_range("");
    }

    void clear() {
        if (_size > 0) {
            data = std::vector<typename std::list<std::pair<const KeyType, ValueType>>::iterator>
                    (2);
            flags = std::vector<std::pair<bool, bool>> (2, {false, false});
            SIZE_OF_TABLE = 2;
            elems.clear();
            _size = 0;
        }
    }
};

