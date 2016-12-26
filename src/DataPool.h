#ifndef DATAPOOL_H_LN6HGAYS
#define DATAPOOL_H_LN6HGAYS
#include <deque>
#include <unordered_map>
#include <memory>

#include "boost/functional/hash/hash.hpp"

#include "make_unique.hpp"
namespace fovris {


template <typename T> class DataPool {
    typedef std::reference_wrapper<const T> MapKey;

    template <typename H = T> struct Hash_ {
        size_t operator()(const MapKey &r) const {
            return std::hash<H>()(r.get());
        }
    };

    template <typename A, typename B> struct Hash_<std::pair<A, B>> {
        size_t operator()(
            const std::reference_wrapper<const std::pair<A, B>> &r) const {
            std::size_t seed = 0;
            boost::hash_combine(seed, std::hash<A>()(r.get().first));
            boost::hash_combine(seed, std::hash<B>()(r.get().second));
            return seed;
        }
    };

    struct EqualTo_ {
        bool operator()(const MapKey &lhs, const MapKey &rhs) const {
            return lhs.get() == rhs.get();
        }
    };

    std::deque<T> storage_;
    std::unordered_map<MapKey, unsigned, Hash_<T>, EqualTo_> storageIdMap_;
    typedef typename std::unordered_map<
        MapKey, unsigned, Hash_<T>, EqualTo_>::const_iterator const_iterator;
    unsigned id_counter_ = 0;

  public:
    const_iterator begin() const { return storageIdMap_.begin(); }
    const_iterator end() const { return storageIdMap_.end(); }

    const T &get(unsigned id) const throw(std::out_of_range) {
        return storage_.at(id);
    }

    bool exists(const T &t) const {
        return storageIdMap_.find(MapKey(t)) != storageIdMap_.end();
    }

    unsigned queryId(const T &t) const throw(std::out_of_range) {
        return storageIdMap_.at(MapKey(t));
    }

    std::pair<unsigned, bool> insert(T t) {
        auto it = storageIdMap_.find(MapKey(t));
        if (it != storageIdMap_.end()) {
            return {it->second, false};
        }
        storage_.push_back(std::move(t));
        storageIdMap_.insert({storage_.back(), id_counter_});
        unsigned r = id_counter_;
        id_counter_ += 1;
        return {r, true};
    }
};

} // fovris
#endif /* end of include guard: DATAPOOL_H_LN6HGAYS */
