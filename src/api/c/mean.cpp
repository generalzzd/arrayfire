/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#include <af/dim4.hpp>
#include <af/defines.h>
#include <af/statistics.h>
#include <handle.hpp>
#include <err_common.hpp>
#include <backend.hpp>
#include <reduce.hpp>
#include <arith.hpp>
#include <print.hpp>

template<typename inType, typename outType>
static outType mean(const af_array &in)
{
    const Array<inType> &input = getArray<inType>(in);
    return reduce_all<af_add_t, inType, outType>(input)/(outType)input.elements();
}

template<typename inType, typename outType>
static af_array mean(const af_array &in, dim_type dim)
{
    const Array<inType> &input = getArray<inType>(in);
    dim4 iDims = input.dims();

    Array<outType>* output = reduce<af_add_t, inType, outType>(input, dim);

    dim4 oDims = output->dims();

    Array<outType> *cnstArray = createValueArray<outType>(oDims, iDims[dim]);
    Array<outType> *retVal    = detail::arithOp<outType, af_div_t>(*output, *cnstArray);

    destroyArray<outType>(*cnstArray);
    destroyArray<outType>(*output);

    return getHandle<outType>(*retVal);
}

af_err af_mean(af_array *out, const af_array in, dim_type dim)
{
    try {
        af_array output = 0;
        ArrayInfo info = getInfo(in);
        af_dtype type = info.getType();
        switch(type) {
            case f64: output = mean<double, double >(in, dim); break;
            case f32: output = mean<float , float  >(in, dim); break;
            case s32: output = mean<int   , int    >(in, dim); break;
            case u32: output = mean<uint  , uint   >(in, dim); break;
            case  u8: output = mean<uchar , uint   >(in, dim); break;
            case  b8: output = mean<char  , int    >(in, dim); break;
            case c32: output = mean<cfloat, cfloat >(in, dim); break;
            case c64: output = mean<cdouble,cdouble>(in, dim); break;
            default : TYPE_ERROR(1, type);
        }
        std::swap(*out, output);
    }
    CATCHALL;
    return AF_SUCCESS;
}

af_err af_mean_all(double *real, double *imag, const af_array in)
{
    try {
        ArrayInfo info = getInfo(in);
        af_dtype type = info.getType();
        switch(type) {
            case f64: *real = mean<double, double>(in); break;
            case f32: *real = mean<float ,  float>(in); break;
            case s32: *real = mean<int   ,    int>(in); break;
            case u32: *real = mean<uint  ,   uint>(in); break;
            case  u8: *real = mean<uchar ,   uint>(in); break;
            case  b8: *real = mean<char  ,    int>(in); break;
            case c32: {
                std::complex<float> tmp = mean<cfloat,cfloat>(in);
                *real = tmp.real();
                *imag = tmp.imag();
                } break;
            case c64: {
                std::complex<double> tmp = mean<cdouble,cdouble>(in);
                *real = tmp.real();
                *imag = tmp.imag();
                } break;
            default : TYPE_ERROR(1, type);
        }
    }
    CATCHALL;
    return AF_SUCCESS;
}
