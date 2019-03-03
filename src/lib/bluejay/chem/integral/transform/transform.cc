#include "bluejay/chem/integral/transform.h"
#include "bluejay/chem/integral/index.h"
#include "bluejay/core/eigen/einsum.h"
#include "bluejay/core/string.h"
#include "bluejay/core/debug.h"

namespace bluejay {
namespace integral {
namespace transform {

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

  template<size_t N, class C = const Matrix<double>& >
  struct IndexTransform {

    const std::tuple<C,IndexPair> C1, C2;
    const IndexShuffle<N> loop_order_shuffle;
    Tensor4d t1, t2;

    IndexTransform(std::tuple<C,IndexPair> C1, std::tuple<C,IndexPair> C2)
      : C1(C1), C2(C2), loop_order_shuffle(loop_order(C1, C2))
    {
    }

    static IndexShuffle<4> loop_order(std::tuple<C,IndexPair> C1, std::tuple<C,IndexPair> C2) {
      IndexList<size_t> t = { std::get<1>(C1).second, std::get<1>(C2).second };
      Assert(
        !t.repeated(),
        "Repeated transform indices %s", str(t.repeated())
      );
      std::vector<size_t> v;
      for (size_t idx = 0; idx < N; ++idx) {
        if (t.count(idx)) continue;
        v.push_back(idx);
      }
      v.insert(v.end(), t.begin(), t.end());
      return IndexShuffle<N>(v.begin(), v.end());
    }

    const Tensor4d& transform_bra(const Tensor4d &t0) {
      auto C1 = this->C1;
      auto C2 = this->C2;
      std::get<1>(C1).second = N-1;
      std::get<1>(C2).second = N-1;
      transform1(C2, t0, t1); // r,s,p,q -> q,r,s,p
      transform1(C1, t1, t2); // q,r,s,p -> p,q,r,s
      return t2;
    }

    static void transform1(
      std::tuple<C,IndexPair> Cx,
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

  };


  // evaluate t0 = shuffle(integral(index_shuffle(p,q,r,s)))
  void compute_integral_block(
    const Molecule::Basis &basis,
    IntegralEngine::TwoBodyEngine &int2e,
    IndexShuffle<4> integral_order_shuffle,
    size_t r,
    size_t s,
    Tensor4d &t0,
    IndexShuffle<4> t0_order_shuffle)
  {

    typedef Tensor4d::Index Index;
    auto nbf = [](const auto &s) { return (Index)s.nbf(); };
    //t0.resize((int64_t)basis.nbf(), (int64_t)basis.nbf(), nbf(r), nbf(s));
    t0.resize(nbf(basis[r]), nbf(basis[s]), nbf(basis), nbf(basis));

    const auto &index = basis.index();
    Tensor4d tmp;

    // // output order is [s,r,q,p], hence reverse
    // t0_order_shuffle = (~t0_order_shuffle)({0,1,2,3});

    // debug::print(
    //   "compute_integral_block: integral_order=%s, t0_order=%s, braket=(%s)",
    //   integral_order_shuffle,
    //   t0_order_shuffle,
    //   integral_order_shuffle
    // );

    // integral is in [s,r,q,p] order, hence reverse of shuffle
    IndexShuffle<4> integral_array_shuffle = t0_order_shuffle.reverse();

    for (size_t q = 0; q < basis.size(); ++q) {
      for (size_t p = 0; p < basis.size(); ++p) {

        auto integral_indices = t0_order_shuffle(integral_order_shuffle({p,q,r,s}));
        int2e.compute(
          basis[integral_indices[0]],
          basis[integral_indices[1]],
          basis[integral_indices[2]],
          basis[integral_indices[3]],
          tmp
        );

        std::array<int64_t,4> offset = { (Index)index[p], (Index)index[q], 0, 0 };
        std::array<int64_t,4> extent = { nbf(basis[p]), nbf(basis[q]), nbf(basis[r]), nbf(basis[s]) };
        offset = t0_order_shuffle(offset);
        extent = t0_order_shuffle(extent);

        // // integral is in [s,r,q,p] order
        // debug::print(
        //   "%s.slice(offset=%s, extent=%s) = %s(%s)",
        //   t0,
        //   offset,
        //   extent,
        //   t0_order_shuffle({3,2,0,1}),
        //   tmp
        // );

        // t0.slice(offset, extent) = tmp.shuffle(integral_array_shuffle);

      }
    }

  }

  // T2(a,b,r,s) = C1(a,p)*(C2(b,q)*(p,q|r,s))
  void transform(
    const Molecule::Basis &basis,
    IntegralEngine::TwoBodyEngine &int2e,
    std::array<size_t,4> integral_order,
    std::tuple<const Matrix<double>&, IndexPair> C1,
    std::tuple<const Matrix<double>&, IndexPair> C2,
    Tensor4d &T2,
    std::array<size_t,4> t2_order)
  {
    IndexList<size_t> integral_index(integral_order);

    // shuffle loop order s.t. transform indices are last
    IndexTransform<4> index_transform_engine(C1, C2);
    IndexShuffle<4> loop_order_shuffle = index_transform_engine.loop_order_shuffle;

    IndexShuffle<4> integral_order_shuffle = integral_order;

    IndexShuffle<4> t2_order_shuffle = {
      integral_order_shuffle[t2_order[0]],
      integral_order_shuffle[t2_order[1]],
      integral_order_shuffle[t2_order[2]],
      integral_order_shuffle[t2_order[3]]
    };
    //t2_order_shuffle = ~t2_order_shuffle;

    debug::print(
      "transform: integral_order=%s, loop_order=%s, t2_shuffle=%s",
      str(integral_order_shuffle),
      str(loop_order_shuffle),
      str(t2_order_shuffle)
    );

    Tensor4d t0;

    const auto &basis_index = basis.index();

    for (size_t s = 0; s < basis.size(); ++s) {
      for (size_t r = 0; r < basis.size(); ++r) {
        compute_integral_block(basis, int2e, integral_order_shuffle, r, s, t0, loop_order_shuffle);
        const auto &t2 = index_transform_engine.transform_bra(t0);
        // // debug::print("T2=%s, t2=%s", str(T2), str(t2));
        // std::array<int64_t,4> offset = { 0, 0, (int64_t)basis_index[r], (int64_t)basis_index[s] };
        // std::array<int64_t,4> extent = t2.dimensions();
        // offset = t2_order_shuffle(offset);
        // extent = t2_order_shuffle(extent);
        // T2.slice(t2_order_shuffle(offset), t2_order_shuffle(extent)) = t2.shuffle(t2_order_shuffle);
      }
    }

    debug::print("T2=%s", T2);

  }

  IntegralTransform::IntegralTransform(
    const Molecule::Basis &basis,
    std::shared_ptr<IntegralEngine> integral_engine
  )
    : basis_(basis)
  {
    // if (!integral_engine) {
    //   integral_engine = IntegralEngine::default_instance();
    // }
    assert(integral_engine);
    integral_engine_ = std::move(integral_engine);
  }

  // void IntegralTransform::transform(
  //   std::string expr,
  //   Tensor4d &t2,
  //   const Matrix<double> &c1,
  //   const Matrix<double> &c2) const
  // {
  //   auto expression = TransformExpression(expr);
  //   expression.Validate(t2, c1, c2);
  //   auto int2e = integral_engine_->coulomb();
  //   const auto &basis = this->basis();
  //   const auto &braket = Shuffle<4>(expression.braket<4>());

  //   auto t1_indices = contraction_indices(
  //     expression.args().at(1),
  //     expression.bra() + expression.ket()
  //   );

  //   Shuffle<4> order({0,1,2,3});
  //   Tensor4d t0, t1;

  //   for (size_t s = 0; s < basis.size(); ++s) {
  //     for (size_t r = 0; r < basis.size(); ++r) {
  //       transform::compute_integral_block(basis, *int2e, braket, r, s, t0, order);
  //       //expression.transform(0);
  //       einsum::einsum(c2, 0, t0, 3, t1);
  //       einsum::einsum(c1, 0, t1, 3, t2);
  //     }
  //   }
  // }

  // template<size_t N, typename Index = size_t>
  // struct index_permutation {
  //   index_permutation(std::array<size_t,N> p, std::array<Index,N> idx)
  //     : permutation_(p), indicies_(idx)
  //   {
  //   }
  //   Index operator[](size_t i) const {
  //     return indicies_[permutation_[i]];
  //   }
  // private:
  //   std::array<size_t,N> permutation_;
  //   std::array<Index,N> indicies_;
  // };

  // void transform_2_index(
  //   std::array<size_t,4> type,
  //   IntegralEngine::TwoBodyEngine &int2e,
  //   const Molecule::Basis& basis,
  //   std::array<size_t,2> ket,
  //   Tensor4d &V)
  // {
  //   //static_assert((Type == 0213 || Type == 0123), "");
  //   for (size_t q = 0; q < basis.size(); ++q) {
  //     for (size_t p = 0; p < basis.size(); ++p) {
  //       index_permutation<4> quartet(type, { p, q, ket[0], ket[1] });
  //       auto nbf = [&] (size_t i) {
  //         return basis::nbf(basis[quartet[i]].shell());
  //       };
  //       Tensor4d v(nbf(0), nbf(1), nbf(2), nbf(3));
  //       auto shell = [&] (size_t i) {
  //         return std::tie(basis[quartet[i]].shell(), basis[quartet[i]].center());
  //       };
  //       //int2e.compute(shell(0), shell(1), shell(2), shell(3), v);
  //     }
  //   }
  // }


  void IntegralTransform::compute_pq(size_t k, size_t l) {
    // const auto &basis = this->basis_;
    // // const auto &r = basis[k];
    // // const auto &s = basis[l];
    // for (const auto &p : basis) {
    //   for (const auto &q : basis) {
    //     //Tensor4d eri;
    //     //integral_engine_->compute(p, q, r, s, eri);
    //     //eri_(r,s,p,q) = eri;
    //   }
    // }
  }

  Tensor4d IntegralTransform::transform_pq(const Matrix<double> &Ci, const Matrix<double> &Cj) const {
    // const auto &V = this->eri_;
    // Tensor4d t1, t2;
    // einsum::einsum(Cj, 1, V, 3, t1); // u(j,r,s,p) = C(j,q)*v(r,s,p,q)
    // einsum::einsum(Ci, 1, V, 3, t2); // t(i,j,r,s) = C(i,p)*u(j,r,s,p)
    //return t2;
    return Tensor4d{};
  }

  Tensor4d IntegralTransform::transform_pq(const Tensor4d &Tij) const {
    //const auto &V = this->eri_;
    //auto t2 = einsum("ijpq,rspq", Tij, V); // t(i,j,r,s) = T(i,j,p,q)*v(r,s,p,q)
    return Tensor4d{};
  }

}
}
}
