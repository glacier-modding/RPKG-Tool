// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// WARNING: This header file was automatically generated
// by GenerateSchemaJsonHeader.ps1.
// Modifying this code by hand is not recommended.

#pragma once

#include <string>

#include <unordered_map>

namespace Microsoft {

  namespace glTF {

    class SchemaJson {

      public:

      static const char* const accessor_schema_json;

      static const char* const accessor_sparse_indices_schema_json;

      static const char* const accessor_sparse_schema_json;

      static const char* const accessor_sparse_values_schema_json;

      static const char* const animation_channel_schema_json;

      static const char* const animation_channel_target_schema_json;

      static const char* const animation_sampler_schema_json;

      static const char* const animation_schema_json;

      static const char* const asset_schema_json;

      static const char* const buffer_schema_json;

      static const char* const bufferView_schema_json;

      static const char* const camera_orthographic_schema_json;

      static const char* const camera_perspective_schema_json;

      static const char* const camera_schema_json;

      static const char* const extension_schema_json;

      static const char* const extras_schema_json;

      static const char* const glTF_schema_json;

      static const char* const glTFChildOfRootProperty_schema_json;

      static const char* const glTFid_schema_json;

      static const char* const glTFProperty_schema_json;

      static const char* const image_schema_json;

      static const char* const material_normalTextureInfo_schema_json;

      static const char* const material_occlusionTextureInfo_schema_json;

      static const char* const material_pbrMetallicRoughness_schema_json;

      static const char* const material_schema_json;

      static const char* const mesh_primitive_schema_json;

      static const char* const mesh_schema_json;

      static const char* const node_schema_json;

      static const char* const sampler_schema_json;

      static const char* const scene_schema_json;

      static const char* const skin_schema_json;

      static const char* const texture_schema_json;

      static const char* const textureInfo_schema_json;

      static const std::unordered_map<std::string, std::string> GLTF_SCHEMA_MAP;

    }; // end class SchemaJson scope

    const char* const SchemaJson::material_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Material",
            "type": "object",
            "description": "The material appearance of a primitive.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "name": { },
                "extensions": { },
                "extras": { },
                "pbrMetallicRoughness": {
                    "allOf": [ { "$ref": "material.pbrMetallicRoughness.schema.json" } ],
                    "description": "A set of parameter values that are used to define the metallic-roughness material model from Physically-Based Rendering (PBR) methodology. When not specified, all the default values of `pbrMetallicRoughness` apply."
                },
                "normalTexture": {
                    "allOf": [ { "$ref": "material.normalTextureInfo.schema.json" } ],
                    "description": "The normal map texture.",
                    "gltf_detailedDescription": "A tangent space normal map. The texture contains RGB components in linear space. Each texel represents the XYZ components of a normal vector in tangent space. Red [0 to 255] maps to X [-1 to 1]. Green [0 to 255] maps to Y [-1 to 1]. Blue [128 to 255] maps to Z [1/255 to 1]. The normal vectors use OpenGL conventions where +X is right and +Y is up. +Z points toward the viewer. In GLSL, this vector would be unpacked like so: `float3 normalVector = tex2D(<sampled normal map texture value>, texCoord) * 2 - 1`. Client implementations should normalize the normal vectors before using them in lighting equations."
                },
                "occlusionTexture": {
                    "allOf": [ { "$ref": "material.occlusionTextureInfo.schema.json" } ],
                    "description": "The occlusion map texture.",
                    "gltf_detailedDescription": "The occlusion map texture. The occlusion values are sampled from the R channel. Higher values indicate areas that should receive full indirect lighting and lower values indicate no indirect lighting. These values are linear. If other channels are present (GBA), they are ignored for occlusion calculations."
                },
                "emissiveTexture": {
                    "allOf": [ { "$ref": "textureInfo.schema.json" } ],
                    "description": "The emissive map texture.",
                    "gltf_detailedDescription": "The emissive map controls the color and intensity of the light being emitted by the material. This texture contains RGB components in sRGB color space. If a fourth component (A) is present, it is ignored."
                },
                "emissiveFactor": {
                    "type": "array",
                    "items": {
                        "type": "number",
                        "minimum": 0.0,
                        "maximum": 1.0
                    },
                    "minItems": 3,
                    "maxItems": 3,
                    "default": [ 0.0, 0.0, 0.0 ],
                    "description": "The emissive color of the material.",
                    "gltf_detailedDescription": "The RGB components of the emissive color of the material. These values are linear. If an emissiveTexture is specified, this value is multiplied with the texel values."
                },
                "alphaMode": {
                    "default": "OPAQUE",
                    "description": "The alpha rendering mode of the material.",
                    "gltf_detailedDescription": "The material's alpha rendering mode enumeration specifying the interpretation of the alpha value of the main factor and texture.",
                    "anyOf": [
                        {
                            "enum": [ "OPAQUE" ],
                            "description": "The alpha value is ignored and the rendered output is fully opaque."
                        },
                        {
                            "enum": [ "MASK" ],
                            "description": "The rendered output is either fully opaque or fully transparent depending on the alpha value and the specified alpha cutoff value."
                        },
                        {
                            "enum": [ "BLEND" ],
                            "description": "The alpha value is used to composite the source and destination areas. The rendered output is combined with the background using the normal painting operation (i.e. the Porter and Duff over operator)."
                        },
                        {
                            "type": "string"
                        }
                    ]
                },
                "alphaCutoff": {
                    "type": "number",
                    "minimum": 0.0,
                    "default": 0.5,
                    "description": "The alpha cutoff value of the material.",
                    "gltf_detailedDescription": "Specifies the cutoff threshold when in `MASK` mode. If the alpha value is greater than or equal to this value then it is rendered as fully opaque, otherwise, it is rendered as fully transparent. A value greater than 1.0 will render the entire material as fully transparent. This value is ignored for other modes."
                },
                "doubleSided": {
                    "type": "boolean",
                    "default": false,
                    "description": "Specifies whether the material is double sided.",
                    "gltf_detailedDescription": "Specifies whether the material is double sided. When this value is false, back-face culling is enabled. When this value is true, back-face culling is disabled and double sided lighting is enabled. The back-face must have its normals reversed before the lighting equation is evaluated."
                }
            },
             "dependencies" : {
                "alphaCutoff" : ["alphaMode"]
            }
        }
        )rawstring";

    const char* const SchemaJson::glTFChildOfRootProperty_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "glTF Child of Root Property",
            "type": "object",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "name": {
                    "type": "string",
                    "description": "The user-defined name of this object.",
                    "gltf_detailedDescription": "The user-defined name of this object.  This is not necessarily unique, e.g., an accessor and a buffer could have the same name, or two accessors could even have the same name."
                }
            }
        }
        )rawstring";

    const char* const SchemaJson::image_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Image",
            "type": "object",
            "description": "Image data used to create a texture. Image can be referenced by URI or `bufferView` index. `mimeType` is required in the latter case.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "uri": {
                    "type": "string",
                    "description": "The uri of the image.",
                    "format": "uriref",
                    "gltf_detailedDescription": "The uri of the image.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.  The image format must be jpg or png.",
                    "gltf_uriType": "image"
                },
                "mimeType": {
                    "anyOf": [
                        {
                            "enum": [ "image/jpeg" ]
                        },
                        {
                            "enum": [ "image/png" ]
                        },
                        {
                            "type": "string"
                        }
                    ],
                    "description": "The image's MIME type."
                },
                "bufferView": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the bufferView that contains the image. Use this instead of the image's uri property."
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "dependencies": {
                "bufferView": [ "mimeType" ]
            },
            "oneOf": [
                { "required": [ "uri" ] },
                { "required": [ "bufferView" ] }
            ]
        }
        )rawstring";

    const char* const SchemaJson::animation_sampler_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Animation Sampler",
            "type": "object",
            "description": "Combines input and output accessors with an interpolation algorithm to define a keyframe graph (but not its target).",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "input": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of an accessor containing keyframe input values, e.g., time.",
                    "gltf_detailedDescription": "The index of an accessor containing keyframe input values, e.g., time. That accessor must have componentType `FLOAT`. The values represent time in seconds with `time[0] >= 0.0`, and strictly increasing values, i.e., `time[n + 1] > time[n]`."
                },
                "interpolation": {
                    "description": "Interpolation algorithm.",
                    "default": "LINEAR",
                    "gltf_detailedDescription": "Interpolation algorithm.",
                    "anyOf": [
                        {
                            "enum": [ "LINEAR" ],
                            "description": "The animated values are linearly interpolated between keyframes. When targeting a rotation, spherical linear interpolation (slerp) should be used to interpolate quaternions. The number output of elements must equal the number of input elements."
                        },
                        {
                            "enum": [ "STEP" ],
                            "description": "The animated values remain constant to the output of the first keyframe, until the next keyframe. The number of output elements must equal the number of input elements."
                        },
                        {
                            "enum": [ "CUBICSPLINE" ],
                            "description": "The animation's interpolation is computed using a cubic spline with specified tangents. The number of output elements must equal three times the number of input elements. For each input element, the output stores three elements, an in-tangent, a spline vertex, and an out-tangent. There must be at least two keyframes when using this interpolation."
                        },
                        {
                            "type": "string"
                        }
                    ]
                },
                "output": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of an accessor, containing keyframe output values.",
                    "gltf_detailedDescription": "The index of an accessor containing keyframe output values. When targeting translation or scale paths, the `accessor.componentType` of the output values must be `FLOAT`. When targeting rotation or morph weights, the `accessor.componentType` of the output values must be `FLOAT` or normalized integer. For weights, each output element stores `SCALAR` values with a count equal to the number of morph targets."
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "input", "output" ]
        }
        )rawstring";

    const char* const SchemaJson::sampler_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Sampler",
            "type": "object",
            "description": "Texture sampler properties for filtering and wrapping modes.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "magFilter": {
                    "description": "Magnification filter.",
                    "gltf_detailedDescription": "Magnification filter.  Valid values correspond to WebGL enums: `9728` (NEAREST) and `9729` (LINEAR).",
                    "gltf_webgl": "`texParameterf()` with pname equal to TEXTURE_MAG_FILTER",
                    "anyOf": [
                        {
                            "enum": [ 9728 ],
                            "description": "NEAREST"
                        },
                        {
                            "enum": [ 9729 ],
                            "description": "LINEAR"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "minFilter": {
                    "description": "Minification filter.",
                    "gltf_detailedDescription": "Minification filter.  All valid values correspond to WebGL enums.",
                    "gltf_webgl": "`texParameterf()` with pname equal to TEXTURE_MIN_FILTER",
                    "anyOf": [
                        {
                            "enum": [ 9728 ],
                            "description": "NEAREST"
                        },
                        {
                            "enum": [ 9729 ],
                            "description": "LINEAR"
                        },
                        {
                            "enum": [ 9984 ],
                            "description": "NEAREST_MIPMAP_NEAREST"
                        },
                        {
                            "enum": [ 9985 ],
                            "description": "LINEAR_MIPMAP_NEAREST"
                        },
                        {
                            "enum": [ 9986 ],
                            "description": "NEAREST_MIPMAP_LINEAR"
                        },
                        {
                            "enum": [ 9987 ],
                            "description": "LINEAR_MIPMAP_LINEAR"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "wrapS": {
                    "description": "s wrapping mode.",
                    "default": 10497,
                    "gltf_detailedDescription": "S (U) wrapping mode.  All valid values correspond to WebGL enums.",
                    "gltf_webgl": "`texParameterf()` with pname equal to TEXTURE_WRAP_S",
                    "anyOf": [
                        {
                            "enum": [ 33071 ],
                            "description": "CLAMP_TO_EDGE"
                        },
                        {
                            "enum": [ 33648 ],
                            "description": "MIRRORED_REPEAT"
                        },
                        {
                            "enum": [ 10497 ],
                            "description": "REPEAT"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "wrapT": {
                    "description": "t wrapping mode.",
                    "default": 10497,
                    "gltf_detailedDescription": "T (V) wrapping mode.  All valid values correspond to WebGL enums.",
                    "gltf_webgl": "`texParameterf()` with pname equal to TEXTURE_WRAP_T",
                    "anyOf": [
                        {
                            "enum": [ 33071 ],
                            "description": "CLAMP_TO_EDGE"
                        },
                        {
                            "enum": [ 33648 ],
                            "description": "MIRRORED_REPEAT"
                        },
                        {
                            "enum": [ 10497 ],
                            "description": "REPEAT"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "gltf_webgl": "`texParameterf()`"
        }
        )rawstring";

    const char* const SchemaJson::extension_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Extension",
            "type": "object",
            "description": "Dictionary object with extension-specific objects.",
            "properties": {
            },
            "additionalProperties": {
                "type": "object"
            }
        }
        )rawstring";

    const char* const SchemaJson::buffer_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Buffer",
            "type": "object",
            "description": "A buffer points to binary geometry, animation, or skins.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "uri": {
                    "type": "string",
                    "description": "The uri of the buffer.",
                    "format": "uriref",
                    "gltf_detailedDescription": "The uri of the buffer.  Relative paths are relative to the .gltf file.  Instead of referencing an external file, the uri can also be a data-uri.",
                    "gltf_uriType": "application"
                },
                "byteLength": {
                    "type": "integer",
                    "description": "The length of the buffer in bytes.",
                    "minimum": 1
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "byteLength" ]
        }
        )rawstring";

    const char* const SchemaJson::node_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Node",
            "type": "object",
            "description": "A node in the node hierarchy.  When the node contains `skin`, all `mesh.primitives` must contain `JOINTS_0` and `WEIGHTS_0` attributes.  A node can have either a `matrix` or any combination of `translation`/`rotation`/`scale` (TRS) properties. TRS properties are converted to matrices and postmultiplied in the `T * R * S` order to compose the transformation matrix; first the scale is applied to the vertices, then the rotation, and then the translation. If none are provided, the transform is the identity. When a node is targeted for animation (referenced by an animation.channel.target), only TRS properties may be present; `matrix` will not be present.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "camera": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the camera referenced by this node."
                },
                "children": {
                    "type": "array",
                    "description": "The indices of this node's children.",
                    "items": {
                        "$ref": "glTFid.schema.json"
                    },
                    "uniqueItems": true,
                    "minItems": 1
                },
                "skin": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the skin referenced by this node."
                },
                "matrix": {
                    "type": "array",
                    "description": "A floating-point 4x4 transformation matrix stored in column-major order.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 16,
                    "maxItems": 16,
                    "default": [ 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0 ],
                    "gltf_detailedDescription": "A floating-point 4x4 transformation matrix stored in column-major order.",
                    "gltf_webgl": "`uniformMatrix4fv()` with the transpose parameter equal to false"
                },
                "mesh": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the mesh in this node."
                },
                "rotation": {
                    "type": "array",
                    "description": "The node's unit quaternion rotation in the order (x, y, z, w), where w is the scalar.",
                    "items": {
                        "type": "number",
                        "minimum": -1.0,
                        "maximum": 1.0
                    },
                    "minItems": 4,
                    "maxItems": 4,
                    "default": [ 0.0, 0.0, 0.0, 1.0 ]
                },
                "scale": {
                    "type": "array",
                    "description": "The node's non-uniform scale, given as the scaling factors along the x, y, and z axes.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 3,
                    "maxItems": 3,
                    "default": [ 1.0, 1.0, 1.0 ]
                },
                "translation": {
                    "type": "array",
                    "description": "The node's translation along the x, y, and z axes.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 3,
                    "maxItems": 3,
                    "default": [ 0.0, 0.0, 0.0 ]
                },
                "weights": {
                    "type": "array",
                    "description": "The weights of the instantiated Morph Target. Number of elements must match number of Morph Targets of used mesh.",
                    "minItems": 1,
                    "items": {
                        "type": "number"
                    }
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "dependencies": {
                "weights": [ "mesh" ],
                "skin": [ "mesh" ]
            },
            "not": {
                "anyOf": [
                    { "required": [ "matrix", "translation" ] },
                    { "required": [ "matrix", "rotation" ] },
                    { "required": [ "matrix", "scale" ] }
                ]
            }
        }
        )rawstring";

    const char* const SchemaJson::accessor_sparse_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Accessor Sparse",
            "type": "object",
            "description": "Sparse storage of attributes that deviate from their initialization value.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "count": {
                    "type": "integer",
                    "description": "Number of entries stored in the sparse array.",
                    "minimum": 1,
                    "gltf_detailedDescription": "The number of attributes encoded in this sparse accessor."
                },
                "indices": {
                    "allOf": [ { "$ref": "accessor.sparse.indices.schema.json" } ],
                    "description": "Index array of size `count` that points to those accessor attributes that deviate from their initialization value. Indices must strictly increase."
                },
                "values": {
                    "allOf": [ { "$ref": "accessor.sparse.values.schema.json" } ],
                    "description": "Array of size `count` times number of components, storing the displaced accessor attributes pointed by `indices`. Substituted values must have the same `componentType` and number of components as the base accessor."
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "count", "indices", "values" ]
        }
        )rawstring";

    const char* const SchemaJson::asset_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Asset",
            "type": "object",
            "description": "Metadata about the glTF asset.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "copyright": {
                    "type": "string",
                    "description": "A copyright message suitable for display to credit the content creator."
                },
                "generator": {
                    "type": "string",
                    "description": "Tool that generated this glTF model.  Useful for debugging."
                },
                "version": {
                    "type": "string",
                    "description": "The glTF version that this asset targets.",
                    "pattern": "^[0-9]+\\.[0-9]+$"
                },
                "minVersion": {
                    "type": "string",
                    "description": "The minimum glTF version that this asset targets.",
                    "pattern": "^[0-9]+\\.[0-9]+$"
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "version" ]
        }
        )rawstring";

    const char* const SchemaJson::animation_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Animation",
            "type": "object",
            "description": "A keyframe animation.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "channels": {
                    "type": "array",
                    "description": "An array of channels, each of which targets an animation's sampler at a node's property. Different channels of the same animation can't have equal targets.",
                    "items": {
                        "$ref": "animation.channel.schema.json"
                    },
                    "minItems": 1
                },
                "samplers": {
                    "type": "array",
                    "description": "An array of samplers that combines input and output accessors with an interpolation algorithm to define a keyframe graph (but not its target).",
                    "items": {
                        "$ref": "animation.sampler.schema.json"
                    },
                    "minItems": 1
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "channels", "samplers" ]
        }
        )rawstring";

    const char* const SchemaJson::camera_orthographic_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Camera Orthographic",
            "type": "object",
            "description": "An orthographic camera containing properties to create an orthographic projection matrix.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "xmag": {
                    "type": "number",
                    "description": "The floating-point horizontal magnification of the view. Must not be zero."
                },
                "ymag": {
                    "type": "number",
                    "description": "The floating-point vertical magnification of the view. Must not be zero."
                },
                "zfar": {
                    "type": "number",
                    "description": "The floating-point distance to the far clipping plane. `zfar` must be greater than `znear`.",
                    "minimum": 0.0,
                    "exclusiveMinimum": true
                },
                "znear": {
                    "type": "number",
                    "description": "The floating-point distance to the near clipping plane.",
                    "minimum": 0.0
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "xmag", "ymag", "zfar", "znear" ]
        }
        )rawstring";

    const char* const SchemaJson::animation_channel_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Animation Channel",
            "type": "object",
            "description": "Targets an animation's sampler at a node's property.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "sampler": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of a sampler in this animation used to compute the value for the target.",
                    "gltf_detailedDescription": "The index of a sampler in this animation used to compute the value for the target, e.g., a node's translation, rotation, or scale (TRS)."
                },
                "target": {
                    "allOf": [ { "$ref": "animation.channel.target.schema.json" } ],
                    "description": "The index of the node and TRS property to target."
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "sampler", "target" ]
        }
        )rawstring";

    const char* const SchemaJson::accessor_sparse_values_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Accessor Sparse Values",
            "type": "object",
            "description": "Array of size `accessor.sparse.count` times number of components storing the displaced accessor attributes pointed by `accessor.sparse.indices`.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "bufferView": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the bufferView with sparse values. Referenced bufferView can't have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target."
                },
                "byteOffset": {
                    "type": "integer",
                    "description": "The offset relative to the start of the bufferView in bytes. Must be aligned.",
                    "minimum": 0,
                    "default": 0
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "bufferView" ]
        }
        )rawstring";

    const char* const SchemaJson::accessor_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Accessor",
            "type": "object",
            "description": "A typed view into a bufferView.  A bufferView contains raw binary data.  An accessor provides a typed view into a bufferView or a subset of a bufferView similar to how WebGL's `vertexAttribPointer()` defines an attribute in a buffer.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "bufferView": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the bufferView.",
                    "gltf_detailedDescription": "The index of the bufferView. When not defined, accessor must be initialized with zeros; `sparse` property or extensions could override zeros with actual values."
                },
                "byteOffset": {
                    "type": "integer",
                    "description": "The offset relative to the start of the bufferView in bytes.",
                    "minimum": 0,
                    "default": 0,
                    "gltf_detailedDescription": "The offset relative to the start of the bufferView in bytes.  This must be a multiple of the size of the component datatype.",
                    "gltf_webgl": "`vertexAttribPointer()` offset parameter"
                },
                "componentType": {
                    "description": "The datatype of components in the attribute.",
                    "gltf_detailedDescription": "The datatype of components in the attribute.  All valid values correspond to WebGL enums.  The corresponding typed arrays are `Int8Array`, `Uint8Array`, `Int16Array`, `Uint16Array`, `Uint32Array`, and `Float32Array`, respectively.  5125 (UNSIGNED_INT) is only allowed when the accessor contains indices, i.e., the accessor is only referenced by `primitive.indices`.",
                    "gltf_webgl": "`vertexAttribPointer()` type parameter",
                    "anyOf": [
                        {
                            "enum": [ 5120 ],
                            "description": "BYTE"
                        },
                        {
                            "enum": [ 5121 ],
                            "description": "UNSIGNED_BYTE"
                        },
                        {
                            "enum": [ 5122 ],
                            "description": "SHORT"
                        },
                        {
                            "enum": [ 5123 ],
                            "description": "UNSIGNED_SHORT"
                        },
                        {
                            "enum": [ 5125 ],
                            "description": "UNSIGNED_INT"
                        },
                        {
                            "enum": [ 5126 ],
                            "description": "FLOAT"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "normalized": {
                    "type": "boolean",
                    "description": "Specifies whether integer data values should be normalized.",
                    "default": false,
                    "gltf_detailedDescription": "Specifies whether integer data values should be normalized (`true`) to [0, 1] (for unsigned types) or [-1, 1] (for signed types), or converted directly (`false`) when they are accessed. This property is defined only for accessors that contain vertex attributes or animation output data.",
                    "gltf_webgl": "`vertexAttribPointer()` normalized parameter"
                },
                "count": {
                    "type": "integer",
                    "description": "The number of attributes referenced by this accessor.",
                    "minimum": 1,
                    "gltf_detailedDescription": "The number of attributes referenced by this accessor, not to be confused with the number of bytes or number of components."
                },
                "type": {
                    "description": "Specifies if the attribute is a scalar, vector, or matrix.",
                    "anyOf": [
                        {
                            "enum": [ "SCALAR" ]
                        },
                        {
                            "enum": [ "VEC2" ]
                        },
                        {
                            "enum": [ "VEC3" ]
                        },
                        {
                            "enum": [ "VEC4" ]
                        },
                        {
                            "enum": [ "MAT2" ]
                        },
                        {
                            "enum": [ "MAT3" ]
                        },
                        {
                            "enum": [ "MAT4" ]
                        },
                        {
                            "type": "string"
                        }
                    ]
                },
                "max": {
                    "type": "array",
                    "description": "Maximum value of each component in this attribute.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 1,
                    "maxItems": 16,
                    "gltf_detailedDescription": "Maximum value of each component in this attribute.  Array elements must be treated as having the same data type as accessor's `componentType`. Both min and max arrays have the same length.  The length is determined by the value of the type property; it can be 1, 2, 3, 4, 9, or 16.\n\n`normalized` property has no effect on array values: they always correspond to the actual values stored in the buffer. When accessor is sparse, this property must contain max values of accessor data with sparse substitution applied."
                },
                "min": {
                    "type": "array",
                    "description": "Minimum value of each component in this attribute.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 1,
                    "maxItems": 16,
                    "gltf_detailedDescription": "Minimum value of each component in this attribute.  Array elements must be treated as having the same data type as accessor's `componentType`. Both min and max arrays have the same length.  The length is determined by the value of the type property; it can be 1, 2, 3, 4, 9, or 16.\n\n`normalized` property has no effect on array values: they always correspond to the actual values stored in the buffer. When accessor is sparse, this property must contain min values of accessor data with sparse substitution applied."
                },
                "sparse": {
                    "allOf": [ { "$ref": "accessor.sparse.schema.json" } ],
                    "description": "Sparse storage of attributes that deviate from their initialization value."
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "dependencies": {
                "byteOffset": [ "bufferView" ]
            },
            "required": [ "componentType", "count", "type" ]
        }
        )rawstring";

    const char* const SchemaJson::skin_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Skin",
            "type": "object",
            "description": "Joints and matrices defining a skin.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "inverseBindMatrices": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the accessor containing the floating-point 4x4 inverse-bind matrices.  The default is that each matrix is a 4x4 identity matrix, which implies that inverse-bind matrices were pre-applied."
                },
                "skeleton": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the node used as a skeleton root. When undefined, joints transforms resolve to scene root."
                },
                "joints": {
                    "type": "array",
                    "description": "Indices of skeleton nodes, used as joints in this skin.",
                    "items": {
                        "$ref": "glTFid.schema.json"
                    },
                    "uniqueItems": true,
                    "minItems": 1,
                    "gltf_detailedDescription": "Indices of skeleton nodes, used as joints in this skin.  The array length must be the same as the `count` property of the `inverseBindMatrices` accessor (when defined)."
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "joints" ]
        }
        )rawstring";

    const char* const SchemaJson::material_pbrMetallicRoughness_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Material PBR Metallic Roughness",
            "type": "object",
            "description": "A set of parameter values that are used to define the metallic-roughness material model from Physically-Based Rendering (PBR) methodology.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "baseColorFactor": {
                    "type": "array",
                    "items": {
                        "type": "number",
                        "minimum": 0.0,
                        "maximum": 1.0
                    },
                    "description": "The material's base color factor.",
                    "default": [ 1.0, 1.0, 1.0, 1.0 ],
                    "minItems": 4,
                    "maxItems": 4,
                    "gltf_detailedDescription": "The RGBA components of the base color of the material. The fourth component (A) is the alpha coverage of the material. The `alphaMode` property specifies how alpha is interpreted. These values are linear. If a baseColorTexture is specified, this value is multiplied with the texel values."
                },
                "baseColorTexture": {
                    "allOf": [ { "$ref": "textureInfo.schema.json" } ],
                    "description": "The base color texture.",
                    "gltf_detailedDescription": "The base color texture. This texture contains RGB(A) components in sRGB color space. The first three components (RGB) specify the base color of the material. If the fourth component (A) is present, it represents the alpha coverage of the material. Otherwise, an alpha of 1.0 is assumed. The `alphaMode` property specifies how alpha is interpreted. The stored texels must not be premultiplied."
                },
                "metallicFactor": {
                    "type": "number",
                    "description": "The metalness of the material.",
                    "default": 1.0,
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "gltf_detailedDescription": "The metalness of the material. A value of 1.0 means the material is a metal. A value of 0.0 means the material is a dielectric. Values in between are for blending between metals and dielectrics such as dirty metallic surfaces. This value is linear. If a metallicRoughnessTexture is specified, this value is multiplied with the metallic texel values."
                },
                "roughnessFactor": {
                    "type": "number",
                    "description": "The roughness of the material.",
                    "default": 1.0,
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "gltf_detailedDescription": "The roughness of the material. A value of 1.0 means the material is completely rough. A value of 0.0 means the material is completely smooth. This value is linear. If a metallicRoughnessTexture is specified, this value is multiplied with the roughness texel values."
                },
                "metallicRoughnessTexture": {
                    "allOf": [ { "$ref": "textureInfo.schema.json" } ],
                    "description": "The metallic-roughness texture.",
                    "gltf_detailedDescription": "The metallic-roughness texture. The metalness values are sampled from the B channel. The roughness values are sampled from the G channel. These values are linear. If other channels are present (R or A), they are ignored for metallic-roughness calculations."
                },
                "extensions": { },
                "extras": { }
            }
        })rawstring";

    const char* const SchemaJson::scene_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Scene",
            "type": "object",
            "description": "The root nodes of a scene.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "nodes": {
                    "type": "array",
                    "description": "The indices of each root node.",
                    "items": {
                        "$ref": "glTFid.schema.json"
                    },
                    "uniqueItems": true,
                    "minItems": 1
                },
                "name": { },
                "extensions": { },
                "extras": { }
            }
        }
        )rawstring";

    const char* const SchemaJson::glTFProperty_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "glTF Property",
            "type": "object",
            "properties": {
                "extensions": {
                    "$ref": "extension.schema.json"
                },
                "extras": {
                    "$ref": "extras.schema.json"
                }
            }
        }
        )rawstring";

    const char* const SchemaJson::material_normalTextureInfo_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Material Normal Texture Info",
            "type": "object",
            "allOf": [ { "$ref": "textureInfo.schema.json" } ],
            "properties": {
                "index": { },
                "texCoord": { },
                "scale": {
                    "type": "number",
                    "description": "The scalar multiplier applied to each normal vector of the normal texture.",
                    "default": 1.0,
                    "gltf_detailedDescription": "The scalar multiplier applied to each normal vector of the texture. This value scales the normal vector using the formula: `scaledNormal =  normalize((normalize(<sampled normal texture value>) * 2.0 - 1.0) * vec3(<normal scale>, <normal scale>, 1.0))`. This value is ignored if normalTexture is not specified. This value is linear."
                },
                "extensions": { },
                "extras": { }
            }
        })rawstring";

    const char* const SchemaJson::mesh_primitive_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Mesh Primitive",
            "type": "object",
            "description": "Geometry to be rendered with the given material.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "attributes": {
                    "type": "object",
                    "description": "A dictionary object, where each key corresponds to mesh attribute semantic and each value is the index of the accessor containing attribute's data.",
                    "minProperties": 1,
                    "additionalProperties": {
                        "$ref": "glTFid.schema.json"
                    }
                },
                "indices": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the accessor that contains the indices.",
                    "gltf_detailedDescription": "The index of the accessor that contains mesh indices.  When this is not defined, the primitives should be rendered without indices using `drawArrays()`.  When defined, the accessor must contain indices: the `bufferView` referenced by the accessor should have a `target` equal to 34963 (ELEMENT_ARRAY_BUFFER); `componentType` must be 5121 (UNSIGNED_BYTE), 5123 (UNSIGNED_SHORT) or 5125 (UNSIGNED_INT), the latter may require enabling additional hardware support; `type` must be `\"SCALAR\"`. For triangle primitives, the front face has a counter-clockwise (CCW) winding order."
                },
                "material": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the material to apply to this primitive when rendering."
                },
                "mode": {
                    "description": "The type of primitives to render.",
                    "default": 4,
                    "gltf_detailedDescription": "The type of primitives to render. All valid values correspond to WebGL enums.",
                    "anyOf": [
                        {
                            "enum": [ 0 ],
                            "description": "POINTS"
                        },
                        {
                            "enum": [ 1 ],
                            "description": "LINES"
                        },
                        {
                            "enum": [ 2 ],
                            "description": "LINE_LOOP"
                        },
                        {
                            "enum": [ 3 ],
                            "description": "LINE_STRIP"
                        },
                        {
                            "enum": [ 4 ],
                            "description": "TRIANGLES"
                        },
                        {
                            "enum": [ 5 ],
                            "description": "TRIANGLE_STRIP"
                        },
                        {
                            "enum": [ 6 ],
                            "description": "TRIANGLE_FAN"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "targets": {
                    "type": "array",
                    "description": "An array of Morph Targets, each  Morph Target is a dictionary mapping attributes (only `POSITION`, `NORMAL`, and `TANGENT` supported) to their deviations in the Morph Target.",
                    "items": {
                        "type": "object",
                        "minProperties": 1,
                        "additionalProperties": {
                            "$ref": "glTFid.schema.json"
                        },
                        "description": "A dictionary object specifying attributes displacements in a Morph Target, where each key corresponds to one of the three supported attribute semantic (`POSITION`, `NORMAL`, or `TANGENT`) and each value is the index of the accessor containing the attribute displacements' data."
                    },
                    "minItems": 1
                },
                "extensions": { },
                "extras": { }
            },
            "gltf_webgl": "`drawElements()` and `drawArrays()`",
            "required": [ "attributes" ]
        }
        )rawstring";

    const char* const SchemaJson::mesh_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Mesh",
            "type": "object",
            "description": "A set of primitives to be rendered.  A node can contain one mesh.  A node's transform places the mesh in the scene.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "primitives": {
                    "type": "array",
                    "description": "An array of primitives, each defining geometry to be rendered with a material.",
                    "items": {
                        "$ref": "mesh.primitive.schema.json"
                    },
                    "minItems": 1
                },
                "weights": {
                    "type": "array",
                    "description": "Array of weights to be applied to the Morph Targets.",
                    "items": {
                        "type": "number"
                    },
                    "minItems": 1
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "primitives" ]
        }
        )rawstring";

    const char* const SchemaJson::camera_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Camera",
            "type": "object",
            "description": "A camera's projection.  A node can reference a camera to apply a transform to place the camera in the scene.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "orthographic": {
                    "allOf": [ { "$ref": "camera.orthographic.schema.json" } ],
                    "description": "An orthographic camera containing properties to create an orthographic projection matrix."
                },
                "perspective": {
                    "allOf": [ { "$ref": "camera.perspective.schema.json" } ],
                    "description": "A perspective camera containing properties to create a perspective projection matrix."
                },
                "type": {
                    "description": "Specifies if the camera uses a perspective or orthographic projection.",
                    "gltf_detailedDescription": "Specifies if the camera uses a perspective or orthographic projection.  Based on this, either the camera's `perspective` or `orthographic` property will be defined.",
                    "anyOf": [
                        {
                            "enum": [ "perspective" ]
                        },
                        {
                            "enum": [ "orthographic" ]
                        },
                        {
                            "type": "string"
                        }
                    ]
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "type" ],
            "not": {
                "required": [ "perspective", "orthographic" ]
            }
        }
        )rawstring";

    const char* const SchemaJson::extras_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Extras",
            "description": "Application-specific data."
        }
        )rawstring";

    const char* const SchemaJson::accessor_sparse_indices_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Accessor Sparse Indices",
            "type": "object",
            "description": "Indices of those attributes that deviate from their initialization value.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "bufferView": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the bufferView with sparse indices. Referenced bufferView can't have ARRAY_BUFFER or ELEMENT_ARRAY_BUFFER target."
                },
                "byteOffset": {
                    "type": "integer",
                    "description": "The offset relative to the start of the bufferView in bytes. Must be aligned.",
                    "minimum": 0,
                    "default": 0
                },
                "componentType": {
                    "description": "The indices data type.",
                    "gltf_detailedDescription": "The indices data type.  Valid values correspond to WebGL enums: `5121` (UNSIGNED_BYTE), `5123` (UNSIGNED_SHORT), `5125` (UNSIGNED_INT).",
                    "anyOf": [
                        {
                            "enum": [ 5121 ],
                            "description": "UNSIGNED_BYTE"
                        },
                        {
                            "enum": [ 5123 ],
                            "description": "UNSIGNED_SHORT"
                        },
                        {
                            "enum": [ 5125 ],
                            "description": "UNSIGNED_INT"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "bufferView", "componentType" ]
        }
        )rawstring";

    const char* const SchemaJson::camera_perspective_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Camera Perspective",
            "type": "object",
            "description": "A perspective camera containing properties to create a perspective projection matrix.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "aspectRatio": {
                    "type": "number",
                    "description": "The floating-point aspect ratio of the field of view.",
                    "minimum": 0.0,
                    "exclusiveMinimum": true,
                    "gltf_detailedDescription": "The floating-point aspect ratio of the field of view. When this is undefined, the aspect ratio of the canvas is used."
                },
                "yfov": {
                    "type": "number",
                    "description": "The floating-point vertical field of view in radians.",
                    "minimum": 0.0,
                    "exclusiveMinimum": true
                },
                "zfar": {
                    "type": "number",
                    "description": "The floating-point distance to the far clipping plane.",
                    "minimum": 0.0,
                    "exclusiveMinimum": true,
                    "gltf_detailedDescription": "The floating-point distance to the far clipping plane. When defined, `zfar` must be greater than `znear`. If `zfar` is undefined, runtime must use infinite projection matrix."
                },
                "znear": {
                    "type": "number",
                    "description": "The floating-point distance to the near clipping plane.",
                    "minimum": 0.0,
                    "exclusiveMinimum": true,
                    "gltf_detailedDescription": "The floating-point distance to the near clipping plane."
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "yfov", "znear" ]
        }
        )rawstring";

    const char* const SchemaJson::texture_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Texture",
            "type": "object",
            "description": "A texture and its sampler.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "sampler": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the sampler used by this texture. When undefined, a sampler with repeat wrapping and auto filtering should be used."
                },
                "source": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the image used by this texture."
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "gltf_webgl": "`createTexture()`, `deleteTexture()`, `bindTexture()`, `texImage2D()`, and `texParameterf()`"
        }
        )rawstring";

    const char* const SchemaJson::textureInfo_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Texture Info",
            "type": "object",
            "description": "Reference to a texture.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "index": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the texture."
                },
                "texCoord": {
                    "type": "integer",
                    "description": "The set index of texture's TEXCOORD attribute used for texture coordinate mapping.",
                    "default": 0,
                    "minimum": 0,
                    "gltf_detailedDescription": "This integer value is used to construct a string in the format `TEXCOORD_<set index>` which is a reference to a key in mesh.primitives.attributes (e.g. A value of `0` corresponds to `TEXCOORD_0`). Mesh must have corresponding texture coordinate attributes for the material to be applicable to it."
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "index" ]
        })rawstring";

    const char* const SchemaJson::material_occlusionTextureInfo_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Material Occlusion Texture Info",
            "type": "object",
            "allOf": [ { "$ref": "textureInfo.schema.json" } ],
            "properties": {
                "index": { },
                "texCoord": { },
                "strength": {
                    "type": "number",
                    "description": "A scalar multiplier controlling the amount of occlusion applied.",
                    "default": 1.0,
                    "minimum": 0.0,
                    "maximum": 1.0,
                    "gltf_detailedDescription": "A scalar multiplier controlling the amount of occlusion applied. A value of 0.0 means no occlusion. A value of 1.0 means full occlusion. This value affects the resulting color using the formula: `occludedColor = lerp(color, color * <sampled occlusion texture value>, <occlusion strength>)`. This value is ignored if the corresponding texture is not specified. This value is linear."
                },
                "extensions": { },
                "extras": { }
            }
        })rawstring";

    const char* const SchemaJson::bufferView_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Buffer View",
            "type": "object",
            "description": "A view into a buffer generally representing a subset of the buffer.",
            "allOf": [ { "$ref": "glTFChildOfRootProperty.schema.json" } ],
            "properties": {
                "buffer": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the buffer."
                },
                "byteOffset": {
                    "type": "integer",
                    "description": "The offset into the buffer in bytes.",
                    "minimum": 0,
                    "default": 0
                },
                "byteLength": {
                    "type": "integer",
                    "description": "The length of the bufferView in bytes.",
                    "minimum": 1
                },
                "byteStride": {
                    "type": "integer",
                    "description": "The stride, in bytes.",
                    "minimum": 4,
                    "maximum": 252,
                    "multipleOf": 4,
                    "gltf_detailedDescription": "The stride, in bytes, between vertex attributes.  When this is not defined, data is tightly packed. When two or more accessors use the same bufferView, this field must be defined.",
                    "gltf_webgl": "`vertexAttribPointer()` stride parameter"
                },
                "target": {
                    "description": "The target that the GPU buffer should be bound to.",
                    "gltf_webgl": "`bindBuffer()`",
                    "anyOf": [
                        {
                            "enum": [ 34962 ],
                            "description": "ARRAY_BUFFER"
                        },
                        {
                            "enum": [ 34963 ],
                            "description": "ELEMENT_ARRAY_BUFFER"
                        },
                        {
                            "type": "integer"
                        }
                    ]
                },
                "name": { },
                "extensions": { },
                "extras": { }
            },
            "required": [ "buffer", "byteLength" ]
        }
        )rawstring";

    const char* const SchemaJson::animation_channel_target_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "Animation Channel Target",
            "type": "object",
            "description": "The index of the node and TRS property that an animation channel targets.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "node": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the node to target."
                },
                "path": {
                    "description": "The name of the node's TRS property to modify, or the \"weights\" of the Morph Targets it instantiates. For the \"translation\" property, the values that are provided by the sampler are the translation along the x, y, and z axes. For the \"rotation\" property, the values are a quaternion in the order (x, y, z, w), where w is the scalar. For the \"scale\" property, the values are the scaling factors along the x, y, and z axes.",
                    "anyOf": [
                        {
                            "enum": [ "translation" ]
                        },
                        {
                            "enum": [ "rotation" ]
                        },
                        {
                            "enum": [ "scale" ]
                        },
                        {
                            "enum": [ "weights" ]
                        },
                        {
                            "type": "string"
                        }
                    ]
                },
                "extensions": { },
                "extras": { }
            },
            "required": [ "path" ]
        }
        )rawstring";

    const char* const SchemaJson::glTFid_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "glTF Id",
            "type": "integer",
            "minimum": 0
        }
        )rawstring";

    const char* const SchemaJson::glTF_schema_json = R"rawstring(
        {
            "$schema": "http://json-schema.org/draft-04/schema",
            "title": "glTF",
            "type": "object",
            "description": "The root object for a glTF asset.",
            "allOf": [ { "$ref": "glTFProperty.schema.json" } ],
            "properties": {
                "extensionsUsed": {
                    "type": "array",
                    "description": "Names of glTF extensions used somewhere in this asset.",
                    "items": {
                        "type": "string"
                    },
                    "uniqueItems": true,
                    "minItems": 1
                },
                "extensionsRequired": {
                    "type": "array",
                    "description": "Names of glTF extensions required to properly load this asset.",
                    "items": {
                        "type": "string"
                    },
                    "uniqueItems": true,
                    "minItems": 1
                },
                "accessors": {
                    "type": "array",
                    "description": "An array of accessors.",
                    "items": {
                        "$ref": "accessor.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of accessors.  An accessor is a typed view into a bufferView."
                },
                "animations": {
                    "type": "array",
                    "description": "An array of keyframe animations.",
                    "items": {
                        "$ref": "animation.schema.json"
                    },
                    "minItems": 1
                },
                "asset": {
                    "allOf": [ { "$ref": "asset.schema.json" } ],
                    "description": "Metadata about the glTF asset."
                },
                "buffers": {
                    "type": "array",
                    "description": "An array of buffers.",
                    "items": {
                        "$ref": "buffer.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of buffers.  A buffer points to binary geometry, animation, or skins."
                },
                "bufferViews": {
                    "type": "array",
                    "description": "An array of bufferViews.",
                    "items": {
                        "$ref": "bufferView.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of bufferViews.  A bufferView is a view into a buffer generally representing a subset of the buffer."
                },
                "cameras": {
                    "type": "array",
                    "description": "An array of cameras.",
                    "items": {
                        "$ref": "camera.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of cameras.  A camera defines a projection matrix."
                },
                "images": {
                    "type": "array",
                    "description": "An array of images.",
                    "items": {
                        "$ref": "image.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of images.  An image defines data used to create a texture."
                },
                "materials": {
                    "type": "array",
                    "description": "An array of materials.",
                    "items": {
                        "$ref": "material.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of materials.  A material defines the appearance of a primitive."
                },
                "meshes": {
                    "type": "array",
                    "description": "An array of meshes.",
                    "items": {
                        "$ref": "mesh.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of meshes.  A mesh is a set of primitives to be rendered."
                },
                "nodes": {
                    "type": "array",
                    "description": "An array of nodes.",
                    "items": {
                        "$ref": "node.schema.json"
                    },
                    "minItems": 1
                },
                "samplers": {
                    "type": "array",
                    "description": "An array of samplers.",
                    "items": {
                        "$ref": "sampler.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of samplers.  A sampler contains properties for texture filtering and wrapping modes."
                },
                "scene": {
                    "allOf": [ { "$ref": "glTFid.schema.json" } ],
                    "description": "The index of the default scene."
                },
                "scenes": {
                    "type": "array",
                    "description": "An array of scenes.",
                    "items": {
                        "$ref": "scene.schema.json"
                    },
                    "minItems": 1
                },
                "skins": {
                    "type": "array",
                    "description": "An array of skins.",
                    "items": {
                        "$ref": "skin.schema.json"
                    },
                    "minItems": 1,
                    "gltf_detailedDescription": "An array of skins.  A skin is defined by joints and matrices."
                },
                "textures": {
                    "type": "array",
                    "description": "An array of textures.",
                    "items": {
                        "$ref": "texture.schema.json"
                    },
                    "minItems": 1
                },
                "extensions": { },
                "extras": { }
            },
            "dependencies": {
                "scene": [ "scenes" ]
            },
            "required": [ "asset" ]
        }
        )rawstring";

    const std::unordered_map<std::string, std::string> SchemaJson::GLTF_SCHEMA_MAP =
    {
        { "material.schema.json", material_schema_json },
        { "glTFChildOfRootProperty.schema.json", glTFChildOfRootProperty_schema_json },
        { "image.schema.json", image_schema_json },
        { "animation.sampler.schema.json", animation_sampler_schema_json },
        { "sampler.schema.json", sampler_schema_json },
        { "extension.schema.json", extension_schema_json },
        { "buffer.schema.json", buffer_schema_json },
        { "node.schema.json", node_schema_json },
        { "accessor.sparse.schema.json", accessor_sparse_schema_json },
        { "asset.schema.json", asset_schema_json },
        { "animation.schema.json", animation_schema_json },
        { "camera.orthographic.schema.json", camera_orthographic_schema_json },
        { "animation.channel.schema.json", animation_channel_schema_json },
        { "accessor.sparse.values.schema.json", accessor_sparse_values_schema_json },
        { "accessor.schema.json", accessor_schema_json },
        { "skin.schema.json", skin_schema_json },
        { "material.pbrMetallicRoughness.schema.json", material_pbrMetallicRoughness_schema_json },
        { "scene.schema.json", scene_schema_json },
        { "glTFProperty.schema.json", glTFProperty_schema_json },
        { "material.normalTextureInfo.schema.json", material_normalTextureInfo_schema_json },
        { "mesh.primitive.schema.json", mesh_primitive_schema_json },
        { "mesh.schema.json", mesh_schema_json },
        { "camera.schema.json", camera_schema_json },
        { "extras.schema.json", extras_schema_json },
        { "accessor.sparse.indices.schema.json", accessor_sparse_indices_schema_json },
        { "camera.perspective.schema.json", camera_perspective_schema_json },
        { "texture.schema.json", texture_schema_json },
        { "textureInfo.schema.json", textureInfo_schema_json },
        { "material.occlusionTextureInfo.schema.json", material_occlusionTextureInfo_schema_json },
        { "bufferView.schema.json", bufferView_schema_json },
        { "animation.channel.target.schema.json", animation_channel_target_schema_json },
        { "glTFid.schema.json", glTFid_schema_json },
        { "glTF.schema.json", glTF_schema_json } };
    

  }; // end namespace glTF scope

}; // end namespace Microsoft scope

