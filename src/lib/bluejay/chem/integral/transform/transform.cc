#include "bluejay/chem/integral/transform.h"
#include "bluejay/chem/integral/index.h"
#include "bluejay/core/eigen/einsum.h"
#include "bluejay/core/string.h"
#include "bluejay/core/debug.h"

namespace bluejay {
namespace integral {
namespace transform {

  static const std::array<char,4> PQRS = { 'p','q','r','s' };
  static const std::array<char,4> IJKL = { 'i', 'j', 'k', 'l' };

  template<size_t N>
  IndexShuffle<N> transform_loop_order(IndexList<size_t> s) {
    std::set<size_t> u = s;
    std::vector<size_t> order;
    for (size_t idx = 0; idx < N; ++idx) {
      if (u.count(idx)) continue;
      order.push_back(idx);
    }
    order.insert(order.end(), u.begin(), u.end());
    return IndexShuffle<N>(order.begin(), order.end());
  }

  struct IndexTransformEngine : boost::static_visitor<> {

    //IndexTransform<4> transform;
    const IndexShuffle<4> integral;
    const IndexShuffle<4> order;

    Tensor4d t0, t1, t2;

    IndexTransformEngine(std::array<size_t,4> integral, const IndexTransform<4> &transform)
      : //transform(transform),
        integral(integral),
        order(loop_order(transform))
    {
    }

    static IndexList<size_t> indices(const Transform2 &t2) {
      return { std::get<1>(t2).second, std::get<2>(t2).second };
    }

    static IndexList<size_t> indices(const std::tuple<Transform1,Transform1> &t2) {
      IndexList<size_t> indices;
      if (const auto &t = std::get<0>(t2)) indices.push_back(std::get<1>(*t).second);
      if (const auto &t = std::get<1>(t2)) indices.push_back(std::get<1>(*t).second);
      return indices;
    }

    static IndexShuffle<4> loop_order(const IndexTransform<4> &transforms) {

      auto indices_visitor = [](const auto &t12) {
        return indices(t12);
      };
      const auto &t12_indices = boost::apply_visitor(indices_visitor, transforms.T12);

      Assert(
        !t12_indices.repeated(),
        "Repeated transform indices %s", str(t12_indices.repeated())
      );
      std::vector<size_t> v;
      for (size_t idx = 0; idx < 4; ++idx) {
        if (t12_indices.count(idx)) continue;
        v.push_back(idx);
      }
      v.insert(v.end(), t12_indices.begin(), t12_indices.end());
      return IndexShuffle<4>(v.begin(), v.end());
    }

    const Tensor4d* transform01(const Transform2 &args) {
      //debug::print("transform01: dims=%s", std::get<0>(args)->dimensions());
      const auto &shape =  std::get<0>(args)->dimensions();
      for (int64_t l = 0; l < shape[3]; ++l) {
        Range<int64_t> ri(0,shape[0]);
        Range<int64_t> rj(0,shape[1]);
        Range<int64_t> rk(0,shape[2]);
        Range<int64_t> rl(l,l+1);
        std::get<0>(args)->get({ri,rj,rk,rl}, this->t1);
        // t2(i,j,k,l) += t0(p,q,k,l)*t1(i,j,k,l)
      }
      return &this->t2;
    }

    const Tensor4d* transform01(const std::tuple<Transform1,Transform1> &args) {
      auto C1 = std::get<0>(args);
      auto C2 = std::get<1>(args);
      const Tensor4d *tx = &this->t0;
      if (C2) {
        std::get<1>(*C2).second = 4-1;
        transform1(*C2, *tx, this->t2); // r,s,p,q -> q,r,s,p
        tx = &this->t2;
      }
      if (C1) {
        std::get<1>(*C1).second = 4-1;
        transform1(*C1, *tx, this->t1); // q,r,s,p -> p,q,r,s
        tx = &this->t1;
      }
      return tx;
    }

    template<class ... Args>
    const Tensor4d* transform_integral_block(const boost::variant<Args...> &args) {
      auto visitor = [this](auto &&args) {
        return this->transform01(args);
      };
      return boost::apply_visitor(visitor, args);
    }

    static void transform1(
      std::tuple<Matrix<double>, IndexPair> Cx,
      const Tensor4d &T,
      Tensor4d &U)
    {
      auto indices = std::get<1>(Cx);
      // debug::print(
      //   "contract: C=%s, T=%s, axis=%s",
      //   std::get<0>(Cx),
      //   T,
      //   indices
      // );
      einsum::einsum(std::get<0>(Cx), indices.first, T, indices.second, U);
      //debug::print("U=%s", str(U));
    }

    // evaluate t0 = shuffle(integral(index_shuffle(p,q,r,s)))
    void compute_integral_block(
      const Basis &basis,
      IntegralEngine::TwoBodyEngine &int2e,
      IndexShuffle<4> integral_index_shuffle,
      size_t k,
      size_t l)
    {

      typedef Tensor4d::Index Index;
      auto nbf = [](const auto &s) { return (Index)s.nbf(); };
      auto index = [](const auto &s) { return (Index)s.index; };

      this->t0.resize(nbf(basis[l]), nbf(basis[k]), nbf(basis), nbf(basis));

      // debug::print(
      //   "integral: loop_order=%s, braket=%s, t0=%s",
      //   loop_index_shuffle(PQRS),
      //   integral_index_shuffle(PQRS),
      //   t0
      // );

      auto ijkl_to_braket = integral_index_shuffle(this->order);
      auto braket_to_ijkl = (~ijkl_to_braket).reverse();

      for (size_t j = 0; j < basis.size(); ++j) {
        for (size_t i = 0; i < basis.size(); ++i) {

          auto braket = ijkl_to_braket({i,j,k,l});
          const auto &p = basis[braket[0]];
          const auto &q = basis[braket[1]];
          const auto &r = basis[braket[2]];
          const auto &s = basis[braket[3]];

          const double *V = int2e.compute(p,q,r,s);
          if (!V) return;

          std::array<Index,4> extent = { nbf(p), nbf(q), nbf(r), nbf(s) };

          TensorMap< Tensor<const double,4> > tmp(V, extent);

          std::array<Index,4> offset = { 0, 0, index(basis[j]), index(basis[i]) };

          // debug::print(
          //   "integral_batch: %s->%s: %s.slice(offset=%s, extent=%s) = %s.shuffle(%s)",
          //   std::array<Index,4>{i,j,k,l},
          //   loop_index_shuffle({i,j,k,l}),
          //   t0,
          //   offset,
          //   (~loop_index_shuffle)(extent),
          //   tmp,
          //   ~loop_index_shuffle
          // );

          this->t0.slice(offset, braket_to_ijkl(extent)) = tmp.shuffle(braket_to_ijkl);

        }
      }

    }

  };

  // T2(a,b,r,s) = C1(a,p)*(C2(b,q)*(p,q|r,s))
  void transform(
    const Basis &basis,
    IntegralEngine::TwoBodyEngine &int2e,
    std::array<size_t,4> integral_order,
    IndexTransform<4> transform)
  {

    IndexTransformEngine index_transform_engine(integral_order, transform);

    IndexShuffle<4> integral_index_shuffle = index_transform_engine.integral;
    IndexShuffle<4> loop_index_shuffle = index_transform_engine.order;

    //IndexShuffle<4> t2_order_shuffle = t2_order;

    // default output order is [a,b,k,l]
    std::array<char,4> abkl = {
      'a', 'b',
      loop_index_shuffle(PQRS)[0],
      loop_index_shuffle(PQRS)[1]
    };

    // default output order is [a,b,k,l]

    // debug::print(
    //   "transform: T2%s = C2[b,%c]*(C1[a,%c]*%s)"
    //   "\n           loop_shuffle =%s"
    //   "\n           T2=%s, t2_order=%s"
    //   "\n           loop order: i->%c, j->%c, k->%c l->%c",
    //   t2_order_shuffle(abkl),
    //   PQRS[std::get<1>(C2).second],
    //   PQRS[std::get<1>(C1).second],
    //   integral_index_shuffle(PQRS),
    //   loop_index_shuffle,
    //   T2,
    //   t2_order_shuffle,
    //   loop_index_shuffle(PQRS)[3],
    //   loop_index_shuffle(PQRS)[2],
    //   loop_index_shuffle(PQRS)[1],
    //   loop_index_shuffle(PQRS)[0]
    // );

    // debug::print(
    //   "transform algo: (loop order=%s)\n"
    //   "\nfor %s=%s:"
    //   "\n  t0%s = %s"
    //   "\n  t1 = C2[b,%c]*t0%s"
    //   "\n  t2 = C1[a,%c]*t1",
    //   loop_index_shuffle(PQRS),
    //   loop_index_shuffle(PQRS), IJKL,
    //   loop_index_shuffle(PQRS), integral_index_shuffle(PQRS),
    //   PQRS[std::get<1>(C2).second], loop_index_shuffle(PQRS),
    //   PQRS[std::get<1>(C1).second]
    // );

    typedef Tensor4d::Index Index;

    for (size_t l = 0; l < basis.size(); ++l) {
      for (size_t k = 0; k < basis.size(); ++k) {
        index_transform_engine.compute_integral_block(basis, int2e, integral_index_shuffle, k, l);
        {
          const auto *t2 = index_transform_engine.transform_integral_block(transform.T12);
          // // debug::print("T2=%s, t2=%s", str(T2), str(t2));
          IndexShuffle<4> t2_order_shuffle(transform.order);
          std::array<int64_t,4> offset = { 0, 0, Index(basis[k].index), Index(basis[l].index) };
          std::array<int64_t,4> extent = t2->dimensions();
          offset = t2_order_shuffle(offset);
          extent = t2_order_shuffle(extent);
          std::vector< Range<int64_t> > idx;
          for (size_t i = 0; i < 4; ++i) {
            idx.emplace_back(offset[i], offset[i]+extent[i]);
          }
          //transform.T->set(idx, Tensor4d(t2->shuffle(t2_order_shuffle)));
        }
      }
    }


  }

}
}
}
