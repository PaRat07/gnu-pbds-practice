#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>


using namespace std;


template<typename KeyT>
struct MetaT {
    size_t sz;
    KeyT sum;
};

#define POLICY_BASE __gnu_pbds::detail::branch_policy<NodeConstIt, NodeIt, Alloc>

template<typename NodeConstIt, typename NodeIt, typename Cmp, typename Alloc>
struct Policy : POLICY_BASE {
public:
    using base = POLICY_BASE;

#undef POLICY_BASE

    using KeyT = base::key_type;
    using metadata_type = MetaT<KeyT>;

    KeyT GetSum() {
        return GetSum(node_begin());
    }

protected:
    void operator()(NodeIt node_it, NodeConstIt end_nd_it) {
        NodeConstIt l_it = node_it.get_l_child();
        const size_t l_sz = (l_it == end_nd_it) ? 0 : GetMeta(l_it).sz;
        const KeyT l_sum = (l_it == end_nd_it) ? 0 : GetMeta(l_it).sum;

        NodeConstIt r_it = node_it.get_r_child();
        const size_t r_sz = (r_it == end_nd_it) ? 0 : GetMeta(r_it).sz;
        const KeyT r_sum = (r_it == end_nd_it) ? 0 : GetMeta(r_it).sz;


        GetMutMeta(node_it).sz = 1 + l_sz + r_sz;
        GetMutMeta(node_it).sum = l_sum + r_sum + **node_it;
    }

    // instead-of-crtp-shit
    virtual NodeConstIt node_begin() const = 0; // root
    virtual NodeConstIt node_end() const = 0; // leaf child

private:
    metadata_type GetMeta(NodeIt cit) {
        return cit.get_metadata();
    }
    metadata_type& GetMutMeta(NodeIt cit) {
        return (metadata_type&)cit.get_metadata();
    }

    static KeyT GetSum(NodeConstIt cit, NodeConstIt end) {
        return cit == end ? KeyT{} : cit.get_metadata().sum;
    }
};

template<typename KeyT>
using CT = __gnu_pbds::tree<KeyT, __gnu_pbds::null_type, std::less<>, __gnu_pbds::rb_tree_tag, Policy>;


// -> (less-or-equal, greater)
template<typename KeyT>
std::pair<CT<KeyT>, CT<KeyT>> SplitLeq(CT<KeyT> tr, KeyT key) {
    CT<KeyT> gr;
    tr.split(key, gr);
    return { std::move(tr), std::move(gr) };
}

template<typename KeyT>
CT<KeyT> Merge(CT<KeyT> le, CT<KeyT> gr) {
    le.join(gr);
    return le;
}

signed main() {
    CT<int> x;
    for (int i = 0; i < 10; ++i) {
        x.insert(i);
    }
    auto [leq5, gr5] = SplitLeq(std::move(x), 5);
    for (int i : leq5) {
        cout << i << " ";
    }
    cout << endl;
    x = Merge(std::move(leq5), std::move(gr5));
    for (int i : x) {
        cout << i << " ";
    }
    cout << endl;

    return 0;
}
