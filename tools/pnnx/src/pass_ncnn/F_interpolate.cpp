// Copyright 2021 Tencent
// SPDX-License-Identifier: BSD-3-Clause

#include "pass_ncnn.h"

namespace pnnx {

namespace ncnn {

class F_interpolate : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
3 2
pnnx.Input              input       0 1 input
F.interpolate           op_0        1 1 input out mode=%mode recompute_scale_factor=* scale_factor=%scale_factor
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "Interp";
    }

    const char* name_str() const
    {
        return "interpolate";
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params) const
    {
        const std::string& mode = captured_params.at("mode").s;
        std::vector<float> scale_factor;
        if (captured_params.at("scale_factor").type == 3)
        {
            scale_factor.push_back(captured_params.at("scale_factor").f);
        }
        else
        {
            scale_factor = captured_params.at("scale_factor").af;
        }

        if (mode == "nearest")
            op->params["0"] = 1;
        if (mode == "bilinear" || mode == "linear")
            op->params["0"] = 2;
        if (mode == "bicubic")
            op->params["0"] = 3;

        if (scale_factor.size() == 1)
        {
            op->params["1"] = 1.f;
            op->params["2"] = scale_factor[0];
        }
        else if (scale_factor.size() == 2)
        {
            op->params["1"] = scale_factor[0];
            op->params["2"] = scale_factor[1];
        }
        else
        {
            fprintf(stderr, "unsupported interpolate scale_factor\n");
        }

        op->params["6"] = 0; // align_corners
    }
};

REGISTER_GLOBAL_PNNX_NCNN_GRAPH_REWRITER_PASS(F_interpolate, 20)

class F_interpolate_1 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
3 2
pnnx.Input              input       0 1 input
F.interpolate           op_0        1 1 input out align_corners=%align_corners mode=%mode recompute_scale_factor=* scale_factor=%scale_factor
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "Interp";
    }

    const char* name_str() const
    {
        return "interpolate";
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params) const
    {
        const std::string& mode = captured_params.at("mode").s;
        std::vector<float> scale_factor;
        if (captured_params.at("scale_factor").type == 3)
        {
            scale_factor.push_back(captured_params.at("scale_factor").f);
        }
        else
        {
            scale_factor = captured_params.at("scale_factor").af;
        }

        if (mode == "nearest")
            op->params["0"] = 1;
        if (mode == "bilinear" || mode == "linear")
            op->params["0"] = 2;
        if (mode == "bicubic")
            op->params["0"] = 3;

        if (scale_factor.size() == 1)
        {
            op->params["1"] = 1.f;
            op->params["2"] = scale_factor[0];
        }
        else if (scale_factor.size() == 2)
        {
            op->params["1"] = scale_factor[0];
            op->params["2"] = scale_factor[1];
        }
        else
        {
            fprintf(stderr, "unsupported interpolate scale_factor\n");
        }

        op->params["6"] = captured_params.at("align_corners").b ? 1 : 0;
    }
};

REGISTER_GLOBAL_PNNX_NCNN_GRAPH_REWRITER_PASS(F_interpolate_1, 20)

class F_interpolate_2 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
3 2
pnnx.Input              input       0 1 input
F.interpolate           op_0        1 1 input out align_corners=%align_corners mode=%mode size=%size
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "Interp";
    }

    const char* name_str() const
    {
        return "interpolate";
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params) const
    {
        const std::string& mode = captured_params.at("mode").s;
        std::vector<int> output_size;
        if (captured_params.at("size").type == 2)
        {
            output_size.push_back(captured_params.at("size").i);
        }
        else
        {
            output_size = captured_params.at("size").ai;
        }

        if (mode == "nearest")
            op->params["0"] = 1;
        if (mode == "bilinear" || mode == "linear")
            op->params["0"] = 2;
        if (mode == "bicubic")
            op->params["0"] = 3;

        if (output_size.size() == 1)
        {
            op->params["3"] = 1.f;
            op->params["4"] = output_size[0];
        }
        else if (output_size.size() == 2)
        {
            op->params["3"] = output_size[0];
            op->params["4"] = output_size[1];
        }
        else
        {
            fprintf(stderr, "unsupported interpolate output_size\n");
        }

        op->params["6"] = captured_params.at("align_corners").b ? 1 : 0;
    }
};

REGISTER_GLOBAL_PNNX_NCNN_GRAPH_REWRITER_PASS(F_interpolate_2, 20)

class F_interpolate_3 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
3 2
pnnx.Input              input       0 1 input
F.interpolate           op_0        1 1 input out mode=%mode size=%size
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "Interp";
    }

    const char* name_str() const
    {
        return "interpolate";
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params) const
    {
        const std::string& mode = captured_params.at("mode").s;
        std::vector<int> output_size;
        if (captured_params.at("size").type == 2)
        {
            output_size.push_back(captured_params.at("size").i);
        }
        else
        {
            output_size = captured_params.at("size").ai;
        }

        if (mode == "nearest")
            op->params["0"] = 1;
        if (mode == "bilinear" || mode == "linear")
            op->params["0"] = 2;
        if (mode == "bicubic")
            op->params["0"] = 3;

        if (output_size.size() == 1)
        {
            op->params["3"] = 1.f;
            op->params["4"] = output_size[0];
        }
        else if (output_size.size() == 2)
        {
            op->params["3"] = output_size[0];
            op->params["4"] = output_size[1];
        }
        else
        {
            fprintf(stderr, "unsupported interpolate output_size\n");
        }

        op->params["6"] = 0; // align_corners
    }
};

REGISTER_GLOBAL_PNNX_NCNN_GRAPH_REWRITER_PASS(F_interpolate_3, 20)

} // namespace ncnn

} // namespace pnnx
