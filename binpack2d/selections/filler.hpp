#ifndef FILLER_HPP
#define FILLER_HPP

#include "../binpack2d.hpp"

namespace binpack2d { namespace strategies {

template<class RawShape>
class _FillerSelection {
public:
    using Item = _Item<RawShape>;
    using ItemRef = std::reference_wrapper<Item>;
    using ItemGroup = std::vector<ItemRef>;
    using PackGroup = std::vector<ItemGroup>;
    using Config = int; //dummy

private:
    using Container = ItemGroup;

    Container store_;
    PackGroup packed_bins_;

public:

    void configure(const Config& /*config*/) { }

    template<class TPlacer, class TIterator,
             class TBin = typename PlacementStrategyLike<TPlacer>::BinType,
             class PConfig = typename PlacementStrategyLike<TPlacer>::Config>
    void packItems(TIterator first,
                   TIterator last,
                   TBin&& bin,
                   PConfig&& pconfig = PConfig())
    {

        store_.clear();
        store_.reserve(last-first);
        packed_bins_.clear();

        std::copy(first, last, std::back_inserter(store_));

        auto sortfunc = [](Item& i1, Item& i2) {
            return i1.area() > i2.area();
        };

        std::sort(store_.begin(), store_.end(), sortfunc);

        Container a = {store_[0], store_[1], store_[4], store_[5] };
//        a.insert(a.end(), store_.end()-10, store_.end());
        store_ = a;

        PlacementStrategyLike<TPlacer> placer(bin);
        placer.configure(pconfig);

        bool was_packed = false;
        for(auto& item : store_ ) {
            if(!placer.pack(item))  {
                packed_bins_.push_back(placer.getItems());
                placer.clearItems();
                was_packed = placer.pack(item);
            } else was_packed = true;
        }

        if(was_packed) {
            packed_bins_.push_back(placer.getItems());
        }
    }

    size_t binCount() const { return packed_bins_.size(); }

    ItemGroup itemsForBin(size_t binIndex) {
        assert(binIndex < packed_bins_.size());
        return packed_bins_[binIndex];
    }

};
}
}

#endif //BOTTOMLEFT_HPP
