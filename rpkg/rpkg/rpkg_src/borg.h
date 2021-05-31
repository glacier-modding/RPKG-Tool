#pragma once
#include "hash.h"
#include "global.h"
#include "thirdparty/directxmath/DirectXMath.h"
#include "thirdparty/directxmath/DirectXCollision.h"
#include "thirdparty/directxmath/DirectXColors.h"
#include "thirdparty/directxmath/DirectXPackedVector.h"
#include <string>
#include <vector>
#include <map>
#include <stdint.h>

class borg
{
public:
	borg();
	borg(uint64_t rpkgs_index, uint64_t hash_index);

	std::string borg_file_name = "";
	uint32_t borg_rpkg_index = 0;
	uint32_t borg_hash_index = 0;
	std::vector<char> borg_input_data;
	std::vector<char> borg_output_data;
	std::vector<char> borg_data;
    uint32_t borg_position = 0;
    uint32_t borg_primary_header_offset = 0;
    uint32_t bones_count = 0;
    uint32_t bones_count_animated = 0;
    uint32_t bones_offset = 0;
    uint32_t poses_offset = 0;
    uint32_t poses_inverse_matrices = 0;
    uint32_t bones_constraints = 0;
    uint32_t poses_header = 0;
    uint32_t bones_invert = 0;
    uint32_t bones_map = 0;
    std::vector<bone_data> bones_data;
    std::vector<vector4> bones_position;
    std::vector<float> bones_positions;
    std::vector<DirectX::XMMATRIX> bones_inverse_bind_matrix;
    std::vector<DirectX::XMMATRIX> bones_bind_matrix;
    std::vector<DirectX::XMMATRIX> bones_local_transforms;
    std::vector<float> bones_inverse_bind_matrices;
};