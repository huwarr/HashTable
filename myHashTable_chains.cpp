#include <functional>
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
    std::vector<std::list<typename std::list<std::pair<const KeyType, ValueType>>::iterator>> data;
    Hash hasher;
    size_t _size;

public:
    using iterator = typename std::list<std::pair<const KeyType, ValueType>>::iterator;
    using const_iterator = typename std::list<std::pair<const KeyType, ValueType>>::const_iterator;

    HashMap(Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector < std::list <
                typename std::list<std::pair<const KeyType, ValueType>>::iterator>>(SIZE_OF_TABLE);
    }


    template <typename Iter>
    HashMap(Iter begin, Iter end, Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector < std::list <
                typename std::list<std::pair<const KeyType, ValueType>>::iterator>>(SIZE_OF_TABLE);
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
        data = std::vector < std::list <
                typename std::list<std::pair<const KeyType, ValueType>>::iterator>>(SIZE_OF_TABLE);
        for (const auto& el : other.elems) {
            insert(el);
        }
    }

    HashMap(std::initializer_list<std::pair<const KeyType, ValueType>> l, Hash h = Hash())
            : hasher(h)
            , _size(0)
            , SIZE_OF_TABLE (2) {
        data = std::vector < std::list <
                typename std::list<std::pair<const KeyType, ValueType>>::iterator>>(SIZE_OF_TABLE);
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
        data = std::vector < std::list < typename
                std::list<std::pair<const KeyType, ValueType>>::iterator>>(SIZE_OF_TABLE);
        elems.clear();
        _size = 0;
        for (const auto& el : prev_elems) {
            insert(el);
        }
    }

    void insert(std::pair<const KeyType, ValueType> elem) {
        size_t index = hasher(elem.first) % SIZE_OF_TABLE;
        bool to_add = true;
        for (const auto& el : data[index]) {
            if (el->first == elem.first) {
                to_add = false;
                break;
            }
        }
        if (to_add) {
            elems.push_back(elem);
            auto it = elems.end();
            data[index].push_back((--it));
            _size++;
        }
        if (_size >= SIZE_OF_TABLE)
            resize();
    }

    void erase(const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (auto it = data[index].begin(); it != data[index].end(); ++it) {
            if ((*it)->first == key) {
                elems.erase(*it);
                data[index].erase(it);
                _size--;
                break;
            }
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
        for (auto it = data[index].begin(); it != data[index].end(); ++it) {
            if ((*it)->first == key) {
                return (*it);
            }
        }
        return end();
    }

    iterator find(const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (auto it = data[index].begin(); it != data[index].end(); ++it) {
            if ((*it)->first == key) {
                return (*it);
            }
        }
        return end();
    }

    ValueType& operator[] (const KeyType& key) {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (const auto& elem : data[index]) {
            if (elem->first == key) {
                return elem->second;
            }
        }
        std::pair<const KeyType, ValueType> elem ({key, ValueType()});
        insert(elem);
        index = hasher(key) % SIZE_OF_TABLE;
        return data[index].back()->second;
    }

    const ValueType& at(const KeyType& key) const {
        size_t index = hasher(key) % SIZE_OF_TABLE;
        for (const auto& elem : data[index]) {
            if (elem->first == key) {
                return elem->second;
            }
        }
        throw std::out_of_range("");
    }

    void clear() {
        if (_size > 0) {
            data = std::vector < std::list < typename
                    std::list<std::pair<const KeyType, ValueType>>::iterator>>(2);
            SIZE_OF_TABLE = 2;
            elems.clear();
            _size = 0;
        }
    }
};

