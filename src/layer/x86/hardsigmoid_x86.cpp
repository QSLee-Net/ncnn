// Copyright 2017 Tencent
// SPDX-License-Identifier: BSD-3-Clause

#include "hardsigmoid_x86.h"

#if __SSE2__
#include <emmintrin.h>
#if __AVX__
#include <immintrin.h>
#endif // __AVX__
#endif // __SSE2__

#include "x86_usability.h"

namespace ncnn {

HardSigmoid_x86::HardSigmoid_x86()
{
#if __SSE2__
    support_packing = true;
#endif // __SSE2__
}

int HardSigmoid_x86::forward_inplace(Mat& bottom_top_blob, const Option& opt) const
{
    int w = bottom_top_blob.w;
    int h = bottom_top_blob.h;
    int d = bottom_top_blob.d;
    int channels = bottom_top_blob.c;
    int elempack = bottom_top_blob.elempack;
    int size = w * h * d * elempack;

    #pragma omp parallel for num_threads(opt.num_threads)
    for (int q = 0; q < channels; q++)
    {
        float* ptr = bottom_top_blob.channel(q);

        int i = 0;
#if __SSE2__
#if __AVX__
#if __AVX512F__
        __m512 _zero_avx512 = _mm512_setzero_ps();
        __m512 _one_avx512 = _mm512_set1_ps(1.f);
        for (; i + 15 < size; i += 16)
        {
            __m512 _p = _mm512_loadu_ps(ptr);
            __m512 _ans = _mm512_set1_ps(beta);
            _ans = _mm512_fmadd_ps(_p, _mm512_set1_ps(alpha), _ans);
            _ans = _mm512_max_ps(_ans, _zero_avx512);
            _ans = _mm512_min_ps(_ans, _one_avx512);
            _mm512_storeu_ps(ptr, _ans);
            ptr += 16;
        }
#endif // __AVX512F__
        __m256 _zero_avx = _mm256_setzero_ps();
        __m256 _one_avx = _mm256_set1_ps(1.f);
        for (; i + 7 < size; i += 8)
        {
            __m256 _p = _mm256_loadu_ps(ptr);
            __m256 _ans = _mm256_set1_ps(beta);
            _ans = _mm256_comp_fmadd_ps(_p, _mm256_set1_ps(alpha), _ans);
            _ans = _mm256_max_ps(_ans, _zero_avx);
            _ans = _mm256_min_ps(_ans, _one_avx);
            _mm256_storeu_ps(ptr, _ans);
            ptr += 8;
        }
#endif // __AVX__
        __m128 _zero = _mm_setzero_ps();
        __m128 _one = _mm_set1_ps(1.f);
        for (; i + 3 < size; i += 4)
        {
            __m128 _p = _mm_load_ps(ptr);
            __m128 _ans = _mm_set1_ps(beta);
            _ans = _mm_add_ps(_mm_mul_ps(_p, _mm_set1_ps(alpha)), _ans);
            _ans = _mm_max_ps(_ans, _zero);
            _ans = _mm_min_ps(_ans, _one);
            _mm_store_ps(ptr, _ans);
            ptr += 4;
        }
#endif // __SSE2__
        for (; i < size; i++)
        {
            if (*ptr < lower)
                *ptr = 0.f;
            else if (*ptr > upper)
                *ptr = 1.f;
            else
                *ptr = *ptr * alpha + beta;
            ptr++;
        }
    }

    return 0;
}

} // namespace ncnn
