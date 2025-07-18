// Copyright 2022 Tencent
// SPDX-License-Identifier: BSD-3-Clause

#include "fuse_pad_conv1d.h"

#include "pass_level2.h"

#include <math.h>
#include <string.h>

namespace pnnx {

class fuse_pad_conv1d_pass : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input       0 1 input
F.pad                   op_pad      1 1 input a mode=constant pad=%pad value=%value
nn.Conv1d               op_0        1 1 a out in_channels=%in_channels out_channels=%out_channels kernel_size=%kernel_size stride=%stride padding_mode=zeros padding=%padding dilation=%dilation groups=%groups bias=%bias @weight @bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "nn.Conv1d";
    }

    const char* name_str() const
    {
        return "padconv1d";
    }

    bool match(const std::map<std::string, Parameter>& captured_params) const
    {
        // constant-0 + zeros
        float pad_value = 0.f;
        if (captured_params.at("value").type == 2)
            pad_value = captured_params.at("value").i;
        if (captured_params.at("value").type == 3)
            pad_value = captured_params.at("value").f;

        if (pad_value != 0.f)
            return false;

        const std::vector<int>& pad = captured_params.at("pad").ai;
        for (int x : pad)
        {
            if (x < 0)
                return false;
        }

        if (pad.size() != 2)
            return false;

        if (pad.size() == 2 && pad[0] != pad[1])
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        const std::vector<int>& pad = captured_params.at("pad").ai;
        std::vector<int> padding = captured_params.at("padding").ai;
        padding[0] += pad[0];

        op->params["in_channels"] = captured_params.at("in_channels");
        op->params["out_channels"] = captured_params.at("out_channels");
        op->params["kernel_size"] = captured_params.at("kernel_size");
        op->params["padding_mode"] = "zeros";
        op->params["stride"] = captured_params.at("stride");
        op->params["padding"] = padding;
        op->params["dilation"] = captured_params.at("dilation");
        op->params["groups"] = captured_params.at("groups");
        op->params["bias"] = captured_params.at("bias");

        op->attrs["weight"] = captured_attrs.at("op_0.weight");

        if (captured_params.at("bias").b)
        {
            op->attrs["bias"] = captured_attrs.at("op_0.bias");
        }
    }
};

class fuse_pad_conv1d_pass_1 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input       0 1 input
F.pad                   op_pad      1 1 input a mode=%mode pad=%pad value=None
nn.Conv1d               op_0        1 1 a out in_channels=%in_channels out_channels=%out_channels kernel_size=%kernel_size stride=%stride padding_mode=* padding=(0) dilation=%dilation groups=%groups bias=%bias @weight @bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "nn.Conv1d";
    }

    const char* name_str() const
    {
        return "padconv1d";
    }

    bool match(const std::map<std::string, Parameter>& captured_params) const
    {
        // reflect/replicate + nopad
        if (captured_params.at("mode").s != "reflect" && captured_params.at("mode").s != "replicate")
            return false;

        const std::vector<int>& pad = captured_params.at("pad").ai;
        for (int x : pad)
        {
            if (x < 0)
                return false;
        }

        if (pad.size() != 2)
            return false;

        if (pad.size() == 2 && pad[0] != pad[1])
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        const std::vector<int>& pad = captured_params.at("pad").ai;
        std::vector<int> padding(1);
        padding[0] = pad[0];

        op->params["in_channels"] = captured_params.at("in_channels");
        op->params["out_channels"] = captured_params.at("out_channels");
        op->params["kernel_size"] = captured_params.at("kernel_size");
        op->params["padding_mode"] = captured_params.at("mode");
        op->params["stride"] = captured_params.at("stride");
        op->params["padding"] = padding;
        op->params["dilation"] = captured_params.at("dilation");
        op->params["groups"] = captured_params.at("groups");
        op->params["bias"] = captured_params.at("bias");

        op->attrs["weight"] = captured_attrs.at("op_0.weight");

        if (captured_params.at("bias").b)
        {
            op->attrs["bias"] = captured_attrs.at("op_0.bias");
        }
    }
};

class fuse_pad_conv1d_pass_2 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input       0 1 input
nn.ConstantPad1d        op_pad      1 1 input a padding=%pad value=%value
nn.Conv1d               op_0        1 1 a out in_channels=%in_channels out_channels=%out_channels kernel_size=%kernel_size stride=%stride padding_mode=zeros padding=%padding dilation=%dilation groups=%groups bias=%bias @weight @bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "nn.Conv1d";
    }

    const char* name_str() const
    {
        return "padconv1d";
    }

    bool match(const std::map<std::string, Parameter>& captured_params) const
    {
        // constant-0 + zeros
        float pad_value = 0.f;
        if (captured_params.at("value").type == 2)
            pad_value = captured_params.at("value").i;
        if (captured_params.at("value").type == 3)
            pad_value = captured_params.at("value").f;

        if (pad_value != 0.f)
            return false;

        const std::vector<int>& pad = captured_params.at("pad").ai;
        for (int x : pad)
        {
            if (x < 0)
                return false;
        }

        if (pad.size() != 2)
            return false;

        if (pad[0] != pad[1])
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        std::vector<int> padding = captured_params.at("padding").ai;
        const std::vector<int>& pad = captured_params.at("pad").ai;
        padding[0] += pad[0];

        op->params["in_channels"] = captured_params.at("in_channels");
        op->params["out_channels"] = captured_params.at("out_channels");
        op->params["kernel_size"] = captured_params.at("kernel_size");
        op->params["padding_mode"] = "zeros";
        op->params["stride"] = captured_params.at("stride");
        op->params["padding"] = padding;
        op->params["dilation"] = captured_params.at("dilation");
        op->params["groups"] = captured_params.at("groups");
        op->params["bias"] = captured_params.at("bias");

        op->attrs["weight"] = captured_attrs.at("op_0.weight");

        if (captured_params.at("bias").b)
        {
            op->attrs["bias"] = captured_attrs.at("op_0.bias");
        }
    }
};

class fuse_pad_conv1d_pass_3 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input       0 1 input
nn.ReplicationPad1d     op_pad      1 1 input a padding=%pad
nn.Conv1d               op_0        1 1 a out in_channels=%in_channels out_channels=%out_channels kernel_size=%kernel_size stride=%stride padding_mode=* padding=(0) dilation=%dilation groups=%groups bias=%bias @weight @bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "nn.Conv1d";
    }

    const char* name_str() const
    {
        return "padconv1d";
    }

    bool match(const std::map<std::string, Parameter>& captured_params) const
    {
        // replicate + nopad
        const std::vector<int>& pad = captured_params.at("pad").ai;
        for (int x : pad)
        {
            if (x < 0)
                return false;
        }

        if (pad.size() != 2)
            return false;

        if (pad[0] != pad[1])
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        std::vector<int> padding(1);
        const std::vector<int>& pad = captured_params.at("pad").ai;
        padding[0] = pad[0];

        op->params["in_channels"] = captured_params.at("in_channels");
        op->params["out_channels"] = captured_params.at("out_channels");
        op->params["kernel_size"] = captured_params.at("kernel_size");
        op->params["padding_mode"] = "replicate";
        op->params["stride"] = captured_params.at("stride");
        op->params["padding"] = padding;
        op->params["dilation"] = captured_params.at("dilation");
        op->params["groups"] = captured_params.at("groups");
        op->params["bias"] = captured_params.at("bias");

        op->attrs["weight"] = captured_attrs.at("op_0.weight");

        if (captured_params.at("bias").b)
        {
            op->attrs["bias"] = captured_attrs.at("op_0.bias");
        }
    }
};

class fuse_pad_conv1d_pass_4 : public GraphRewriterPass
{
public:
    const char* match_pattern_graph() const
    {
        return R"PNNXIR(7767517
4 3
pnnx.Input              input       0 1 input
nn.ReflectionPad1d      op_pad      1 1 input a padding=%pad
nn.Conv1d               op_0        1 1 a out in_channels=%in_channels out_channels=%out_channels kernel_size=%kernel_size stride=%stride padding_mode=* padding=(0) dilation=%dilation groups=%groups bias=%bias @weight @bias
pnnx.Output             output      1 0 out
)PNNXIR";
    }

    const char* type_str() const
    {
        return "nn.Conv1d";
    }

    const char* name_str() const
    {
        return "padconv1d";
    }

    bool match(const std::map<std::string, Parameter>& captured_params) const
    {
        // reflect + nopad
        const std::vector<int>& pad = captured_params.at("pad").ai;
        for (int x : pad)
        {
            if (x < 0)
                return false;
        }

        if (pad.size() != 2)
            return false;

        if (pad[0] != pad[1])
            return false;

        return true;
    }

    void write(Operator* op, const std::map<std::string, Parameter>& captured_params, const std::map<std::string, Attribute>& captured_attrs) const
    {
        std::vector<int> padding(1);
        const std::vector<int>& pad = captured_params.at("pad").ai;
        padding[0] = pad[0];

        op->params["in_channels"] = captured_params.at("in_channels");
        op->params["out_channels"] = captured_params.at("out_channels");
        op->params["kernel_size"] = captured_params.at("kernel_size");
        op->params["padding_mode"] = "reflect";
        op->params["stride"] = captured_params.at("stride");
        op->params["padding"] = padding;
        op->params["dilation"] = captured_params.at("dilation");
        op->params["groups"] = captured_params.at("groups");
        op->params["bias"] = captured_params.at("bias");

        op->attrs["weight"] = captured_attrs.at("op_0.weight");

        if (captured_params.at("bias").b)
        {
            op->attrs["bias"] = captured_attrs.at("op_0.bias");
        }
    }
};

void fuse_pad_conv1d(Graph& graph)
{
    fuse_pad_conv1d_pass a;
    fuse_pad_conv1d_pass_1 b;
    fuse_pad_conv1d_pass_2 c;
    fuse_pad_conv1d_pass_3 d;
    fuse_pad_conv1d_pass_4 e;
    int opindex = 0;

    pnnx_graph_rewrite(graph, &a, opindex);
    pnnx_graph_rewrite(graph, &b, opindex);
    pnnx_graph_rewrite(graph, &c, opindex);
    pnnx_graph_rewrite(graph, &d, opindex);
    pnnx_graph_rewrite(graph, &e, opindex);
}

} // namespace pnnx
