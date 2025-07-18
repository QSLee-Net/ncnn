// Copyright 2024 Tencent
// SPDX-License-Identifier: BSD-3-Clause

#include "absval_riscv.h"

#if __riscv_vector
#include <riscv_vector.h>
#endif // __riscv_vector

namespace ncnn {

#if __riscv_zvfh
static inline vfloat16m8_t __riscv_vfabs_v_f16m8_absval(vfloat16m8_t op1, size_t vl)
{
    return __riscv_vfsgnjx_vv_f16m8(op1, op1, vl);
}
#endif // __riscv_zvfh

#if NCNN_ZFH
int AbsVal_riscv::forward_inplace_fp16s(Mat& bottom_top_blob, const Option& opt) const
{
    const int w = bottom_top_blob.w;
    const int h = bottom_top_blob.h;
    const int d = bottom_top_blob.d;
    const int channels = bottom_top_blob.c;
    const int elempack = bottom_top_blob.elempack;
    const int size = w * h * d * elempack;

    #pragma omp parallel for num_threads(opt.num_threads)
    for (int q = 0; q < channels; q++)
    {
        __fp16* ptr = bottom_top_blob.channel(q);

#if __riscv_zvfh
        int n = size;
        while (n > 0)
        {
            size_t vl = __riscv_vsetvl_e16m8(n);

            vfloat16m8_t _p = __riscv_vle16_v_f16m8(ptr, vl);
            _p = __riscv_vfabs_v_f16m8_absval(_p, vl);
            __riscv_vse16_v_f16m8(ptr, _p, vl);

            ptr += vl;
            n -= vl;
        }
#else  // __riscv_zvfh
        for (int i = 0; i < size; i++)
        {
            *ptr = (*ptr > (__fp16)0.f) ? (*ptr) : (-*ptr);
            ptr++;
        }
#endif // __riscv_zvfh
    }

    return 0;
}
#endif // NCNN_ZFH

} // namespace ncnn
