#include "global.h"

std::vector<rpkg> rpkgs;
int percent_progress = 0;
std::string response_string = "";
std::string timing_string = "";
std::vector<char> response_data;
std::string task_status_string = "";
int task_single_status = READY;
int task_multiple_status = READY;
int gui_control = READY;
std::string localization_string = "";
std::string hashes_with_no_reverse_depends = "";
std::string hash_direct_depends = "";
std::string hash_list_string = "";
std::string patch_deletion_list_string = "";
std::vector<std::string> hashes_depends_map_rpkg_file_paths;
std::vector<std::map<uint64_t, uint64_t>> hashes_depends_map;
int hash_list_version = 0;
bool hash_list_loaded = false;
std::map<uint64_t, uint64_t> hash_list_hash_map;
std::vector<std::string> hash_list_hash_file_names;
std::vector<std::string> hash_list_hash_value_strings;
std::vector<std::string> hash_list_hash_strings;
std::string search_imported_hashes_string = "";
std::string search_hash_list_string = "";
std::string rpkg_file_paths_hash_is_in = "";
int pcm_sample_size = 0;
int pcm_sample_rate = 0;
int pcm_channels = 0;
std::string extracted_wem_string = "";
std::map<uint32_t, std::string>* property_map;
std::map<std::string, std::map<uint32_t, std::string>>* enum_map;
bool property_map_initialized = false;
bool enum_map_initialized = false;
std::vector<temp> temps;
std::map<uint64_t, uint32_t> temps_map;
std::vector<matrix43> temp_world_coordinates;
std::vector<std::string> temp_world_coordinates_property_names;
std::map<uint32_t, uint32_t> temp_world_coordinates_map;
std::vector<std::string> prim_asset_file_names;
uint32_t prim_asset_file_count = 0;
std::string map_editor_output_path = "";
std::vector<std::string> map_editor_parents;
std::vector<std::string> map_editor_property_names;
std::vector<std::string> map_editor_matrixes;
std::vector<std::vector<std::string>> map_editor_glb_file_names;
std::map<std::string, uint32_t> map_editor_prim_file_names;
std::map<std::string, uint32_t> map_editor_resource_map;
std::string map_editor_godot_resources = "";
std::string map_editor_godot_nodes = "";
std::string map_editor_temp_matrix = "";
std::vector<std::string> map_editor_godot_import_property_strings;
std::map<std::string, uint32_t> map_editor_godot_import_property_strings_map;
std::vector<matrix43> map_editor_godot_import_matrixes;
std::vector<uint64_t> entity_id_master_list;
std::map<uint64_t, uint64_t> entity_id_master_list_map;
std::vector<uint32_t> entity_id_tblu_index;
std::vector<uint32_t> entity_id_temps_index;
std::vector<uint32_t> entity_id_logicalParent;
std::vector<std::string> entity_id_logicalParent_string;
std::vector<uint64_t> entity_id_m_eidParent;
std::vector<bool> entity_id_has_matrix;

void initialize_property_map()
{
    if (!property_map_initialized)
    {
        property_map_initialized = true;

        property_map = new std::map<uint32_t, std::string>();

        (*property_map)[0x267A966C] = "m_bActive";
        (*property_map)[0x46ED288D] = "m_rFacePunchEmitterDecal";
        (*property_map)[0xB1580D32] = "m_bShowOnMenuMap";
        (*property_map)[0x127C2AC2] = "m_rEventModifier";
        (*property_map)[0xE8548959] = "postInitPropertyValues";
        (*property_map)[0x95632B3F] = "m_pEnumResource";
        (*property_map)[0xA64F2FD6] = "m_eCoverType";
        (*property_map)[0xF746B021] = "m_fSleepEnergyThreshold";
        (*property_map)[0x5AEF2EF1] = "m_vVector0";
        (*property_map)[0xF73AFC00] = "m_eSoundCollisionType";
        (*property_map)[0x31BEEB43] = "m_aBrickSelectorList";
        (*property_map)[0x68C60D5E] = "m_pLeader";
        (*property_map)[0x3D7D91E] = "m_nZoneIndex";
        (*property_map)[0x97AAAEEA] = "m_rItem";
        (*property_map)[0x93DDC11] = "m_sId";
        (*property_map)[0x4F438C89] = "m_bPacifyRelevant";
        (*property_map)[0x4E92AD8C] = "m_GlowStashedGunsOutlineColor";
        (*property_map)[0x72D2BACD] = "m_Disabled";
        (*property_map)[0x36EFB882] = "m_bAllOnNoRequirements";
        (*property_map)[0x4FADB2E5] = "m_rPlayerCandidateForSpreading";
        (*property_map)[0x66EC437] = "m_ClosedColor";
        (*property_map)[0xD44E38BD] = "m_bSuspiciousItem";
        (*property_map)[0xF9CAF64C] = "m_sGameChangerName";
        (*property_map)[0x9D4ABF17] = "m_aChildNetworks";
        (*property_map)[0x8625A29A] = "m_rEnabled";
        (*property_map)[0xAD4B46ED] = "m_aEntityPath";
        (*property_map)[0xC8C731A] = "m_pView";
        (*property_map)[0x16436610] = "m_pController";
        (*property_map)[0x30C1E2EC] = "m_fScalar1";
        (*property_map)[0x2ECB6C62] = "m_eIndicatorType";
        (*property_map)[0xAC99E46E] = "m_bList";
        (*property_map)[0x7B24AA84] = "m_aFlowCost";
        (*property_map)[0x165507F5] = "m_bStopSpeakOnLeave";
        (*property_map)[0x28FA13F4] = "m_eMaxMoveSpeed";
        (*property_map)[0x19ED66DF] = "m_vVelocity";
        (*property_map)[0x4A326B16] = "m_bMainItemVisibleOnSpawn";
        (*property_map)[0xE94B61FC] = "m_bNotIgnoreAccidents";
        (*property_map)[0x7C91483A] = "m_fDepthSizeX";
        (*property_map)[0x47C6D27A] = "m_fScalar0";
        (*property_map)[0x1CE005C1] = "m_bScopedWeapon";
        (*property_map)[0xB6D06355] = "m_fDisturbanceSize";
        (*property_map)[0x8F88565] = "m_bWallPiercing";
        (*property_map)[0xD3E50F9B] = "m_fImpactEventCooldownTime";
        (*property_map)[0xB5955EAE] = "m_fVisitedLocationsInfluenceDistance";
        (*property_map)[0x2366B378] = "m_LastVisible";
        (*property_map)[0xCD2488A9] = "m_bHandled";
        (*property_map)[0x292A3E51] = "m_aTargetConditions";
        (*property_map)[0xD3DFC2B2] = "m_Orientation";
        (*property_map)[0xC5EAF544] = "m_pConnectedTo";
        (*property_map)[0x3337C070] = "m_eTypeId";
        (*property_map)[0xD48AC024] = "m_fInfectionRate";
        (*property_map)[0x2DE81E67] = "m_vVector1";
        (*property_map)[0x50DE12E5] = "m_ePriority";
        (*property_map)[0x9352CC62] = "m_ePhysicsType";
        (*property_map)[0x634B42F9] = "m_fDepthExportFarZ";
        (*property_map)[0xC3E67F4B] = "m_vVector3";
        (*property_map)[0x187C2257] = "m_rMappingPosition";
        (*property_map)[0xBE472EB6] = "m_rOperateSequence";
        (*property_map)[0x37F32C3F] = "m_ridSplatDecalMaterial2";
        (*property_map)[0xB4E14FDD] = "m_vVector2";
        (*property_map)[0xD0F049AE] = "m_25DProjectionSettings";
        (*property_map)[0x58D8403] = "m_eMotionSwing1";
        (*property_map)[0x8B646E9] = "m_pGizmo";
        (*property_map)[0x5CB1F0C0] = "m_WaterTessellationExp";
        (*property_map)[0xC5E088AA] = "m_fDopplerLevel";
        (*property_map)[0x1F2C406C] = "m_bIsHidden";
        (*property_map)[0x66F32D9F] = "m_fLastKillTimestamp";
        (*property_map)[0xCFFE04A3] = "m_fSurfaceAdherence";
        (*property_map)[0x539A4544] = "m_fC";
        (*property_map)[0x4D522D1A] = "m_eEmotionLimit";
        (*property_map)[0xA56E163B] = "m_bSoundCuriousAccident";
        (*property_map)[0xC742418] = "m_bumpsHistory";
        (*property_map)[0xEA0770B9] = "m_bForceShowOnLoadingScreen";
        (*property_map)[0xA9C8B356] = "m_fScalar2";
        (*property_map)[0xC46D509D] = "m_aViewportLockedStates";
        (*property_map)[0xE089C6F7] = "m_nGlowAlliesFadeSpeed";
        (*property_map)[0x43AA3289] = "m_aTrespassingRules";
        (*property_map)[0x28FF477A] = "m_fBendStiffness";
        (*property_map)[0xBECEEC00] = "m_nPieceIndex";
        (*property_map)[0x949E6F13] = "m_aActionModifiers";
        (*property_map)[0x8DF18141] = "m_fStickLength";
        (*property_map)[0x9C54AE76] = "m_pCollisionImpactEvent";
        (*property_map)[0xDECF83C0] = "m_fScalar3";
        (*property_map)[0x80A49450] = "m_targetEntity";
        (*property_map)[0x37FB5A4F] = "m_fFloatValue";
        (*property_map)[0x5D82EAE8] = "m_vVector4";
        (*property_map)[0x710BCEE5] = "m_fCurveStartFractionSpine";
        (*property_map)[0x960CFB1] = "m_fDrawDistance";
        (*property_map)[0xF897C67F] = "m_fMaxFollowTime";
        (*property_map)[0x2531DE32] = "m_aEmitters";
        (*property_map)[0x40BCB7BF] = "m_rDialogResource";
        (*property_map)[0xD94E8A19] = "m_bLerpNoise";
        (*property_map)[0x6C14EA88] = "m_AlignRotation";
        (*property_map)[0x6F2D6DB1] = "m_fSpringMassPrimedMedium";
        (*property_map)[0xC335EF93] = "bDefaultDeathSound";
        (*property_map)[0x40AB1663] = "m_fScalar4";
        (*property_map)[0xED7CDC15] = "m_pInstinctControl";
        (*property_map)[0x55343506] = "selectedIndex";
        (*property_map)[0xE8A1BA94] = "m_rSplineEntity";
        (*property_map)[0xE7A99017] = "m_fAttackTime";
        (*property_map)[0x14CF59EB] = "rootEntityIndex";
        (*property_map)[0x2A85DA7E] = "m_vVector5";
        (*property_map)[0x9F25BF0A] = "m_eEvent";
        (*property_map)[0xFF42B902] = "m_MieColor";
        (*property_map)[0x23F64217] = "FlagAlpha";
        (*property_map)[0xDEF8D622] = "m_GlowVSOpponentOutlineColor";
        (*property_map)[0x7EA3B279] = "m_bLeftHandEnablePositionOffset";
        (*property_map)[0x7E70A3ED] = "m_bIsEntering";
        (*property_map)[0x285DD62D] = "m_vOriginQuaternion";
        (*property_map)[0x8BC2CECF] = "m_fCachedFrameWeight";
        (*property_map)[0x199E17AE] = "m_fHuntedPeripheralAngle";
        (*property_map)[0xF1FBD595] = "m_eEndEffectorID";
        (*property_map)[0x5E9172A3] = "m_bPreMergeSegments";
        (*property_map)[0x11170FDF] = "m_rSwitchFireType";
        (*property_map)[0x37AC26F5] = "m_fScalar5";
        (*property_map)[0x59308930] = "types";
        (*property_map)[0x378CCF24] = "m_assignedBodyguards";
        (*property_map)[0xA19A8708] = "m_nPatrolWaypointSubIndex";
        (*property_map)[0xCEE310BD] = "m_nScope";
        (*property_map)[0x32EBDA27] = "m_bKinematic";
        (*property_map)[0x1143890B] = "m_rBystanderPoint";
        (*property_map)[0xF4D2324C] = "m_AmmoItemID";
        (*property_map)[0xCE2DE31E] = "m_ZOffsetOfRearPlane";
        (*property_map)[0x7F50D701] = "m_bEnableBoundsOptimization";
        (*property_map)[0xDCC60F3A] = "m_nHMCrouchReactionDelay";
        (*property_map)[0xC5499140] = "m_AllowedProfessions";
        (*property_map)[0x23C4F02C] = "m_rAgilityConfigs";
        (*property_map)[0x996E9C10] = "m_bGlobalLightOverrideDisableSpec";
        (*property_map)[0x2A382E05] = "m_pPropLocation";
        (*property_map)[0xACC1435D] = "m_vCover";
        (*property_map)[0x76C76AA6] = "m_pAlignGizmo";
        (*property_map)[0xFF862FB6] = "m_dtAnchorLookResetTimer";
        (*property_map)[0x57698A6A] = "role";
        (*property_map)[0x604799D] = "m_aActIdentifiers";
        (*property_map)[0x9A3E17AD] = "m_nBulletDecalNumVariationsY";
        (*property_map)[0xC9B5F2CC] = "m_bApplyPromptDescriptionText";
        (*property_map)[0x97D479FE] = "m_vAngularVelocityInAxis";
        (*property_map)[0xE8CF492C] = "m_tMove";
        (*property_map)[0x9040155F] = "m_bSendFromHost";
        (*property_map)[0x73CC2CAD] = "FlagScaleX";
        (*property_map)[0x229B037] = "m_mDynamicObjectIDGenerationKeys";
        (*property_map)[0x294F8943] = "m_3DWorldOffset";
        (*property_map)[0x4569832B] = "m_sDataSourceOnlinePath";
        (*property_map)[0xD1B56F5F] = "m_vCoverDir";
        (*property_map)[0xB256B8E8] = "m_fKillOnCollisionChance";
        (*property_map)[0x3EC17D78] = "m_bMonitorHitman";
        (*property_map)[0xBEF1D57D] = "m_bIsLyingGround";
        (*property_map)[0x395A19F7] = "m_vSubPosition1";
        (*property_map)[0xBFF15344] = "m_fVignetteGradientLightStrength";
        (*property_map)[0x5DBD55A8] = "m_nGridCRC";
        (*property_map)[0x8CDC1683] = "x";
        (*property_map)[0xB88F97C7] = "m_sChannel";
        (*property_map)[0x3039EC80] = "m_VerticalRange";
        (*property_map)[0xD27C45FC] = "m_MathMultipliesData_float32";
        (*property_map)[0x921F4345] = "m_eTargetBone";
        (*property_map)[0x9DF68FE1] = "m_fLethalVelocity";
        (*property_map)[0x8F5AB0A2] = "m_nReactionNearDelay1";
        (*property_map)[0xFBDB2615] = "y";
        (*property_map)[0xA6A668B] = "m_DeadBodyVisibleDispatchRules";
        (*property_map)[0x8BFB37AE] = "missingText";
        (*property_map)[0x21BAF8B4] = "m_mainStateStartTime";
        (*property_map)[0x22BAA009] = "m_bIsPlaying";
        (*property_map)[0x48CF1919] = "m_fTimer";
        (*property_map)[0xBC8AA974] = "m_pContainerControl";
        (*property_map)[0x916B06E7] = "h";
        (*property_map)[0xFC9293CE] = "m_bIsCrowdAmbient";
        (*property_map)[0xF7624743] = "m_bSomeoneHandledBodybag";
        (*property_map)[0xDD841736] = "m_bStandUp";
        (*property_map)[0x35FD3501] = "m_fMaxTargetDistance";
        (*property_map)[0x12364C28] = "m_VolumeShapes";
        (*property_map)[0x1C630B12] = "w";
        (*property_map)[0x6400C289] = "m_ridBloodPoolDecalController";
        (*property_map)[0x4973F817] = "m_WeaponType";
        (*property_map)[0x2B470F94] = "m_bIsInvincible";
        (*property_map)[0xBFD67133] = "m_vLocalSize";
        (*property_map)[0x6322EBAD] = "m_fCurveDurationNeck1";
        (*property_map)[0x83B1C948] = "m_JumpPointAmbience";
        (*property_map)[0x67FA1DB3] = "m_vScaleManual";
        (*property_map)[0x4953529] = "m_fRangeModifierFootsteps";
        (*property_map)[0x4A94F2D3] = "KINEMATIC_COLLIDABLES_ONLY_TRANSPARENT";
        (*property_map)[0xECA1B81A] = "m_aExitPoints";
        (*property_map)[0x3BAE3834] = "m_aActivitySpheres";
        (*property_map)[0xB7F48FAB] = "m_vWorldOffset";
        (*property_map)[0xCA3885CF] = "XAxis";
        (*property_map)[0x762953F0] = "m_TargetControl";
        (*property_map)[0xAE01BC75] = "m_eInteraction";
        (*property_map)[0x48948A98] = "_i";
        (*property_map)[0xA6313ABF] = "m_fTimeMultiplier";
        (*property_map)[0x54A19CCF] = "m_fPickBufferDuration";
        (*property_map)[0xE3D05689] = "m_pInvestigateGroup";
        (*property_map)[0x5E8C682C] = "m_NrGuardsToSend";
        (*property_map)[0x580BDE77] = "m_fOcclusionRadius";
        (*property_map)[0xC1347A77] = "m_fCharacterWetnessDiffuseMul";
        (*property_map)[0x1B65D2BC] = "m_pTargetSpatial";
        (*property_map)[0xBF5D9476] = "m_PathfinderDistanceThreshold";
        (*property_map)[0x51A22635] = "m_eOrder";
        (*property_map)[0x3BF18062] = "m_pControlPointGizmo";
        (*property_map)[0x94EA82EA] = "m_rReveal";
        (*property_map)[0x122EB8CA] = "m_nSplatDecalNumVariationsY2";
        (*property_map)[0xE0156550] = "m_warningCount";
        (*property_map)[0x7186756] = "m_eState";
        (*property_map)[0x6A8FA78D] = "m_bDebugBones";
        (*property_map)[0x28589E73] = "m_fPeripheralAngleWhileMoving";
        (*property_map)[0x39460B45] = "m_bBlockMelee";
        (*property_map)[0xC2557EEC] = "m_fAreaQuadViewFactorThreshold";
        (*property_map)[0xC2E64C6D] = "m_bWasIntroCutFired";
        (*property_map)[0xA007A2C4] = "m_nIconId";
        (*property_map)[0x2CEFD471] = "m_sLastEventToken";
        (*property_map)[0xFA5A236B] = "m_bCacheOnBack";
        (*property_map)[0x6661FC95] = "m_eRequiredVoiceVariation";
        (*property_map)[0x75BC6BA] = "m_vGlobalLightDirection";
        (*property_map)[0xB2066555] = "m_rEmitterProxy";
        (*property_map)[0x29F40041] = "m_fProgress";
        (*property_map)[0xE91251F8] = "m_rDoorOpen";
        (*property_map)[0x95E95BFB] = "m_ridBulletImpactShotInv";
        (*property_map)[0xB21F8B70] = "m_bCanFlee";
        (*property_map)[0x3F704079] = "m_pCrouchReaction";
        (*property_map)[0x17676886] = "m_fDistance";
        (*property_map)[0x66095BA1] = "m_bEnableDistanceCulling";
        (*property_map)[0xCD44E64A] = "m_bTargetInRange";
        (*property_map)[0x8A31280C] = "m_fCrosshairOffset";
        (*property_map)[0xB0AA06E3] = "m_bNodeFilterLight";
        (*property_map)[0xCE2D52EE] = "m_bWaitUntilDangerEnds";
        (*property_map)[0xBE24DCF1] = "m_ridBulletImpactReflection";
        (*property_map)[0x69EDB17A] = "m_Layer";
        (*property_map)[0x665CDF80] = "m_OptimalSpawnDistance";
        (*property_map)[0x87BA8331] = "m_vGlowEnemiesVignetteRadii";
        (*property_map)[0x14B3C51F] = "m_bShowWarning";
        (*property_map)[0x92F8E6C7] = "m_fVisionMinDistance";
        (*property_map)[0xA20ABB3A] = "subEntities";
        (*property_map)[0xBA348380] = "m_sIconMenuId";
        (*property_map)[0x43FCD684] = "m_bNoActionAvailable";
        (*property_map)[0x1D4748D7] = "m_fRotationSpeedMin";
        (*property_map)[0xBA8A3ED5] = "m_vLeftFadeAngles";
        (*property_map)[0x78AC5776] = "nAmmoTotal";
        (*property_map)[0x2AE7B2C0] = "m_rContextKeywords";
        (*property_map)[0xA8414957] = "m_bIsMoving";
        (*property_map)[0xD5910645] = "m_rActors";
        (*property_map)[0x7FA304C3] = "m_sLabel";
        (*property_map)[0x8EB0A031] = "m_MirrorObjects";
        (*property_map)[0x1BC920DF] = "m_pRoomReferencePosOverride";
        (*property_map)[0x3858D58] = "m_bIgnoreSillyHitman";
        (*property_map)[0x174D49E8] = "m_sDescription";
        (*property_map)[0xE0E78430] = "m_bOutfitRelevant";
        (*property_map)[0x797FA604] = "m_bRightHandRotationType";
        (*property_map)[0x419FBDE5] = "m_BoneWeightManagerEntity";
        (*property_map)[0x22065298] = "m_nOnlyOnZoomLevel";
        (*property_map)[0xB5F0A021] = "objectiveType";
        (*property_map)[0xDAB28393] = "m_sGlyph";
        (*property_map)[0xEB2DDA4D] = "m_PosLeftHandAttach";
        (*property_map)[0xADCD9532] = "m_fMaxDistanceToIntermediatesDuringEscort";
        (*property_map)[0x863DF014] = "m_ridBulletImpactShot";
        (*property_map)[0x19F811F3] = "m_fBulletDecalSizeVariation";
        (*property_map)[0xD782BE0E] = "m_fYAxisAngle";
        (*property_map)[0xAF431E13] = "targets";
        (*property_map)[0x72B9551F] = "m_bInterpolate";
        (*property_map)[0x3B6F6586] = "m_rSignalOutputPinsCondition";
        (*property_map)[0xDE445E87] = "m_sAnimName";
        (*property_map)[0xF354D7AB] = "m_bNeverFlee";
        (*property_map)[0x30DD0EB] = "m_rDrainPipe";
        (*property_map)[0x1310F274] = "m_aClearActions";
        (*property_map)[0x4B95010C] = "m_nCount";
        (*property_map)[0x9FC0BCBE] = "m_nRTVal";
        (*property_map)[0x5F7B551E] = "m_rBruise";
        (*property_map)[0x6DD04BA9] = "m_fOpacity";
        (*property_map)[0xECCE2132] = "m_fShadowFadeLength";
        (*property_map)[0x2F02DBE1] = "m_bTray";
        (*property_map)[0xFA4ADA79] = "m_targetClips";
        (*property_map)[0x81C58F28] = "m_animationResourceID";
        (*property_map)[0x20B183A3] = "m_fVolume";
        (*property_map)[0xB9BE879] = "m_bIllegalItem";
        (*property_map)[0x1BC4AAA4] = "m_bHDREnabled";
        (*property_map)[0x8FBF1207] = "m_bInRange";
        (*property_map)[0xFF5E5838] = "m_rNameTextResource";
        (*property_map)[0x54186631] = "m_fSplatDecalSizeVariation";
        (*property_map)[0xCAD4A1DE] = "m_pPeekCameraSurface";
        (*property_map)[0xDD0EE5D9] = "m_fHandlerAudioPlayPosition";
        (*property_map)[0x8E7025EE] = "m_bIsHitmanSuit";
        (*property_map)[0x6EFB1F43] = "m_eAlignmentType";
        (*property_map)[0xF4354218] = "m_bIsPreparing";
        (*property_map)[0x2854D6A5] = "m_bDropTempHolsterableItems";
        (*property_map)[0x772B470C] = "bSniperShot";
        (*property_map)[0x31CD7F54] = "m_bLabelRelevant";
        (*property_map)[0xE531926F] = "m_aDisabledAIBehaviorFiringActors";
        (*property_map)[0x8D43F7AA] = "m_vAreaQuadSize";
        (*property_map)[0x7B0E2D8B] = "m_eItemTypeLeft";
        (*property_map)[0xD140D655] = "m_ConsiderTemporaryImmunity";
        (*property_map)[0x5AD48F97] = "m_eKeywordBehavior";
        (*property_map)[0x36A6276D] = "m_bSetValueAlwaysTriggerChangedEvent";
        (*property_map)[0xE8D8510] = "m_fRangeFar";
        (*property_map)[0x5D42DD16] = "m_fFarDistance";
        (*property_map)[0xF66F662B] = "m_nFontSize";
        (*property_map)[0x7B3756A6] = "m_ridBulletDecalController";
        (*property_map)[0x81D8E614] = "m_bHairTriggerKillExpected";
        (*property_map)[0xA5554B8E] = "m_bAlwaysPickupApproachPath";
        (*property_map)[0x1C843EDB] = "m_nDensity1Radius";
        (*property_map)[0xC062FA85] = "m_ridBulletDecalMaterial";
        (*property_map)[0x57AB27F6] = "m_pCivilianObserversGroup";
        (*property_map)[0xBFF37DE4] = "isIllegal";
        (*property_map)[0x4C9820DC] = "m_HeroConfig";
        (*property_map)[0xAF28D3E4] = "m_aInclusions";
        (*property_map)[0x5940064D] = "m_inputPinConditions";
        (*property_map)[0xE70D6DF3] = "m_bCarryWeapon";
        (*property_map)[0x30C8A9AF] = "sHelpText";
        (*property_map)[0x7761EB75] = "m_fAgilityAttractionJumpPenalty";
        (*property_map)[0x2BCDA8EC] = "m_BodySensorData";
        (*property_map)[0x5247E085] = "m_vSplatDecalSize2";
        (*property_map)[0x71FD17E3] = "m_fFadeInSpeed";
        (*property_map)[0x5BEDFCF0] = "m_bPrecisePositioning";
        (*property_map)[0x93C69837] = "m_bStartVisible";
        (*property_map)[0x12340E90] = "m_fNoiseScale";
        (*property_map)[0x43ADE2F6] = "m_bOverrideTetherDistances";
        (*property_map)[0xA684AC46] = "m_PrimitiveScale";
        (*property_map)[0xED39273B] = "m_nBulletDecalNumVariationsX";
        (*property_map)[0x42231407] = "m_fWeight3";
        (*property_map)[0x272A05D6] = "m_nRows";
        (*property_map)[0x506F2F70] = "m_vBulletDecalSize";
        (*property_map)[0x5E3485C3] = "m_aVolumes";
        (*property_map)[0xF86870F2] = "m_bCanPistolWhip";
        (*property_map)[0x4614CEFE] = "m_rPromptDescriptionTextResource";
        (*property_map)[0xCABEBA4C] = "m_FadeOutCurve";
        (*property_map)[0x4A80F9C] = "m_nLODOffsetGeom";
        (*property_map)[0x4401FE96] = "m_rRTESimpleRef";
        (*property_map)[0xE33A30DE] = "m_bCrouched";
        (*property_map)[0xBEF1A836] = "m_fPreferredDistance";
        (*property_map)[0x56A85A11] = "m_bStartedReportDialog";
        (*property_map)[0x99D49FCB] = "m_bIsMale";
        (*property_map)[0x936E667C] = "m_Data";
        (*property_map)[0xA85196DA] = "m_ridSplatDecalController";
        (*property_map)[0x686193C9] = "m_AIEnvSaveData";
        (*property_map)[0x413F29AE] = "m_key";
        (*property_map)[0xC84984CC] = "m_bPacifyExpected";
        (*property_map)[0xE6B41BEA] = "rCharacter";
        (*property_map)[0x82D6E041] = "m_ridSecondarySplatDecalController";
        (*property_map)[0x247DBFFF] = "m_ridSplatDecalMaterial";
        (*property_map)[0x41A638B6] = "m_fStrangleTime";
        (*property_map)[0x15BC3078] = "m_bEnableMotors";
        (*property_map)[0xBF7B57C8] = "entitySubsets";
        (*property_map)[0xCF9A5C28] = "m_actorTemplate3";
        (*property_map)[0x58A9DB83] = "m_pPlayingSound";
        (*property_map)[0x30BCFD69] = "m_fProbability";
        (*property_map)[0xD801790] = "m_vEnvCollisionsBoxSizePalm";
        (*property_map)[0x4AAA5E8C] = "m_sEvent";
        (*property_map)[0x120C7DF7] = "m_eAimingRequirements";
        (*property_map)[0x2068C3A7] = "m_vSplatDecalSize";
        (*property_map)[0xE13CDADB] = "m_nSplatDecalNumVariationsX";
        (*property_map)[0xAAF91A9F] = "m_PerceptibleCrowd_Service_SaveData";
        (*property_map)[0xE47AFB35] = "m_pSplashHintDataProvider";
        (*property_map)[0xF888871F] = "m_bDeadByUnnoticed";
        (*property_map)[0x2DA3454D] = "m_nRandomSelectionSeed";
        (*property_map)[0x963BEA4D] = "m_nSplatDecalNumVariationsY";
        (*property_map)[0x9FDA95E2] = "m_bOnlyOnSniperZoom";
        (*property_map)[0xBA30AA46] = "m_rEnableCondition";
        (*property_map)[0xE4D2B078] = "m_rPromptTextResource";
        (*property_map)[0x63B01C85] = "enumField";
        (*property_map)[0x53C83425] = "m_fEnergyMax";
        (*property_map)[0xAF5B18FE] = "m_fCameraFadeSizeFactor";
        (*property_map)[0x4FF4F23F] = "dummy";
        (*property_map)[0xB35898B] = "m_nSplatDecalNumVariationsX2";
        (*property_map)[0x718D9C6] = "nNeighbor2";
        (*property_map)[0x9A600F5A] = "m_fMaxWarnDelay";
        (*property_map)[0x4B5FA651] = "m_fSplatDecalSizeVariation2";
        (*property_map)[0x75E64E4A] = "m_bGrabOnlyNeededActors";
        (*property_map)[0xF787C5C] = "m_fDecalProjectionPositiveDistance";
        (*property_map)[0xA4DAB4CF] = "m_fGC_Distance";
        (*property_map)[0x6BC7AD80] = "m_TrespassingPeripheralAttentionDuration";
        (*property_map)[0xB65D17A] = "m_fGateDistanceThreshold";
        (*property_map)[0xEF646D9F] = "m_state";
        (*property_map)[0xDB863CBE] = "m_fHMLastTrespassingAttentionMax";
        (*property_map)[0xDA84A168] = "m_LockdownManagerData";
        (*property_map)[0xE76ACE4C] = "m_blend";
        (*property_map)[0xC6A112AC] = "m_sGeotag";
        (*property_map)[0x49366148] = "m_fHDRBrightPassMaxPercentage";
        (*property_map)[0xCD2D7840] = "m_pCausticsMap";
        (*property_map)[0x9BC500E2] = "m_fWireLength";
        (*property_map)[0x7CF67E86] = "m_ridBulletImpact";
        (*property_map)[0x9DC24D81] = "FlagScaleZ";
        (*property_map)[0xC05E1ED3] = "m_nOutfitCharset";
        (*property_map)[0x963DB1B7] = "m_fTransRestitution";
        (*property_map)[0xE4FFF410] = "m_fEvaluationTime";
        (*property_map)[0x36E636F1] = "m_aSelections";
        (*property_map)[0xDE11F34D] = "m_eGlobalFilter";
        (*property_map)[0xEAF1EFB5] = "m_fPenetratableBulletEnergyThreshold";
        (*property_map)[0xE33E7317] = "m_rActEntity";
        (*property_map)[0x5B54023A] = "m_bTurnedOn";
        (*property_map)[0xF8911622] = "m_rBoneAnimator";
        (*property_map)[0xDCB496C7] = "m_rMapTracker";
        (*property_map)[0xB95A2AE6] = "m_bAmmoImpactEffectEnabled";
        (*property_map)[0x3DDCB33C] = "m_bUseSkinningPosNormalConstrains";
        (*property_map)[0x48E3DE8F] = "m_bSequenceAllowCamera";
        (*property_map)[0x951EC7BE] = "m_aAmmoIds";
        (*property_map)[0xDAE9C354] = "m_bAlwaysPenetrable";
        (*property_map)[0xD82F1485] = "m_nExtraMagazineBullets";
        (*property_map)[0x4B4B5C2C] = "m_fInvestigateCautiousCivilianPropagationLOSMinDistance";
        (*property_map)[0xBFD30652] = "m_rSwitchWeaponFamily";
        (*property_map)[0x521FC46F] = "m_aSavableObjectsData";
        (*property_map)[0x281B79AE] = "m_aeStates";
        (*property_map)[0x1182D070] = "m_eScaleMode";
        (*property_map)[0x42D4EEB9] = "m_nVersion";
        (*property_map)[0x9D06DC5] = "m_fCullingAngle";
        (*property_map)[0xD605DAB4] = "m_sPressedEventName";
        (*property_map)[0x54ED1574] = "m_fInitialSettleTime";
        (*property_map)[0x674A2E4F] = "m_ridClipTemplate";
        (*property_map)[0x2BED040A] = "m_fCurveDurationNeck";
        (*property_map)[0xB905255] = "m_nCurVariationIndex";
        (*property_map)[0xF51DD0D4] = "m_fMaxValue";
        (*property_map)[0xB9CABFDE] = "m_aCharSets";
        (*property_map)[0x7701013D] = "m_bIgnoreDeadBodies";
        (*property_map)[0xAD0DF236] = "m_pPhysicsSystemSecondary";
        (*property_map)[0xFB453214] = "m_bDebugThis";
        (*property_map)[0xB4BF1D5A] = "m_nLengthTessellation";
        (*property_map)[0xEB148DBC] = "m_aOpportunities";
        (*property_map)[0x25D557D1] = "m_sPressedEventData";
        (*property_map)[0x3772ED16] = "m_pMaterialOverrideResource3";
        (*property_map)[0x95E68346] = "m_bUseInFallbacks";
        (*property_map)[0x7B442B46] = "m_eStatePrevious";
        (*property_map)[0x16DB4DFC] = "m_bForwardFocusToFirstChild";
        (*property_map)[0x65C4A321] = "m_mPositionInCover";
        (*property_map)[0x66F001F6] = "m_bYawBlendOutRadius";
        (*property_map)[0x74B7EBDD] = "m_bBubblePressAction";
        (*property_map)[0x475C6D3D] = "m_fMaxPitchAngle";
        (*property_map)[0xA9D7EB1] = "m_nNumRows";
        (*property_map)[0x11270A36] = "m_bEnableDynamicEnforcers";
        (*property_map)[0xE19F5A0E] = "m_aEventReceivers";
        (*property_map)[0x2AB1153C] = "m_ChestAvoidanceDistance";
        (*property_map)[0x51556EF8] = "m_localValueArray";
        (*property_map)[0xC61555E] = "m_nDepthDimension";
        (*property_map)[0x378417D8] = "m_fRemaining";
        (*property_map)[0x52124A7E] = "m_fCombatNPCSeperationRadius";
        (*property_map)[0x28C55902] = "vPos";
        (*property_map)[0x5ED21319] = "m_bBlockVisionAlerted";
        (*property_map)[0x90914FC8] = "m_fConeAngle";
        (*property_map)[0xE8923402] = "m_fFill";
        (*property_map)[0xC520F221] = "m_aPerceptibleActors";
        (*property_map)[0xDE7F60ED] = "m_Receivers";
        (*property_map)[0xD9F8E16C] = "m_bA";
        (*property_map)[0x7DEB831] = "m_rEventChannel";
        (*property_map)[0xBB93EEE8] = "m_TimeLeft";
        (*property_map)[0xAFE5C01C] = "m_rBodyHeadlineResource";
        (*property_map)[0x5FBA840F] = "m_nBodypartLODCutOff";
        (*property_map)[0x7DC3FE5A] = "m_bIsSittingChair";
        (*property_map)[0x6F735AC8] = "m_bGlobalAttractorsEnabled";
        (*property_map)[0x2709F424] = "m_ScopeCrossHair";
        (*property_map)[0x19F11E2C] = "m_GlowEnforcerColor";
        (*property_map)[0x4C3A0AC0] = "m_GlowContractTargetUnconsciousColor";
        (*property_map)[0xD4138AA5] = "m_BlueOfGreen";
        (*property_map)[0xFA9E93B1] = "m_bEnabled";
        (*property_map)[0xE7AB0DA9] = "m_bUseRoomOcclusion";
        (*property_map)[0x4B517EF6] = "m_fInitialNormalizedPitch";
        (*property_map)[0xAC5D1C25] = "m_NumAxeSlitThroatEmitters";
        (*property_map)[0x85803CFF] = "m_bShowItem";
        (*property_map)[0xBAAE0ED2] = "m_aControlPoints";
        (*property_map)[0x37636442] = "m_aAddedActors";
        (*property_map)[0x79AA4394] = "m_bNegativeZ";
        (*property_map)[0xA757872B] = "m_FixedAlpha";
        (*property_map)[0xADE4B1C0] = "m_nCurrentLoop";
        (*property_map)[0x64F57E43] = "mQuaterion";
        (*property_map)[0x567B8203] = "AddAgentData_BehaviorsDisabled_Pool_SaveData";
        (*property_map)[0x651CEEE5] = "m_eSensitivity";
        (*property_map)[0x4E2D5520] = "m_fS";
        (*property_map)[0x2F950A92] = "m_bHitmanCanConceal";
        (*property_map)[0x7E0FFFBC] = "m_bPosPinOnTransformChange";
        (*property_map)[0x8BCFA965] = "m_fDecalProjectionNegativeDistance";
        (*property_map)[0xE80A84DE] = "m_vLinkedQuaternionRotation";
        (*property_map)[0xBF043D86] = "m_nCountFrom";
        (*property_map)[0xB5E622D6] = "m_aPoseCollections";
        (*property_map)[0x8FF5E065] = "m_bBlendOutCloth";
        (*property_map)[0xF1862FF5] = "m_EntityListA";
        (*property_map)[0x5AC5280E] = "sInputPlatform";
        (*property_map)[0x99B1C269] = "m_nCountTo";
        (*property_map)[0xF35F9519] = "m_fLeftHandTargetWeight";
        (*property_map)[0xEAC3E029] = "m_bAllowBaseMovements";
        (*property_map)[0xF5157AEC] = "m_BoxesExcludingArea";
        (*property_map)[0x9C48DF69] = "m_assistant";
        (*property_map)[0x96E819CD] = "m_fGateAngleThreshold";
        (*property_map)[0x38AAC753] = "m_nStepSize";
        (*property_map)[0x43085A8E] = "m_bStepAtFirst";
        (*property_map)[0xBF8A0B67] = "m_bWarnOnWeaponFound";
        (*property_map)[0xD6E1C10D] = "m_fCollisionOffset";
        (*property_map)[0x257C86B8] = "m_aConnections";
        (*property_map)[0xC20C8665] = "m_bIncrement";
        (*property_map)[0xF707D108] = "m_bIsStandingStill";
        (*property_map)[0x6EF5861C] = "m_bKilledByPropRelevant";
        (*property_map)[0x11A9E2EB] = "m_bPeekSequenceRelativeToHorisontalMovement";
        (*property_map)[0xBC4B816B] = "m_nMultiTrackLength";
        (*property_map)[0xBD9349AC] = "m_bX";
        (*property_map)[0xB230385D] = "m_eFlowType";
        (*property_map)[0xAFF8E60C] = "m_fAcceleration";
        (*property_map)[0x88088104] = "m_bStepAtLast";
        (*property_map)[0xEBF41CAE] = "m_guardPoint";
        (*property_map)[0x35C940D1] = "m_aTrackSaveData";
        (*property_map)[0x32595C00] = "m_sendSignalsInEditMode";
        (*property_map)[0xF885D511] = "m_bOneShotEmitter";
        (*property_map)[0x241D0931] = "m_aPreviewActors";
        (*property_map)[0x2D20E628] = "m_bInfiniteSteps";
        (*property_map)[0xC087CB84] = "m_bGrenade";
        (*property_map)[0x1916B31A] = "m_bPausedOnStart";
        (*property_map)[0xD6211790] = "m_sValueFormat";
        (*property_map)[0xC971721A] = "m_eBodyMode";
        (*property_map)[0x13FF6222] = "m_nStartupIndex";
        (*property_map)[0xCF853678] = "m_bInfiniteLoop";
        (*property_map)[0x7B278F2] = "m_nLoopAmounts";
        (*property_map)[0x523D2482] = "m_bGenerateSoundOnCollision";
        (*property_map)[0x2A5B246A] = "BlobRid";
        (*property_map)[0x8FB5E7E] = "m_bShowOnTop";
        (*property_map)[0xEA101D50] = "m_bDangerous";
        (*property_map)[0x5DBEC365] = "m_nHMVisibleWeaponReactionDelay";
        (*property_map)[0xCB05D18A] = "m_bPausedUpdate";
        (*property_map)[0x2BBF8550] = "logicalParent";
        (*property_map)[0x8B37A3AE] = "m_fWireWidth";
        (*property_map)[0xC5181A88] = "m_aEntityPaths";
        (*property_map)[0x5D82E4F4] = "m_nGameTimeDelta";
        (*property_map)[0xAAF937A3] = "m_fHuntedIgnoreDistanceStand";
        (*property_map)[0x31634C02] = "precise";
        (*property_map)[0xA04B255A] = "m_nStableID";
        (*property_map)[0x6823C9BE] = "m_aLayerOverrides";
        (*property_map)[0xD18E3E91] = "m_rRequirements";
        (*property_map)[0xB43C2816] = "m_fLFHorizontalInt";
        (*property_map)[0xE860549A] = "m_SkipFromEnd";
        (*property_map)[0x7E3B11CB] = "m_fCLPDistanceMultiplier";
        (*property_map)[0x91D4B002] = "m_aBodies";
        (*property_map)[0x9D4E107C] = "m_pEntityB";
        (*property_map)[0x70EBBFF7] = "m_ConditionsOnGet";
        (*property_map)[0x64B6B5DC] = "m_fStrengthMultiplier";
        (*property_map)[0x508848B5] = "m_bSleeping";
        (*property_map)[0xDAAFCDD7] = "m_Curve";
        (*property_map)[0x9152085C] = "m_bDoneTriggered";
        (*property_map)[0xA4A47968] = "m_fEndValue";
        (*property_map)[0xDF0E6FFD] = "m_Grey";
        (*property_map)[0x3AA1E5CB] = "m_bSpatialVisibility";
        (*property_map)[0x4439DFC3] = "m_fCLPVerifyDistance";
        (*property_map)[0x552F9E01] = "fHitsNumberTimeout";
        (*property_map)[0x25FA92C4] = "m_ridCurveEntity";
        (*property_map)[0x9CAE5BF4] = "m_nSelectedSource";
        (*property_map)[0x40E8D037] = "m_fHMWeaponAttentionChange";
        (*property_map)[0x8647FA0C] = "m_aVREventConfigs";
        (*property_map)[0x3312BEF2] = "m_fZAxisSpeed";
        (*property_map)[0x6819AEBC] = "m_vVelocityIn";
        (*property_map)[0x78037EC6] = "m_ResetEventValues";
        (*property_map)[0x573403DB] = "m_sTitleText";
        (*property_map)[0x8A4E31CC] = "m_bPhoneMode";
        (*property_map)[0x806338F8] = "m_pAudioEmitter";
        (*property_map)[0x25621832] = "m_nTimeRemoveLimit";
        (*property_map)[0xD918599B] = "m_eExpectedWeaponClass";
        (*property_map)[0x2B856B7] = "m_Curve2";
        (*property_map)[0xC9A64EB4] = "m_bDeadByAccident";
        (*property_map)[0xBD69245D] = "m_bUseRealTime";
        (*property_map)[0xBC78E4E4] = "m_DebugShowDistanceGrid";
        (*property_map)[0x48D9B0BD] = "m_iPoints";
        (*property_map)[0x8B66AB8B] = "m_fDynamicFriction";
        (*property_map)[0xE2DF4AC2] = "m_VisitedLocations";
        (*property_map)[0xB8811708] = "m_SpawnPoints";
        (*property_map)[0x151FB2F] = "m_nBooleanEventsHandled";
        (*property_map)[0xB2C70CC5] = "m_fInitialBulletsModifier";
        (*property_map)[0xDED8C703] = "m_eAnimSetEmotionState";
        (*property_map)[0x5932867B] = "m_vItemVelocity";
        (*property_map)[0xFF576FC4] = "m_EnteringConditions";
        (*property_map)[0x2740ABC3] = "m_fMinDistance";
        (*property_map)[0x8C327DD6] = "m_To";
        (*property_map)[0x2B611B0] = "m_TargetLocation";
        (*property_map)[0x6A87F529] = "m_bThreatAssessmentIncludePatrolPoints";
        (*property_map)[0x1144B1F3] = "m_ragdollBoneName";
        (*property_map)[0xB7EA4F8C] = "m_fExplosionPushOrigin";
        (*property_map)[0xD0849412] = "m_bActivating";
        (*property_map)[0xB73AE623] = "m_fCurveStartTimeNeck";
        (*property_map)[0x37D045] = "m_VisitedLocationsInfluenceDistance";
        (*property_map)[0x7A73C1E9] = "m_cPrimeToStretchCurve";
        (*property_map)[0x7E907DDA] = "nGridWidth";
        (*property_map)[0x3F78FB2B] = "m_aEntities";
        (*property_map)[0x839140A] = "m_bRandomRotation";
        (*property_map)[0x55F8233D] = "m_vTiling";
        (*property_map)[0x4B4B9102] = "m_eMotionTwist";
        (*property_map)[0xD508430] = "nLayerIndex";
        (*property_map)[0x40F1B0D6] = "m_bB";
        (*property_map)[0xAB32FC79] = "ContractRid";
        (*property_map)[0xB784D9E0] = "m_fHazardZoneTime";
        (*property_map)[0x20295C5] = "m_aData";
        (*property_map)[0x807DD271] = "m_bEscortsReactToDistactions";
        (*property_map)[0x726ADFC3] = "postInit";
        (*property_map)[0xB5073862] = "m_vForward";
        (*property_map)[0x54FE7252] = "fMaxDistance";
        (*property_map)[0x6139B66E] = "m_fWaterHeight";
        (*property_map)[0x77672FC9] = "m_HolsterAbility";
        (*property_map)[0xF911DEF6] = "m_Stage1Duration";
        (*property_map)[0xF4C0CC32] = "m_eOverrideSensorState";
        (*property_map)[0x3CBCD9A1] = "m_eProjectionType";
        (*property_map)[0xDE621A84] = "m_bIsRunning";
        (*property_map)[0xAF732000] = "m_fSpringMassSmall";
        (*property_map)[0x8DCA6097] = "m_nLifetime";
        (*property_map)[0xB05223B9] = "m_rContainerItem";
        (*property_map)[0x5DE5848C] = "m_fTimeToLiveSecondsMin";
        (*property_map)[0xF01132A5] = "m_fAutoFadeMaxTime";
        (*property_map)[0x1B382635] = "m_StateData";
        (*property_map)[0xA41EFDAB] = "m_pHUDIcon";
        (*property_map)[0x19E2F86] = "m_bVRVisibilityBehavior";
        (*property_map)[0xA57177E1] = "m_fForceDuration";
        (*property_map)[0xBD0CD62B] = "m_bFrictionSquare";
        (*property_map)[0x8DF981CB] = "m_bNodeFilterLinked";
        (*property_map)[0x9D166D48] = "m_rValueEntity";
        (*property_map)[0x4FB708BC] = "m_aCameras";
        (*property_map)[0x1F1A7459] = "m_bSmoothenExitFlows";
        (*property_map)[0x13F7BC98] = "mainslotsSlim";
        (*property_map)[0x3E3E2D69] = "m_fTessellationOffset";
        (*property_map)[0xA2CEC971] = "m_vTorqueImpulse";
        (*property_map)[0xFF77A1EF] = "m_ParticleColorRangeStart";
        (*property_map)[0xD460178] = "m_bDisabledFromStart";
        (*property_map)[0xE1B4D5E6] = "m_fMaxDistanceWorldSpace";
        (*property_map)[0xB8011F26] = "m_nPOIHeightOffset1";
        (*property_map)[0x35242491] = "m_fWeight2";
        (*property_map)[0xC9A0A9CE] = "otherslotsCount";
        (*property_map)[0xA338A1CC] = "m_rSoundEvent_CloseCombat";
        (*property_map)[0x2DA23D90] = "m_bHeroCanDumpBody";
        (*property_map)[0x1BA185D5] = "m_SpatialPosOnLedge";
        (*property_map)[0x2751464] = "m_bHitmanSuspectedInCurrentOutfit";
        (*property_map)[0x9076F4D9] = "m_fDiffusePower";
        (*property_map)[0xCC631709] = "m_rDescriptionTextResource";
        (*property_map)[0xC9497956] = "m_eExclusionLayerState";
        (*property_map)[0x400E2C39] = "m_bSelectVisibleOnly";
        (*property_map)[0x34AD19E1] = "m_bIsCheckedWhenFrisked";
        (*property_map)[0xA29F6604] = "m_bStaticCollisions";
        (*property_map)[0xF91C09AF] = "objectiveId";
        (*property_map)[0x781B2D1B] = "m_eMotionTransY";
        (*property_map)[0xB41A506D] = "m_nDramaType";
        (*property_map)[0x61E81E7A] = "m_rBundlePosition";
        (*property_map)[0xD8FE187D] = "m_bCollideHitman";
        (*property_map)[0xB8CFE56F] = "isActionInventory";
        (*property_map)[0x5454093C] = "m_bUseTrackSequence";
        (*property_map)[0xC353FF68] = "m_bSignalOnSelf";
        (*property_map)[0x5C51BB24] = "m_rActorInstance";
        (*property_map)[0x1F8DD593] = "m_bOutside";
        (*property_map)[0x759E6186] = "m_bIgnoreMultipleNPCShots";
        (*property_map)[0x90919BD] = "m_pCondition";
        (*property_map)[0x4786AB66] = "m_nTicks";
        (*property_map)[0x922D1EF] = "m_bCrowd";
        (*property_map)[0xE32FC5D] = "m_groupAge";
        (*property_map)[0xA9EBBFD4] = "m_nInterval";
        (*property_map)[0x270BF94] = "mTrajectoryTranslation";
        (*property_map)[0x6BAE8BCD] = "m_bResetWhenStopped";
        (*property_map)[0x82DD3759] = "m_bIgnoreOcclusionSetup";
        (*property_map)[0xFF2DE9B] = "m_nMaxNotifications";
        (*property_map)[0x3D32CB82] = "m_bGC_DebugHasItemState";
        (*property_map)[0x78F7A780] = "m_actorResourceID";
        (*property_map)[0x6F78981B] = "m_aActivityPullSpheres";
        (*property_map)[0x226D7DEA] = "m_bResetOnLoad";
        (*property_map)[0x89D24F94] = "m_rItemUsed";
        (*property_map)[0x5AFAC781] = "m_references";
        (*property_map)[0x3E0D52A] = "m21";
        (*property_map)[0x991B6DB3] = "m_fSettingDuration";
        (*property_map)[0xC72D91B3] = "m_bSynchronous";
        (*property_map)[0xDA01C2C1] = "m_GizmoGeomRID";
        (*property_map)[0x5ECBA704] = "m_bStartActive";
        (*property_map)[0x58906C58] = "m_bItems";
        (*property_map)[0x7E33DB08] = "m_fParallelFalloff";
        (*property_map)[0x8982B62B] = "m_fBlend";
        (*property_map)[0x9C3A1852] = "m_bEnableSmoothing";
        (*property_map)[0x97E01F14] = "m_tEvacuationStart";
        (*property_map)[0xC54FFD2B] = "m_DefaultWeaponVariationResource";
        (*property_map)[0x49C36404] = "m_userInputWeight";
        (*property_map)[0x7E0EAB7D] = "m_bResult";
        (*property_map)[0x210CA423] = "m_fCurveStartTimeNeck1";
        (*property_map)[0xADB3EF29] = "m_fHeadDamageMultiplier";
        (*property_map)[0xFE05A8F] = "m_aCharacters";
        (*property_map)[0x52EE851B] = "m_bRepeatWhileTrue";
        (*property_map)[0xD5F4F865] = "m_pCollisionRollingEvent";
        (*property_map)[0xBE7191FA] = "m_behaviorModifiers";
        (*property_map)[0x951CCC0] = "m_pActMoveToTemplate";
        (*property_map)[0x356B6AD8] = "m_PostfilterParametersID";
        (*property_map)[0x2453572C] = "m_nRemaining";
        (*property_map)[0x945AD0DD] = "mTranslation";
        (*property_map)[0xE811444] = "fPacifyDamage";
        (*property_map)[0xBA831CF2] = "m_accuseUnconsciousOutfit";
        (*property_map)[0x9C8C37B2] = "m_RoomNode";
        (*property_map)[0xFE11D138] = "Name";
        (*property_map)[0xDCD60218] = "m_rLinkedEntityTarget";
        (*property_map)[0xEEDE9D03] = "m_fDoubleVisionBlendSpeed";
        (*property_map)[0xB6820602] = "m_bRequired";
        (*property_map)[0x5E68448A] = "m_fStep";
        (*property_map)[0x24DAF9E5] = "m_tStartStandDown";
        (*property_map)[0xFAF868A2] = "m_fWalkDistance";
        (*property_map)[0x74BB7CD3] = "m_fObstructionMaxDistOverride";
        (*property_map)[0x701FE950] = "nNeighbor3";
        (*property_map)[0x3C075803] = "m_CustomTrackData";
        (*property_map)[0x4EBFD58E] = "m_bRecoverItem_FreeBone";
        (*property_map)[0x34A03D47] = "m_nGrabBoneID";
        (*property_map)[0x41D520BA] = "m_bExpire";
        (*property_map)[0x83A78016] = "m_aActors";
        (*property_map)[0x80ED8D88] = "m_bContainsItem";
        (*property_map)[0x5235359E] = "m_eSituationState";
        (*property_map)[0x549355B] = "m_objectiveGuid";
        (*property_map)[0x14CD46D] = "m_fDefaultMessageTimeout";
        (*property_map)[0x7F9C41AC] = "m_fTransitionDuration";
        (*property_map)[0xEEA2B367] = "m_FadeThreshold";
        (*property_map)[0x2F1A793E] = "m_rUsableCondition";
        (*property_map)[0x51D84A4B] = "m_fArrestDistance";
        (*property_map)[0xA12F4AC8] = "m_bRememberInitialBehaviour";
        (*property_map)[0x8400A331] = "m_bCreateSkirtConstrains";
        (*property_map)[0x8E5A0583] = "m_sText";
        (*property_map)[0x8CA57966] = "m_rActorProvider";
        (*property_map)[0xAF79942] = "m_nLeaderApproachNode";
        (*property_map)[0x4E50C585] = "m_aParticleData";
        (*property_map)[0x11F21E43] = "m_eAgilityState";
        (*property_map)[0x3ADD08F8] = "HERO_VR";
        (*property_map)[0x4E5C4AEB] = "m_rPriorityModifier";
        (*property_map)[0x2F0866F4] = "m_fHuntedIgnoreDistanceRun";
        (*property_map)[0x635D31C8] = "m_nEventAction";
        (*property_map)[0x2BB18D90] = "m_rActBehavior";
        (*property_map)[0xBC2EDE77] = "m_rDefaultBehavior";
        (*property_map)[0x50FB1DC7] = "m_bCaptureAllActions";
        (*property_map)[0xDD5FD9B4] = "m_fDepthPosX";
        (*property_map)[0xC08C4427] = "m_name";
        (*property_map)[0xED8064E0] = "m_fAngleNeck1";
        (*property_map)[0x9BFF8283] = "m_fDiffuseClamp";
        (*property_map)[0xDC32D297] = "m_uiModSituation";
        (*property_map)[0xBADD4B8D] = "m_fParticleReflectScale";
        (*property_map)[0xFE2DD177] = "m_rActor";
        (*property_map)[0xAC6FD16B] = "m_coverPlane";
        (*property_map)[0x6B6F35C2] = "m_sFailedEventName";
        (*property_map)[0xB706CAF2] = "m_bDeepTrespassing";
        (*property_map)[0xE87FFA4A] = "m_aItemInstanceSaveData";
        (*property_map)[0x4E6F7512] = "m_bThrowBodyOverRail";
        (*property_map)[0xA06B97C6] = "m_rDesignOnlyRef2";
        (*property_map)[0x137D7F35] = "m_fMinValue";
        (*property_map)[0xA9723455] = "m_rRequiredKeywords";
        (*property_map)[0xF4C32681] = "durationMax";
        (*property_map)[0x202259BD] = "m_bIgnoreLockdown";
        (*property_map)[0x7242F517] = "m_eContinuity";
        (*property_map)[0x1165293D] = "m_nAudioHighPassFilter";
        (*property_map)[0x2BCB0628] = "m_rItemVisible";
        (*property_map)[0x48CB7F7] = "m_bKillPacified";
        (*property_map)[0x562E0AB4] = "m_bPlayInRealTimeAndDuringPause";
        (*property_map)[0x1336130C] = "conversationType";
        (*property_map)[0xD38D6F9F] = "m_fDistanceToTarget";
        (*property_map)[0x21092CF8] = "m_fPrecisionShotTimeScale";
        (*property_map)[0xC7EAB1BE] = "m_bAllowDeath";
        (*property_map)[0xA42EF3AD] = "m_rSentryZone";
        (*property_map)[0x93D16DAD] = "m_vStartPosition";
        (*property_map)[0x12E106BA] = "m_bMakeRedDot";
        (*property_map)[0xF712AC8C] = "m_Foot";
        (*property_map)[0xBDBA6981] = "m_pVariationResourceEntity";
        (*property_map)[0xCC37CDE3] = "m_nRestoreHealth";
        (*property_map)[0x62447BC8] = "m_RecenterFadeInTime";
        (*property_map)[0x24E1C155] = "m_bKillParticlesOnCollision";
        (*property_map)[0xAA2C9E54] = "m_aPositions";
        (*property_map)[0xE59FF187] = "m_bForegroundBlur";
        (*property_map)[0x7991073B] = "m_bTreatUnnoticableKillsAsAlive";
        (*property_map)[0x662B3C15] = "m_aValues";
        (*property_map)[0x3DA6ED12] = "m_fStopMoveDistance";
        (*property_map)[0xDDD59BD] = "m_defaultClip";
        (*property_map)[0x27253357] = "m_degpmAnchorMoveResetSpeedLinear";
        (*property_map)[0xDF5F0888] = "m_isValid";
        (*property_map)[0x118541EF] = "m_animationClips";
        (*property_map)[0xE75443C6] = "m_vLuminanceMinMax";
        (*property_map)[0xF0B27B9] = "m_aEvents";
        (*property_map)[0x50865424] = "m_rKeywords";
        (*property_map)[0x184D0265] = "m_bConversationHelper";
        (*property_map)[0x2318BC1F] = "m_fSlowDownStrafeNearFactor";
        (*property_map)[0x74E7849] = "m_maxSamplingBuffers";
        (*property_map)[0x7A4B9F8E] = "m_bNeedAlphaUpdate";
        (*property_map)[0x7490CF33] = "m_fHMWeaponAttention";
        (*property_map)[0xD36F7DCF] = "m_transitionClips";
        (*property_map)[0x72BE7398] = "m_fPointAimDurationMax";
        (*property_map)[0x8550C4] = "m_BRInputs";
        (*property_map)[0xA5DFA48D] = "m_partialAnimationClips";
        (*property_map)[0xFEDB70F8] = "m_eSituationType";
        (*property_map)[0xA362D4BE] = "m_CombatActs";
        (*property_map)[0xBD95E669] = "m_sConfigType";
        (*property_map)[0xD8CE14] = "m_fCurveStartFractionHead";
        (*property_map)[0xB66A6323] = "m_vPos";
        (*property_map)[0xA9F45A73] = "m_SlotDetails";
        (*property_map)[0x158C6051] = "m_rigResourceID";
        (*property_map)[0xF30D9F66] = "m_vUpVectorRangeStart";
        (*property_map)[0xFC0C66CD] = "m_bTurnTowards2";
        (*property_map)[0x3F17CE8B] = "m_eSpecialInputAction";
        (*property_map)[0xB15B506B] = "m_aeExitEventTypes";
        (*property_map)[0x1566313] = "m_nNumSimSegments";
        (*property_map)[0xE177D352] = "m_pEmitter";
        (*property_map)[0xB62E7538] = "m_eDialogRangeStart";
        (*property_map)[0x2727649D] = "Timestamp";
        (*property_map)[0xADEA47DF] = "m_bIsTargetExpected";
        (*property_map)[0x7CF1FCBE] = "m_AudioSetup";
        (*property_map)[0x12B86495] = "aAmmoTypes";
        (*property_map)[0xC32CA608] = "m_bWrapTileIndex";
        (*property_map)[0xA677C41E] = "m_pageKiloByteSize";
        (*property_map)[0x553BD148] = "m_bHorizontalSurfaceCollisionsOnly";
        (*property_map)[0x4F2CED9A] = "m_bTransparencyDisablesZBuffer";
        (*property_map)[0x4B120AE1] = "m_eDirection";
        (*property_map)[0x2DFF1888] = "m_rUpdateCondition";
        (*property_map)[0x4873E76E] = "m_fProjectedAreaCullingShadows";
        (*property_map)[0x5DB1705E] = "m_rCircleClamp";
        (*property_map)[0x491196AC] = "m_resourceId";
        (*property_map)[0xA8DF419C] = "m_maxLODForFractionalSampling";
        (*property_map)[0x13C3584A] = "m_pHeaderResource";
        (*property_map)[0x2387DDD9] = "m_bIsGameControlActive";
        (*property_map)[0x3E4217EC] = "m_uiModStatus";
        (*property_map)[0x4D767A32] = "m_rAction";
        (*property_map)[0xDD59B5F4] = "ACTOR_PROXY";
        (*property_map)[0x5A1512A0] = "assignTo";
        (*property_map)[0xA13010B2] = "age";
        (*property_map)[0x23175352] = "m_pTitleResource";
        (*property_map)[0x870AEB8B] = "m_tLockdownTime";
        (*property_map)[0x2DF6271D] = "m_nNumPacify";
        (*property_map)[0x227CC2B5] = "m_fAttentionGainRate";
        (*property_map)[0xD3619F34] = "m_bDisableDeadBodySensor";
        (*property_map)[0x644254DB] = "m_pTitleResourceXBoxOneOverride";
        (*property_map)[0x194DDA9] = "m_fSoundDampening";
        (*property_map)[0x3C7E24BF] = "m_pExpectedOutfit";
        (*property_map)[0xF0199B99] = "m_bBlockFootSteps";
        (*property_map)[0xF5E24FC9] = "m_nEvent";
        (*property_map)[0xF9AF9052] = "m_fArrayIndexMin";
        (*property_map)[0xE1396824] = "m_pTitleResourcePS4Override";
        (*property_map)[0xDAFB9887] = "m_aAllowedWeaponKeys";
        (*property_map)[0xFDFCCA8B] = "nRowSpan";
        (*property_map)[0x11577CC6] = "m_walkOnly";
        (*property_map)[0xDE0DAAF] = "m_sPropertyName";
        (*property_map)[0x891D57ED] = "blueprintIndexInResourceHeader";
        (*property_map)[0xB6726270] = "m_bName";
        (*property_map)[0xA10B78A6] = "m_rSearchVolume";
        (*property_map)[0xCE1F4BDB] = "m_nAnimationBlendMaxLOD";
        (*property_map)[0xDD6B3FBA] = "m_bCollisionGroup3";
        (*property_map)[0xB36AED9F] = "m_ActStartTime";
        (*property_map)[0x7B29AB1A] = "m_rEnablePickupOnSpawn";
        (*property_map)[0xD6AF974F] = "m_eType";
        (*property_map)[0xE1F954B9] = "m_rMovementConfigs";
        (*property_map)[0xD012EF3C] = "m_BlueOfRed";
        (*property_map)[0xF6FD0DBF] = "m_fContactForce";
        (*property_map)[0x48481BC] = "m_aSpatials";
        (*property_map)[0x26DE3D84] = "m_pTitleResourceKeyOverride";
        (*property_map)[0x9E6F02C4] = "m_rChildNetwork";
        (*property_map)[0xDFD12BDC] = "m_OrderData";
        (*property_map)[0xF76F989] = "m_rVictim";
        (*property_map)[0xDDC35AD3] = "m_pTitleResourcePCOverride";
        (*property_map)[0xCACF4646] = "m_pHelperGizmo";
        (*property_map)[0x67C3733A] = "m_fMinHealthThreshold";
        (*property_map)[0xCC938CAA] = "m_fConeAngleZ";
        (*property_map)[0xE144217D] = "m_fParticleSizeFactor";
        (*property_map)[0x659A8642] = "m_gamePostfilter";
        (*property_map)[0xD9A18E6F] = "m_blendGraphRoot";
        (*property_map)[0xBBC2C8AC] = "img";
        (*property_map)[0x3D22B5B4] = "m_ResourceID";
        (*property_map)[0xCD21C0F8] = "m_fScopeFOV_VR";
        (*property_map)[0x589FE277] = "m_vUpVectorRangeEnd";
        (*property_map)[0x62F82672] = "m_bFrisked";
        (*property_map)[0x90EF0DFE] = "m_rLeftHandStatesExitConfigs";
        (*property_map)[0x580B623B] = "m_SpatialPosOnCover";
        (*property_map)[0x2FC43F32] = "m_bWeaponsAllowed";
        (*property_map)[0xD69E9599] = "m_nGlowCivilianFadeSpeed";
        (*property_map)[0xF15D641C] = "m_fMinimumTimeToReturn";
        (*property_map)[0xCE59E287] = "m_fScale";
        (*property_map)[0x8CE49FE8] = "m_fParticleFxSize";
        (*property_map)[0x9AD774D0] = "m_sDeviceName";
        (*property_map)[0xE910719D] = "m_contractsBosySpottedEntity";
        (*property_map)[0xCB3C288] = "m_bDisableWind";
        (*property_map)[0xA9711827] = "m_aReveals";
        (*property_map)[0x76DBC11B] = "m_pTutorialImage";
        (*property_map)[0x621E1223] = "m_bExplosionRelevant";
        (*property_map)[0x2DACB630] = "m_bCanBeOperatedWithItemInLeftHand";
        (*property_map)[0x9463501A] = "m_bFireOnLevelStart";
        (*property_map)[0x6AF42493] = "m_bGettingDeactivated";
        (*property_map)[0x8C0EEFCE] = "m_pImpactMaterial";
        (*property_map)[0xDB861A10] = "m_bNeedExactRepositioning";
        (*property_map)[0x19AEE] = "m_bOnlyLocalImpacts";
        (*property_map)[0x70DE77FA] = "m_nMaxSMGAmmo";
        (*property_map)[0x700107A1] = "m_bIsLifetimeAnimation";
        (*property_map)[0x63895146] = "m_rWeapon";
        (*property_map)[0x1AE5D2EC] = "m_fTurningVelocity";
        (*property_map)[0x83B50061] = "m_sObjectiveId";
        (*property_map)[0xEC9582B6] = "m_HeirEntities";
        (*property_map)[0x35480565] = "m_bRestoreValueAfterEnd";
        (*property_map)[0xF8186C6E] = "fRadius";
        (*property_map)[0x9FDEBB6C] = "m_aCrowdExclusionSpheres";
        (*property_map)[0x90F20A4A] = "m_eAttachLocation";
        (*property_map)[0xF9F3A9D6] = "m_eFlowCandidates";
        (*property_map)[0xA96CCEFC] = "m_eMaxTension";
        (*property_map)[0x26896FD7] = "m_rOverrideOriginalCamera";
        (*property_map)[0xBD6E16CA] = "m_eSneakingRequirements";
        (*property_map)[0xF5862F39] = "m_fInvestigateCautiousCivilianPropagationProximitySpreadPerSecond";
        (*property_map)[0x67AA572B] = "m_CustomData";
        (*property_map)[0xC6B5AAA8] = "m_aContext";
        (*property_map)[0x4D703318] = "m_bDoHighLODSeparation";
        (*property_map)[0xA1BA286F] = "m_bIsEnabled";
        (*property_map)[0x3905BE36] = "fNearDistance";
        (*property_map)[0x656A3E29] = "m_rRevealDistanceCondition";
        (*property_map)[0xA11776D4] = "m_cHitmanAngleFalloff";
        (*property_map)[0xF3E72622] = "m_RadiusInHIPS";
        (*property_map)[0x9D0952A2] = "m_aValueEntities";
        (*property_map)[0x191A3231] = "m_bLeftHand";
        (*property_map)[0xB7AF96FC] = "m_StateController";
        (*property_map)[0x457CD88] = "m_bWeapon";
        (*property_map)[0xCBDDFABE] = "m_fSettingRemainingPathDistanceMin";
        (*property_map)[0xFB9EE142] = "m_planePoints";
        (*property_map)[0xA4F735E6] = "m_rConextObjectInteractionPoint";
        (*property_map)[0x9DDFC3F7] = "m_CharacterStatLabelList";
        (*property_map)[0x25B92884] = "m_pBehaviorEntity";
        (*property_map)[0xB47BA920] = "m_bIgnoreIfInactive";
        (*property_map)[0xAD7869E4] = "m_pDescriptionResourceXBoxOneOverride";
        (*property_map)[0x7F7E23C6] = "m_bNeedsLeftHand";
        (*property_map)[0xC07D1421] = "m_bPullVictimFromWindow";
        (*property_map)[0x466F2FFC] = "target";
        (*property_map)[0x7D6B592B] = "m_nEntityID";
        (*property_map)[0xA518A4B3] = "m_bObjectInPhysicsWorld";
        (*property_map)[0x44EC5074] = "m_nBackTurnedCrowdAttentionMod";
        (*property_map)[0x78D59C95] = "aimTarget";
        (*property_map)[0x71957374] = "m_fWaterPlaneHeight";
        (*property_map)[0x95083903] = "m_bIsTerminated";
        (*property_map)[0x21B18001] = "cbMappings";
        (*property_map)[0xCCCD5FE7] = "m_tLastInvestigationCompleted";
        (*property_map)[0x12A24124] = "m_bClearContextOnOpen";
        (*property_map)[0x559E18C2] = "m_bIncludeTarget";
        (*property_map)[0xA3BBB53E] = "propertyType";
        (*property_map)[0x624D615B] = "m_RedOfGreen";
        (*property_map)[0xCE7312FC] = "m_aVRAgilityConfigs";
        (*property_map)[0xA0781E25] = "m_aPromptText";
        (*property_map)[0xF215E4A8] = "bThroughWall";
        (*property_map)[0x4F21CF76] = "m_sContractSessionId";
        (*property_map)[0x7623B9CD] = "m_Camera";
        (*property_map)[0xFF578FE4] = "m_vLowBoundPos";
        (*property_map)[0x3F689F37] = "m_fValue";
        (*property_map)[0xACCB29FF] = "m_sMessage";
        (*property_map)[0x468381F5] = "shortId";
        (*property_map)[0xF26FEF6] = "speed";
        (*property_map)[0x434FD67F] = "m_pTemplate";
        (*property_map)[0x69E16D0B] = "m_nMaxRPGAmmo";
        (*property_map)[0x3F5BD5C3] = "m_WeaponRepositoryID";
        (*property_map)[0x15B89441] = "bInWarningZone";
        (*property_map)[0x7617B39E] = "m_ValueFloat";
        (*property_map)[0x95B8A21B] = "m_actors";
        (*property_map)[0xEAACC2CD] = "m_bDisableSpecReflProbes";
        (*property_map)[0x4B2547D0] = "m_bIsTimedOut";
        (*property_map)[0x2F89F6CF] = "m_fFrameRate";
        (*property_map)[0x9A41E07E] = "m_bIsUnpaused";
        (*property_map)[0xD34B4D08] = "m_fMaxTetherRange";
        (*property_map)[0xBB648123] = "m_bYawAnimation";
        (*property_map)[0x681D92E6] = "m_rProvider";
        (*property_map)[0xB6247568] = "m_BoxesDefiningArea";
        (*property_map)[0xD555D86F] = "m_vProjectorMapUVVelocity";
        (*property_map)[0x61B41D4D] = "m_fTargetRadiusOverride";
        (*property_map)[0xCFC7D02] = "m_target";
        (*property_map)[0xEB89572D] = "m_fSkinningConstrainScale";
        (*property_map)[0x967CE191] = "m_aOcclusionRooms";
        (*property_map)[0xD929A856] = "m_bLockedToActor";
        (*property_map)[0xC021C91A] = "m_pMetaDataResource";
        (*property_map)[0xDDDEFFF0] = "m_nMaxDistForTargetSteering";
        (*property_map)[0xF4E2B1D5] = "m_fExtrusionHeight";
        (*property_map)[0xFBA19880] = "m_bAlignYawPitch";
        (*property_map)[0x966BB1FE] = "shouldCrouch";
        (*property_map)[0x4341856B] = "m_bMountDismountLeft";
        (*property_map)[0xCAD8D3CF] = "m_farBoxSize";
        (*property_map)[0x2E998786] = "m_eventType";
        (*property_map)[0xDD83E884] = "m_bMultiplayerCrowd";
        (*property_map)[0x890536B8] = "m_duration";
        (*property_map)[0x979EEB6] = "m_aTargetOffsets";
        (*property_map)[0xE3410630] = "m_eActorFraction";
        (*property_map)[0xA3E2C94E] = "m_nEnd";
        (*property_map)[0xEF2F22EE] = "m_fASSAOMultiplier";
        (*property_map)[0xB3F2F116] = "m_nDirection";
        (*property_map)[0xD049C083] = "m_fT";
        (*property_map)[0x118A2306] = "m_fRLegDamageScalar";
        (*property_map)[0x11A8B257] = "m_bWaitingForLoadVideo";
        (*property_map)[0x3F42396] = "m_aKnownEntities";
        (*property_map)[0x59630EA2] = "m_pMaterialOrgResource0";
        (*property_map)[0x2F1DA3A1] = "m_eRayDetailLevel";
        (*property_map)[0x7F4A774] = "m_aGoalKeys";
        (*property_map)[0x22C246B6] = "m_DynamicEnforcerServiceData";
        (*property_map)[0x6A85C332] = "m_bVideoError";
        (*property_map)[0x4158AB03] = "typeSourceTemplate";
        (*property_map)[0xBE8AC20] = "m_fHMAttentionLastUpdate";
        (*property_map)[0x7AC921E] = "m_fLimitBias";
        (*property_map)[0xF83EE994] = "m_aGoals";
        (*property_map)[0xBB1F81FC] = "m_vBoxFadePos";
        (*property_map)[0x4FCFE5C4] = "m_BankResourceHQID";
        (*property_map)[0x44EB35D5] = "m_fPeripheralHigh";
        (*property_map)[0x3380D5A2] = "m_sMatchNode";
        (*property_map)[0xF47FE1F8] = "m_bAimAtPeople";
        (*property_map)[0xB403BFA2] = "m_fReticleLegendOffsetGamepad";
        (*property_map)[0x681D0E23] = "m_fHMAttention";
        (*property_map)[0x41D949D1] = "m_nTilesX";
        (*property_map)[0x28B4B20A] = "m_uiModOverride";
        (*property_map)[0x9F1FA89E] = "m_rEventProcessor";
        (*property_map)[0x426CF36F] = "m_nCharset";
        (*property_map)[0xB2E78BE5] = "m_bArrowStart";
        (*property_map)[0xDB6BBCD] = "m_fAngularDamping";
        (*property_map)[0x1CCC3401] = "m_nMaxSniperAmmo";
        (*property_map)[0x5BF3B14] = "m_fSubAngle1";
        (*property_map)[0x573AC3EF] = "m_fHMDisguiseAttention";
        (*property_map)[0xCF7517E8] = "Clients";
        (*property_map)[0x11174E1B] = "m_fHMTrespassingAttention";
        (*property_map)[0x52C57ACC] = "m_fProjectedAreaGateClipThreshold";
        (*property_map)[0x4C782B20] = "m_tLastTrespassingAttentionGain";
        (*property_map)[0x4FA799F2] = "m_fArrayIndexA";
        (*property_map)[0x8B0B565B] = "m_bTurnTowards3";
        (*property_map)[0x1FBFB328] = "m_tLastAttentionEvaluate";
        (*property_map)[0xBCB7E91F] = "m_fSwing2Limit";
        (*property_map)[0x5FA48A6C] = "m_eGameTension";
        (*property_map)[0x512709F4] = "m_bForceGeomGlowType";
        (*property_map)[0xA4D2E663] = "m_bEmptyExpressionEvaluatesTrue";
        (*property_map)[0x5368FAD8] = "m_sRawDisplayText";
        (*property_map)[0x4AE6F50] = "m_ExclusionList";
        (*property_map)[0xF58422DA] = "m_aCrowdGroupBoxes";
        (*property_map)[0x2364D89E] = "m_bRandomTile";
        (*property_map)[0xCA16AC63] = "m_fHuntedIgnoreDistanceSneakWalk";
        (*property_map)[0x8E1E4D98] = "m_tAmbientStartTime";
        (*property_map)[0xF5E9C0BD] = "m_sBriefingText";
        (*property_map)[0xA4405639] = "m_vDistortionWobbleSpeed";
        (*property_map)[0x3EFC3E23] = "m_tExpiredAIModifierSuppressSocialGreeting";
        (*property_map)[0x31F1E4BF] = "m_fDamageMax";
        (*property_map)[0xE083C5CD] = "m_pActorList";
        (*property_map)[0x2488E0BF] = "m_bOdometer";
        (*property_map)[0xEFED5128] = "m_eInvestigateSoundDef";
        (*property_map)[0x1025EE00] = "m_pFacing";
        (*property_map)[0xD0D7C06C] = "m_bVisible";
        (*property_map)[0xC50BCE51] = "m_Enabled";
        (*property_map)[0x5B17B596] = "m_nCombatMaxCoverShootActorCount";
        (*property_map)[0x41DCC9F7] = "m_fDurationVariance";
        (*property_map)[0x3A8A5C9] = "m_bOneShot";
        (*property_map)[0x698AFE2E] = "m_fRangeNearBonus";
        (*property_map)[0x8787346F] = "m_From";
        (*property_map)[0x2332F8F2] = "IsShore";
        (*property_map)[0xA35AEFEB] = "m_bReportCollisions";
        (*property_map)[0xAF926CDA] = "m_fInSpreadRangeDistance";
        (*property_map)[0x1CA23EDD] = "m_fParticleSizeMax";
        (*property_map)[0xCCFB0C82] = "m_bArrestReasonStated";
        (*property_map)[0x16DCE665] = "m_fDepthOfFieldFocusStart";
        (*property_map)[0x8BCE3747] = "m_bSituationActive";
        (*property_map)[0xBF5C85FB] = "m_bTriggerInWarzone";
        (*property_map)[0xD28B8C38] = "m_nPriority";
        (*property_map)[0x5346BBFB] = "m_pSafeZoneLoopPosition";
        (*property_map)[0x49ABE9AA] = "m_nFromEntityID";
        (*property_map)[0xA9E75AD1] = "m_vGlowBackgroundUnmaskedFadeNearFar";
        (*property_map)[0x6F777E83] = "m_nGlowContractTargetSpecialFadeSpeed";
        (*property_map)[0xC805FC86] = "m_AnimPlayer";
        (*property_map)[0x4E3E9AED] = "m_eTargetSelectionType";
        (*property_map)[0x51591A47] = "m_fPitchAngle1";
        (*property_map)[0x72DB9B27] = "m_bHDRActive";
        (*property_map)[0x6BD7031C] = "m_AnimSet";
        (*property_map)[0x3D074A85] = "m_rLadder";
        (*property_map)[0x4DEE9884] = "m_eLightType";
        (*property_map)[0xA0E8EEB7] = "m_NearDepth";
        (*property_map)[0x1AD5441A] = "m_fDurationInterval";
        (*property_map)[0xA24A6C4D] = "PlaybackBusName";
        (*property_map)[0x4F5A21E] = "m_fWaterTessellationFar";
        (*property_map)[0xFF08D3A1] = "m_fLookAtWeighting";
        (*property_map)[0x66AB2597] = "iEvents";
        (*property_map)[0x464F02AA] = "_a";
        (*property_map)[0x63312798] = "m_bCanMountBothSides";
        (*property_map)[0xBC96C6D5] = "m_bWasInvestigated";
        (*property_map)[0xD15907FE] = "difficultyLevel";
        (*property_map)[0x38170FCB] = "m_rDoorStuck";
        (*property_map)[0x16A00970] = "m_nReactionFarDelay3";
        (*property_map)[0xA2F9EF1B] = "m_rSpatial";
        (*property_map)[0xD8B93884] = "m_bAutoAspect";
        (*property_map)[0xD6D7A9BF] = "m_bUseParticleColor";
        (*property_map)[0x4EDA95EE] = "m_fOTSCamPrecisionModifier";
        (*property_map)[0x3A2D41A9] = "m_bFxDraw";
        (*property_map)[0x2853C7F3] = "m_fFootstepsSolidity";
        (*property_map)[0x6B259411] = "SubtitleLanguages";
        (*property_map)[0xE23EF912] = "m_object";
        (*property_map)[0xDCF5F8D9] = "m_fHDRLensFlareThresholdC";
        (*property_map)[0x822CF816] = "m_CurveEntity";
        (*property_map)[0xF7515BFD] = "m_rX";
        (*property_map)[0xFFD33A1C] = "combatPosition";
        (*property_map)[0x12716D08] = "m_type";
        (*property_map)[0x95275238] = "m_fCombatNPCDecisionRadiusAggressive";
        (*property_map)[0xEED9DA93] = "m_started";
        (*property_map)[0xDF7D309B] = "m_Source1";
        (*property_map)[0xD7978764] = "m_nColumn";
        (*property_map)[0xE8F1A48] = "fTotalDamage";
        (*property_map)[0xAEEE72E5] = "m_fExplosionPushBorder";
        (*property_map)[0xDE68A17E] = "m_fStopFollowDistance";
        (*property_map)[0xACBFCA9E] = "m_nDistance";
        (*property_map)[0x72069170] = "m_IgnoredSpatials";
        (*property_map)[0x111D3798] = "boneId";
        (*property_map)[0xEB871E17] = "m_bLockOnHitmanPosition";
        (*property_map)[0x6B0E18E1] = "m_fVisionSamplesMax";
        (*property_map)[0x7F74C940] = "m_rWorldPosEntity";
        (*property_map)[0xE284468] = "entity";
        (*property_map)[0xD2601C4] = "bIsArray";
        (*property_map)[0x82705553] = "m_bFreezeOnLowSpeed";
        (*property_map)[0x13D34408] = "bInsideAreaBeingInvestigated";
        (*property_map)[0xDCA65EEE] = "m_pTitle";
        (*property_map)[0x5180E750] = "m_value";
        (*property_map)[0x6D660211] = "m_fOcclusionFactorClosed";
        (*property_map)[0xB40BA5E6] = "m_nSuspiciousDeaths";
        (*property_map)[0x771F03C9] = "m_fTrespassingPenaltySneakWalk";
        (*property_map)[0x393BD41A] = "m_bEnablePickup";
        (*property_map)[0x27113016] = "m_pActEscortSearchTemplate";
        (*property_map)[0x972215AA] = "m_sRawStateTextIfOn";
        (*property_map)[0xF2B42DE] = "m_fSafetyTimeOut";
        (*property_map)[0xF6CB170A] = "m_fNPCAttractRadius";
        (*property_map)[0x8CDE5729] = "type";
        (*property_map)[0xF8E9934D] = "m_eCollisionLayer_FullyDetached";
        (*property_map)[0x9257E0FC] = "m_sLongBriefingText";
        (*property_map)[0x77B93BA8] = "m_eCollisionResponse";
        (*property_map)[0xB765AC30] = "m_fHideInCoverTimerMax";
        (*property_map)[0x38256217] = "m_fCoefficientOfRestitution";
        (*property_map)[0xC5DB39DB] = "m_fInitialBlendTime";
        (*property_map)[0x10BD9AFF] = "m_aGateData";
        (*property_map)[0xC95238E1] = "minDistance";
        (*property_map)[0xCDFED0E7] = "m_fD";
        (*property_map)[0x247157AA] = "m_LinkedEntityReplicator";
        (*property_map)[0xFAF73568] = "m_fFov1Min";
        (*property_map)[0xA81A0D87] = "m_nColor";
        (*property_map)[0x8B31A161] = "m_fCurveDurationSpine1";
        (*property_map)[0x7944EB38] = "m_bKickNPCIsAccident";
        (*property_map)[0xC5DC4EB7] = "m_fDirectionCoefficient";
        (*property_map)[0x8AF1095E] = "m_vSize";
        (*property_map)[0xB61030C0] = "m_eTensionRemoveLimit";
        (*property_map)[0x80FBF4F9] = "m_fFreezeSpeedThreshold";
        (*property_map)[0x8E837372] = "m_bUsePerVertexSkinning";
        (*property_map)[0xAA45DDB2] = "m_fPanSensitivity";
        (*property_map)[0x5F1C0E75] = "m_fCullDistanceXN";
        (*property_map)[0xBF298A20] = "Item";
        (*property_map)[0x1753241C] = "m_bIsInventorySubactionGroup";
        (*property_map)[0x55CFA8B2] = "m_bDynamicCollisions";
        (*property_map)[0x993F9727] = "m_fObstructionMaxRadius";
        (*property_map)[0x8BC64B58] = "m_bPlayAnnouncementDialog";
        (*property_map)[0x46490A0B] = "m_pInCoverReaction";
        (*property_map)[0x1492E3DD] = "m_nTargetNodeIndex";
        (*property_map)[0xF14471BE] = "m_pVideoDatabase";
        (*property_map)[0x689223BD] = "m_ActivationCondition";
        (*property_map)[0x43EFE99B] = "m_cameraEntity";
        (*property_map)[0xEA5A42C4] = "m_bIsUICamera";
        (*property_map)[0x7C6EC14B] = "m_sSweetDisplayText";
        (*property_map)[0x1F8F57E5] = "m_DisguiseAttentionDuration";
        (*property_map)[0x387ADD7F] = "m_FailedEventValues";
        (*property_map)[0x10C6DB59] = "m_QualityNeo";
        (*property_map)[0xF506048B] = "m_nStashSize";
        (*property_map)[0xFDDDB26E] = "m_fStrengthAmplitude";
        (*property_map)[0xF3305548] = "m_eMaxGameTenstion";
        (*property_map)[0x911F2940] = "m_PosFreeBoneAttach";
        (*property_map)[0x829B08F2] = "m_pTargetControl";
        (*property_map)[0x20382383] = "m_bIsEscapeNode";
        (*property_map)[0x83D23EC3] = "m_fAngleSpine1";
        (*property_map)[0x19D9B923] = "m_pTemplateResource";
        (*property_map)[0xA497B8DD] = "m_bManaged";
        (*property_map)[0x26A24BE0] = "m_bReplicate";
        (*property_map)[0x80077E80] = "m_fHuntedPenaltyRun";
        (*property_map)[0xBA9D8C36] = "m_pDataProvider";
        (*property_map)[0x212943A8] = "m_rCastActor";
        (*property_map)[0x2C1A8783] = "m_bKeepPlayingAfterSequenceEnds";
        (*property_map)[0xF7F9E167] = "m_MicroMarkersVRControl";
        (*property_map)[0x6640B3E] = "m_RepositoryId";
        (*property_map)[0x5821AEC2] = "m_vGlowCivilianFadeNearFar";
        (*property_map)[0x6B1C0EEE] = "m_bNodeFilterCrowd";
        (*property_map)[0x11EF9C60] = "m_bIsControlPointsPathClosed";
        (*property_map)[0x6114C514] = "m_MuzzleSmokeEffect";
        (*property_map)[0xFC7F336A] = "m_cBackToIdleCurve";
        (*property_map)[0xBAACE981] = "locationMustBeVisible";
        (*property_map)[0xD3037658] = "m_bLerpMotion";
        (*property_map)[0xD479BD0E] = "m_fHeroStandStillDurationBeforeIntermediate";
        (*property_map)[0x96455BFA] = "m_fHorizontalCollisionsAngleTolerance";
        (*property_map)[0x24DC3226] = "m_vGlobalSize";
        (*property_map)[0xA73B78C2] = "m_FacePunchEmitter";
        (*property_map)[0x731610E0] = "m_fPlaybackSpeed";
        (*property_map)[0x152BBFAA] = "m_rCameraTarget";
        (*property_map)[0x757EA9BD] = "m_rFilter";
        (*property_map)[0x40153E70] = "m_nPatrolWaypointIndex";
        (*property_map)[0x2A5029D5] = "m_bIsUndefined";
        (*property_map)[0x119BD69E] = "m_bDisplayAsKill";
        (*property_map)[0x31A618A5] = "m_rCamera";
        (*property_map)[0x689EA2B3] = "m_rRevealedCondition";
        (*property_map)[0xC58D5E8F] = "m_bUseGlobalRandom";
        (*property_map)[0xC0D5D92D] = "m_bHideOnStart";
        (*property_map)[0x4C9C0659] = "propertyOwner";
        (*property_map)[0xFA4978FE] = "m_pStartCondition";
        (*property_map)[0xAC2D752B] = "m_fWeight1";
        (*property_map)[0xDC4781A4] = "m_fWeight4";
        (*property_map)[0x25728B5] = "exposedEntities";
        (*property_map)[0x40A54C2] = "m_eFaceDirection";
        (*property_map)[0xA17FA65E] = "m_nType";
        (*property_map)[0x5874284C] = "m_bForwardFocusToSelected";
        (*property_map)[0x370D2BA1] = "m_ValueBool";
        (*property_map)[0x5B1ED1F8] = "m_bRandomFlipV";
        (*property_map)[0x9BB1070D] = "m_Curve1";
        (*property_map)[0xAB40B132] = "m_fWeight5";
        (*property_map)[0x2F960C9A] = "sSymbolName";
        (*property_map)[0xAA0A048D] = "m_rItemDestroyed";
        (*property_map)[0xDD98C34F] = "m_rEnabledCondition";
        (*property_map)[0x1ADE4F80] = "m_BodyPartLODCutOffMeshResource";
        (*property_map)[0x1FFCE5C4] = "m_bEnableLightFade";
        (*property_map)[0xDE42A620] = "m_EdgeTraversalPenaly";
        (*property_map)[0x62DA3B07] = "aievent";
        (*property_map)[0x2B86D8B] = "m_rSwitchWeaponClass";
        (*property_map)[0x8B4039D8] = "m_fLidOpenFraction";
        (*property_map)[0x97624B18] = "m_bCanBeMarked";
        (*property_map)[0xE48CC842] = "m_Id";
        (*property_map)[0x5F8E1AEB] = "m_bIsAttached";
        (*property_map)[0x2AA45480] = "m_tLastSearchCompleted";
        (*property_map)[0x6F8946FC] = "m_aDescriptors";
        (*property_map)[0x3434D703] = "m_eCategory";
        (*property_map)[0xE7032C7C] = "m_bWeaponRelevant";
        (*property_map)[0x666A78BE] = "lookAtTarget";
        (*property_map)[0x8D933AE] = "m_sScatterMap";
        (*property_map)[0xBCFB3EC7] = "m_bRegisterInNotebook";
        (*property_map)[0xBCD748E9] = "m_vEndPosition";
        (*property_map)[0x9238DD7B] = "m_BloodEmitter";
        (*property_map)[0xA48FBE63] = "m_pStreamedResPtr";
        (*property_map)[0xE67F2C87] = "m_bUseSkinningSphereConstrains";
        (*property_map)[0x2CC42AC8] = "fHealthPercentTimeout";
        (*property_map)[0xAD7C9C45] = "m_bCompleted";
        (*property_map)[0x321F7247] = "m_bItemsStored";
        (*property_map)[0xB04AFF4] = "m_bPlayerCheckEnabled";
        (*property_map)[0x7E77D7C8] = "m_bNotIgnoreTrespassing";
        (*property_map)[0x12B528BB] = "m_Source";
        (*property_map)[0x68D12778] = "m_fMoveOrderFuturePosition";
        (*property_map)[0x1F3B15FD] = "m_MuzzleSmokeEffectGroup";
        (*property_map)[0xF3EFC919] = "m_bFailed";
        (*property_map)[0x6210848E] = "m_TargetWeaponItem";
        (*property_map)[0x667E0626] = "m_ridDecalMaterial";
        (*property_map)[0x74269052] = "m_bPushRelevant";
        (*property_map)[0xF1A3481C] = "m_bAlignOnly";
        (*property_map)[0x4CD89789] = "m_NormalMapID";
        (*property_map)[0x4F75D187] = "bInReFriskZone";
        (*property_map)[0xEC9C0666] = "m_fSocializingCooldown";
        (*property_map)[0x5EE8E519] = "m_bMakeSilenced";
        (*property_map)[0x481D5035] = "m_sSuccessEventName";
        (*property_map)[0x43AD13D9] = "m_bExpectedLethalThrow";
        (*property_map)[0x107DCA77] = "m_bIsIdle";
        (*property_map)[0x618A618A] = "m_SuccessEventValues";
        (*property_map)[0xA606E2CE] = "m_StartupDisguiseId";
        (*property_map)[0x628E9301] = "m_IsExitingSuccessfulBehavior";
        (*property_map)[0xE71085F2] = "m_rDeadBody";
        (*property_map)[0x76349F9B] = "m_nMaxPistolLightAmmo";
        (*property_map)[0x9EC45A78] = "m_IDLow";
        (*property_map)[0xA86A9E1C] = "m_nNumOfRungs";
        (*property_map)[0xA36F2CEA] = "sContractSessionId";
        (*property_map)[0x6727ABD7] = "m_sResetEventName";
        (*property_map)[0x847368E5] = "m_PenaltyEnforcer";
        (*property_map)[0x748E7454] = "m_sHUDText";
        (*property_map)[0xB4B56B8E] = "m_bSpawnOnStart";
        (*property_map)[0x307141FB] = "m_GestureBackAct";
        (*property_map)[0x7D7482BC] = "m_ImmunityDuration";
        (*property_map)[0x4CE56DF7] = "m_bTriggerEvents";
        (*property_map)[0x6A32E475] = "m_iRepeatSuccess";
        (*property_map)[0x4CCE3BF2] = "m_er64";
        (*property_map)[0x4037A07A] = "m_GlobalLightFrontColor";
        (*property_map)[0x500C8663] = "m_eDetachOnCollision";
        (*property_map)[0x36BE89FA] = "m_nCrowdActorRadius";
        (*property_map)[0xDC26772E] = "m_bAntiboid";
        (*property_map)[0x6CE14823] = "a1";
        (*property_map)[0xB3E33F9D] = "m_bTriggerOnTransition";
        (*property_map)[0x1018FE9] = "m_fAttenuationSpreadDistance";
        (*property_map)[0xACE777B0] = "m_iRepeatFailed";
        (*property_map)[0x98D322AF] = "m_iCurrentSuccess";
        (*property_map)[0x4D72CB8A] = "m_bIsPreferredToFire";
        (*property_map)[0x9C5A7D07] = "m_nNumShotsFired";
        (*property_map)[0x13B7E26E] = "m_GlowContractTargetNonCriticalUnconsciousOutlineColor";
        (*property_map)[0xFDE241CC] = "m_EscortedActor";
        (*property_map)[0xBC0BD465] = "m_bKiller";
        (*property_map)[0xED1EB8CF] = "m_fAgilityAttractionSlidePenalty";
        (*property_map)[0x2F21E3E3] = "m_iCurrentFailed";
        (*property_map)[0xB9ABC915] = "m_bPCCastShadows";
        (*property_map)[0xBA0C3F25] = "m_bEnableCollision";
        (*property_map)[0xB8B56835] = "m_nCellSizeX";
        (*property_map)[0x7D1FF80C] = "m_rConnectionEnabled";
        (*property_map)[0x6A231CA5] = "m_fDamping";
        (*property_map)[0xBB01C8C8] = "maxDeathType";
        (*property_map)[0xBDC0ECAF] = "m_ActivationValue";
        (*property_map)[0xAE30C695] = "m_fGlowEnforcerOutlineWidth";
        (*property_map)[0xE1C4C7DB] = "m_bBlockSilencedGunShots";
        (*property_map)[0xDE89FB23] = "m_bTriggeredAlarm";
        (*property_map)[0xEFEBD003] = "m_bUpdateActivationOnCompleted";
        (*property_map)[0xEA8AB2F7] = "m_bCanBeUsedAsIntermediateAnimation";
        (*property_map)[0x3870DF6C] = "m_bShowInHud";
        (*property_map)[0xF87A03AE] = "m_bKickNPCEnabled";
        (*property_map)[0x2B2EEB97] = "m_IsInMorgue";
        (*property_map)[0xC6B35F59] = "m_bDifficulty_Level2";
        (*property_map)[0x96D60DBE] = "m_HiddenColor";
        (*property_map)[0x2CFF62DC] = "m_aSocialTargets";
        (*property_map)[0x6FD1746D] = "m_fExplosionLiftMultiplier";
        (*property_map)[0xDF1599B6] = "m_fClampRadiusOut";
        (*property_map)[0xEABE4EAF] = "m_Distance";
        (*property_map)[0xC1E2B652] = "m_bCombinedDisplayInHud";
        (*property_map)[0x988128FC] = "m_rBagSharedKnowledge";
        (*property_map)[0x80F77234] = "m_aPhysicsAspects";
        (*property_map)[0xAC793729] = "m_pBusyStateIndicator";
        (*property_map)[0xCAF937AF] = "m_OnInactive";
        (*property_map)[0x2A75CBA3] = "m_fDefaultActivationArc";
        (*property_map)[0xBC3CD67F] = "m_OnActive";
        (*property_map)[0xF2AC3F0E] = "m_bConnectsToTarget";
        (*property_map)[0xFD75553B] = "m_Spheres";
        (*property_map)[0x81732109] = "m_fCurveStartTimeSpine1";
        (*property_map)[0x71D39CB6] = "m_iInitialControlCount";
        (*property_map)[0xF7E30FBD] = "m_fVerticalMarginBottom";
        (*property_map)[0xC3381C33] = "m_aExits";
        (*property_map)[0x3C257A02] = "m_bUseDynamicShadowMap";
        (*property_map)[0xB4395074] = "m_sImage";
        (*property_map)[0xDFF16FC8] = "m_desiredTargetLocation";
        (*property_map)[0x5AEE7A37] = "m_nMinCutoff";
        (*property_map)[0xDEECE8DF] = "m_fStartSearchOnTargetDeadSubsequentDelay";
        (*property_map)[0x935B0FC9] = "m_fCivilianPropagationLOSMinDistance";
        (*property_map)[0xB16B7EC] = "m_fSplashVelocity";
        (*property_map)[0x2FD4C259] = "m_eBoundsType";
        (*property_map)[0x995598EC] = "m_sObjectiveType";
        (*property_map)[0x1C034728] = "m_StatLabelList";
        (*property_map)[0x8234EAD6] = "m_fDecayTime";
        (*property_map)[0x747650F0] = "m_fSwingDriveStrength";
        (*property_map)[0xDC2E36E6] = "m_fPlayerMaxInfectedPercentage";
        (*property_map)[0x9041197C] = "m_sBriefingName";
        (*property_map)[0xA33F09A6] = "m_fCameraFadeNearDistance";
        (*property_map)[0x539AA63E] = "m_fExplosionLiftBias";
        (*property_map)[0x244B61D7] = "m_aTrackerPool";
        (*property_map)[0x32210115] = "m_sAuxBusName";
        (*property_map)[0x99F36485] = "m_bShowChildrenOnSelected";
        (*property_map)[0x9ECF6B15] = "m_sOutfit";
        (*property_map)[0xDCA0234E] = "m_nMinSeparationDist";
        (*property_map)[0x16D1D769] = "m_bHasEnteredOnce";
        (*property_map)[0x153C3CE] = "m_bAuthorityFigure";
        (*property_map)[0x89BF9DBC] = "m_aValueXEntities";
        (*property_map)[0x324F9680] = "m_fSlowDownStrafeFarFactor";
        (*property_map)[0xE042905D] = "m_vGlowInteractionDeselectedVignetteRadii";
        (*property_map)[0xA75263FD] = "m_Memory";
        (*property_map)[0xFD55FBD7] = "m_bBlurMips";
        (*property_map)[0xD1C0E74A] = "m_bIsPaused";
        (*property_map)[0x1E259930] = "m_tLastSearchCalc";
        (*property_map)[0x72DCED8D] = "m_rItemPickupEnabled";
        (*property_map)[0x6F949845] = "time";
        (*property_map)[0x8312274E] = "m_bDynamicEnforcer";
        (*property_map)[0x4EECCA9C] = "m_WaitMaxSeconds";
        (*property_map)[0xEF23134F] = "m_nVariation";
        (*property_map)[0xC50A1C4C] = "m_rAmmoEffectFactory";
        (*property_map)[0xC18ACC90] = "m_eAutoScanMode";
        (*property_map)[0x48EBAA94] = "m_nFramesPerRun";
        (*property_map)[0x220271B2] = "m_nMaxObstruction";
        (*property_map)[0x1B56E09A] = "m_fEffectActivationTimeout";
        (*property_map)[0xFAFA0683] = "m_CollisionEntity";
        (*property_map)[0x57F16D6E] = "m_fHealth";
        (*property_map)[0xE1B387F8] = "m_aUnpauseActions";
        (*property_map)[0x66871EEE] = "m_PosHandAttachVROverride";
        (*property_map)[0xA73B0691] = "m_bIgnoreOutifChange";
        (*property_map)[0x76E8DB1C] = "m_bSetKinematicOnPlace";
        (*property_map)[0xC2BC5E97] = "m_SecondaryTargetControl";
        (*property_map)[0x4C01E0DA] = "m_rItemInPhysicsWorld";
        (*property_map)[0x93FEF9F5] = "m_eFoot";
        (*property_map)[0xD7410E37] = "m_CandidateTime";
        (*property_map)[0xE9B0050] = "m_StatusMarkersVRControl";
        (*property_map)[0x34C803E2] = "pTemplateResource";
        (*property_map)[0xBD942468] = "m_fA";
        (*property_map)[0x41DFDDDF] = "m_nbCheckForInfiniteLoops";
        (*property_map)[0x4CB13B48] = "m_aItemSpawners";
        (*property_map)[0xA03C2391] = "m_rSoundEvent_Push_Victim";
        (*property_map)[0x42DEADEB] = "m_bAllowEditorProjectionOverride";
        (*property_map)[0x249D75D2] = "m_fB";
        (*property_map)[0x13AEB6BC] = "m_bNodeFilterSpeedTree";
        (*property_map)[0x5919E198] = "m_rMessageResource";
        (*property_map)[0x5045948C] = "m_bSubstract";
        (*property_map)[0x851FBA9B] = "m_nStartIndex";
        (*property_map)[0x621A63EF] = "m_fArrowSize";
        (*property_map)[0x9BC20BC4] = "m_bProbeDown";
        (*property_map)[0x2CF2CF39] = "m_bEnableAnimationScaling";
        (*property_map)[0xE967A9CD] = "m_bGenerateOnStart";
        (*property_map)[0x708CC6C1] = "m_actorSlotDistanceMin";
        (*property_map)[0x1A9D3EBA] = "m_pCurrentInstance";
        (*property_map)[0xE702DE6A] = "m_fPropagationSpeed";
        (*property_map)[0xF378311C] = "m_tStart";
        (*property_map)[0xE2A69939] = "m_fMaxIdleTime";
        (*property_map)[0x4DF07C85] = "m_bSphereTriangles";
        (*property_map)[0x655C56C7] = "m_rMovementExitedConfigs";
        (*property_map)[0xB89D6CBE] = "m_actorTemplate2";
        (*property_map)[0xFB37DB64] = "m_bBodyDumpEnabled";
        (*property_map)[0x59B27F23] = "entity2";
        (*property_map)[0xC943C699] = "m_Actor";
        (*property_map)[0x95C24A8] = "m_rRightHandStatesEnterConfigs";
        (*property_map)[0x83F28068] = "m_aOutOfRangeScreenplays";
        (*property_map)[0xB5DC3781] = "m_sContractType";
        (*property_map)[0x1A254B89] = "m_NumSlitThroatEmitters";
        (*property_map)[0x67913164] = "m_pathToDirectory";
        (*property_map)[0x2B87425F] = "m_rRightHandStatesExitConfigs";
        (*property_map)[0x2C6C237F] = "m_fGlobalLightFrontIntensity";
        (*property_map)[0x78E15ADD] = "m_aTracks";
        (*property_map)[0x75555808] = "RemoveAgentData_GuardDomain_Pool_SaveData";
        (*property_map)[0xA836DF99] = "m_rLeftHandStatesEnterConfigs";
        (*property_map)[0x5C7481C9] = "m_bRelativeRotation";
        (*property_map)[0x4F1F3EA0] = "hardCondition";
        (*property_map)[0x90A18A1] = "m_aFlowVectorIndex";
        (*property_map)[0xE74E4E45] = "m_rEventConfigs";
        (*property_map)[0xB721B18A] = "m_bCheatSpawnGroup_Keys";
        (*property_map)[0xE480EA07] = "m_bIsActivated";
        (*property_map)[0xAA6E38F5] = "m_AudioResource";
        (*property_map)[0x88C70C6E] = "m_tTriggerTime";
        (*property_map)[0xED91EE11] = "m_aVRConfigComponents";
        (*property_map)[0x1DE97F23] = "constantPinValue";
        (*property_map)[0xCF8B5AF] = "sAliasName";
        (*property_map)[0x24DF34AD] = "m_aIndices";
        (*property_map)[0x213DD60A] = "m_eEventSetOnEscalation";
        (*property_map)[0xCBB993B3] = "m_fResistFactor";
        (*property_map)[0xBE6EA09A] = "m_fChangeAmmoSafetyTimeout";
        (*property_map)[0xBEE06094] = "m_aReferencedControllers";
        (*property_map)[0xCD460934] = "entityID";
        (*property_map)[0x7048243E] = "eventType";
        (*property_map)[0xB11AD4B7] = "m_reportedTarget";
        (*property_map)[0x51FEC98B] = "m_actorTemplate4";
        (*property_map)[0xAEA7C8AC] = "m_bCheckRightHand";
        (*property_map)[0x1AE8D74E] = "sPropertyName";
        (*property_map)[0x9023664A] = "causalnode";
        (*property_map)[0x1E29B1F] = "m_sTextLanguage";
        (*property_map)[0x40EC1F32] = "m_pExpectedOutfits";
        (*property_map)[0x794CCFE1] = "m_fTrespassingIgnoreDistanceRun";
        (*property_map)[0xA7056326] = "sName";
        (*property_map)[0xAFCAF10B] = "m_rScreenplay";
        (*property_map)[0x85581182] = "m_pEmoteID";
        (*property_map)[0xBBED7C52] = "m_fSpringMassPrimedLarge";
        (*property_map)[0x5FCF4A42] = "m_eParameter";
        (*property_map)[0x488EC7E5] = "m_bToggle";
        (*property_map)[0x524D578A] = "m_iPrecisionNumBullets";
        (*property_map)[0x3830D430] = "m_aLoadingScreenTips";
        (*property_map)[0x912A2061] = "m_pClipFactory";
        (*property_map)[0x3E7B07EC] = "m_eDifficulty";
        (*property_map)[0x6B25CFF5] = "m_BoxDefiningZone";
        (*property_map)[0xC1D15060] = "m_fTrespassingIgnoreDistanceWalk";
        (*property_map)[0x1664E5F3] = "m_fVisionSampleRate";
        (*property_map)[0xF3C74840] = "m_aWeaponsAllowedExceptionKeys";
        (*property_map)[0x4A01290C] = "m_rHitCrowdActor";
        (*property_map)[0x5EF59DC8] = "m_fVisionMaxDistance";
        (*property_map)[0x46073F34] = "m_fCullDistanceYN";
        (*property_map)[0x94AAB2BF] = "m_fVisionDistance";
        (*property_map)[0x26198B95] = "m_rAnimVariationResourceRequirement";
        (*property_map)[0x72085ADE] = "m_aActEvents";
        (*property_map)[0x32B7FF72] = "m_nMaxMGAmmo";
        (*property_map)[0xEC0FBF28] = "m_sCustomTags";
        (*property_map)[0x44741C6] = "m_pEntityA";
        (*property_map)[0x2774FAF5] = "m_eMenu";
        (*property_map)[0xA2610116] = "m_fTrespassingPenaltyStand";
        (*property_map)[0x8105843C] = "pTarget";
        (*property_map)[0xD2E7CDE8] = "m_bDisabled";
        (*property_map)[0xDFFE6A14] = "m_bEverOwnedByHitman";
        (*property_map)[0x9172C517] = "m_nReactionAngle3";
        (*property_map)[0xEDDA0252] = "m_eActorCCClass";
        (*property_map)[0x80D52EF6] = "m_bEnableCloth";
        (*property_map)[0x5951686F] = "m_fTrespassingPenaltyWalk";
        (*property_map)[0x1FEF9EB6] = "m_members";
        (*property_map)[0x67F55909] = "m_fCameraFadeFarDistance";
        (*property_map)[0x9A45388] = "m_pEnabledCondition";
        (*property_map)[0x2B547CA3] = "m_fTrespassingPenaltyRun";
        (*property_map)[0x32469CBF] = "m_fTrespassingIgnoreDistanceStand";
        (*property_map)[0xB2170683] = "m_bPreferredAccidentInvestigator";
        (*property_map)[0xDE43FF7C] = "m_bCheckItemOnBackLegal";
        (*property_map)[0xC6E7710E] = "m_fBoxVisibleStart";
        (*property_map)[0xAC5AF022] = "m_fHuntedIgnoreDistanceWalk";
        (*property_map)[0x1576C6CB] = "m_pDragBodyReaction";
        (*property_map)[0x987D5FC4] = "m_fTrespassingIgnoreDistanceSneakWalk";
        (*property_map)[0x6459D32C] = "m_vASSAOFadeOutRange";
        (*property_map)[0x5996C50E] = "m_eAlignment";
        (*property_map)[0x18300A0A] = "bKillByAccident";
        (*property_map)[0x8781CB35] = "m_bAllowFastWalk";
        (*property_map)[0xF2C6BC08] = "m_fHuntedPenaltyStand";
        (*property_map)[0x18A2553D] = "m_bIsTurning";
        (*property_map)[0x98C835AA] = "m_bApplyToActorListOnStart";
        (*property_map)[0x5F7E4D01] = "m_aPendingTimers";
        (*property_map)[0x100471] = "m_nProbeDistance";
        (*property_map)[0x4A8E6885] = "m_rConnectorBeginning";
        (*property_map)[0xCC68773C] = "m_vGlowCivilianVignetteRadii";
        (*property_map)[0x2521202B] = "m_bIsPointOfInterest3";
        (*property_map)[0xE211DEFC] = "m_pUseAction";
        (*property_map)[0x807235FC] = "m_fHuntedPenaltySneakWalk";
        (*property_map)[0xFA835850] = "m_bHasInitialValue";
        (*property_map)[0xC15DEEC9] = "m_pActiveBehavior";
        (*property_map)[0x1EE93C] = "noItemsMessage";
        (*property_map)[0x6E5E6A1F] = "m_fCameraFacingBlendTime";
        (*property_map)[0xFB9D4A1F] = "m_fHuntedPenaltyWalk";
        (*property_map)[0x5AA9A1E1] = "m_pVisibleWeaponReaction";
        (*property_map)[0xEBF2DC42] = "m_sName";
        (*property_map)[0xA37882A4] = "m_rStone";
        (*property_map)[0x63C1636A] = "m_eRessourceType";
        (*property_map)[0x63B0E00C] = "m_Listener";
        (*property_map)[0xA2C847EA] = "m_fTargetPathOffsetMax";
        (*property_map)[0x6A52D5CE] = "m_aReferencedStates";
        (*property_map)[0xE7605E7E] = "m_nTargetValueFloat";
        (*property_map)[0x81399AB7] = "m_rCompromisedEvacNode";
        (*property_map)[0xCAC4A334] = "m_fCurveStartFractionSpine2";
        (*property_map)[0x72346146] = "m_bHQStaticShadowMap";
        (*property_map)[0x4AA9B13F] = "m_fPlaneAlignmentAngularSpeed";
        (*property_map)[0xA6C51444] = "m_TargetControls";
        (*property_map)[0x44BB1696] = "m_entity";
        (*property_map)[0x6AA3ADB5] = "m_bPositiveY";
        (*property_map)[0x4DFA9C4D] = "m_AvailableWeaponKeys";
        (*property_map)[0x9CC1E5A2] = "m_bCastShadows";
        (*property_map)[0xEF429D8C] = "m_Visible";
        (*property_map)[0x2B070D98] = "m_bConditionValue";
        (*property_map)[0x7D825CA6] = "m_sDefaultItemName";
        (*property_map)[0xB889F684] = "m_fTimeOfDayRangeEnd";
        (*property_map)[0xE90E3A90] = "m_bTimerHasJustStopped";
        (*property_map)[0xCA47136F] = "m_bInclusive";
        (*property_map)[0x2AF0CE3F] = "m_aExcludedPropertyNames";
        (*property_map)[0xC6A55B35] = "m_eMovement";
        (*property_map)[0x77E035BF] = "m_bEnableHitmanBulletImpactCulling";
        (*property_map)[0xB5E7BDA6] = "m_nAmmoCountGun";
        (*property_map)[0xD097A8BE] = "m_fBrushFalloffRange";
        (*property_map)[0x8394CE4A] = "m_bRequiresHitmanInFront";
        (*property_map)[0x469EAFE8] = "m_nMaxElement";
        (*property_map)[0x38010137] = "m_vGlowTagFocusUntaggedFadeNearFar";
        (*property_map)[0xCA6A6C51] = "m_bPulsing";
        (*property_map)[0xB08F15B9] = "m_bFire";
        (*property_map)[0x6184FD21] = "m_GlowContractTargetNonCriticalColor";
        (*property_map)[0xAC74095A] = "activity";
        (*property_map)[0x83E596FD] = "m_rDisguisePositions";
        (*property_map)[0xED586F51] = "m_pActionIcons";
        (*property_map)[0xB3FEE5DF] = "m_bAccidentsResolved";
        (*property_map)[0xCDD7FA6D] = "m_fSkillReloadSpeedCoef";
        (*property_map)[0x6CB0696C] = "objectives";
        (*property_map)[0xC5EF597C] = "m_GestureRightAct";
        (*property_map)[0x83FF13A4] = "m_nGlowCameraFadeSpeed";
        (*property_map)[0xD44FA5DF] = "m_fFov2Max";
        (*property_map)[0x5FEE23A9] = "sWorldSpaceSettings";
        (*property_map)[0x41D34E51] = "m_eExitActionAmbient";
        (*property_map)[0xFC22EB88] = "exits";
        (*property_map)[0xB3ACFFED] = "m_GlowTagFocusTaggedUnconsciousOutlineColor";
        (*property_map)[0x8A2C8EA5] = "m_fEmissionDelta";
        (*property_map)[0x7159BCC3] = "m_ParticleColorRangeEnd";
        (*property_map)[0x9AB66076] = "m_aOutfitKeys";
        (*property_map)[0xD0629FF5] = "m_pCausticsDistortionMap";
        (*property_map)[0x676BF61E] = "m_eRegionMask";
        (*property_map)[0x28DCFEB2] = "m_bTimerHasStopped";
        (*property_map)[0x5F72E4A] = "m_fCost";
        (*property_map)[0x18012AAF] = "m_Stage2SpreadEvaluator";
        (*property_map)[0xFDB1DBCD] = "m_aWaypoints";
        (*property_map)[0xAB893E20] = "m_bAffectsAllowedDisguises";
        (*property_map)[0xE3FDF336] = "m_fPathWidth";
        (*property_map)[0x9F841846] = "m_vHitmanExitPosition";
        (*property_map)[0xE72E6FE9] = "m_fScopedPrecisionShotCameraSpeedMultiplier";
        (*property_map)[0xC981BC2] = "m_fEnergyThreshold";
        (*property_map)[0x431713A0] = "m_Entity";
        (*property_map)[0xAA6C0F2C] = "m_bCollisionGroup2";
        (*property_map)[0x9B644C46] = "m_bIsCyclable";
        (*property_map)[0xFF1EF365] = "m_rRightHandColliders";
        (*property_map)[0xF8AE3A93] = "m_nEscalation";
        (*property_map)[0xCB8AD3DA] = "m_GlobalLightOverrideFrontColor";
        (*property_map)[0xB7330DA5] = "m_MaterialID";
        (*property_map)[0xBD6F04DC] = "m_aBodyPartSetups";
        (*property_map)[0x8CFA8BE7] = "m_fStartFollowDistance";
        (*property_map)[0x5C2346A1] = "m_rCCWeaponBroken";
        (*property_map)[0x6437376E] = "m_bPosPinOnActivate";
        (*property_map)[0xAEEB2EDB] = "m_bAnchorEnabled";
        (*property_map)[0x544A91B9] = "m_vGlowContractFadeNearFar";
        (*property_map)[0x52548AE8] = "m_bMoving";
        (*property_map)[0x8398B489] = "m_fCloseupPlaneDistance";
        (*property_map)[0x84272077] = "m_fWarnOffPathDistance";
        (*property_map)[0x22442E1C] = "m_pMessage";
        (*property_map)[0x6A4B8C19] = "m_ePose";
        (*property_map)[0xA131FB0] = "m_fMinWarnDelay";
        (*property_map)[0x41701353] = "m_sRigidbodyImpactsDynamicSwitch";
        (*property_map)[0x6F521149] = "m_vBodypartBoundingMarginMin";
        (*property_map)[0xEDE865B5] = "m_bApplySpeedMultiplier";
        (*property_map)[0x3B234037] = "m_GestureForwardAct";
        (*property_map)[0x935024A8] = "m_fRangeFarBonus";
        (*property_map)[0xB294B64D] = "m_fWorldScale";
        (*property_map)[0x26F8E097] = "m_nOwnerID";
        (*property_map)[0xEA03820A] = "m_fCombatPreferredDistanceFromThreatAggressive";
        (*property_map)[0x2E4E7DFE] = "m_fResetTransitionTime";
        (*property_map)[0xE9984CE2] = "m_maxTension";
        (*property_map)[0xC548F0E2] = "m_pExplosion";
        (*property_map)[0x609A268D] = "m_rExitSequence";
        (*property_map)[0xBB94D0B0] = "m_iBulletsPerMagazineBonus";
        (*property_map)[0xBFB3FEA1] = "m_bTestDeadActors";
        (*property_map)[0x3B45B009] = "m_GestureLeftAct";
        (*property_map)[0xACA5E32E] = "m_bJailUsher";
        (*property_map)[0xEB2AEC5E] = "m_eInputAction";
        (*property_map)[0x79E8EE0C] = "m_nOutfitVariation";
        (*property_map)[0xDAA0D219] = "m_vListenerLinearVelocity";
        (*property_map)[0xB28D6E04] = "m_rEscortTarget";
        (*property_map)[0x67877E2] = "m_bActivatedByIActivatable";
        (*property_map)[0xA5AA3F08] = "m_bBlockIfAudioChannelUnavailable";
        (*property_map)[0x10B3A9C0] = "m_aIntermediateScreenplays";
        (*property_map)[0xEC01B46D] = "m_bPositionOnly";
        (*property_map)[0x6C0E5B74] = "m_fVelocityFactor";
        (*property_map)[0xCD90EFC9] = "m_KeeperProfessions";
        (*property_map)[0xB7201B9B] = "m_rAnimVariationResource";
        (*property_map)[0x44B90171] = "m_ConditionList";
        (*property_map)[0xA5B19DC2] = "m_fPropagationLOSSpreadPerSecond";
        (*property_map)[0xEB73072C] = "m_IsHiddenInVR";
        (*property_map)[0x4B0E6461] = "m_ExcludedFromDeepTrespassingProfessions";
        (*property_map)[0x56E2632F] = "m_sPromptText";
        (*property_map)[0x74F7655A] = "m_fRadiusYBottom";
        (*property_map)[0x231EC5F2] = "m_bWokenUpInTheGroup";
        (*property_map)[0x45D97D43] = "m_bTargetSlowsDownWhenOutOfRange";
        (*property_map)[0x724259B5] = "m_pReverseAct";
        (*property_map)[0x8CCDE04F] = "m_fSpeedUpDistance";
        (*property_map)[0xA8B2A4F8] = "m_bLerpColorCorrection";
        (*property_map)[0xB40CF08E] = "m_fAngularVelocityOutAngle";
        (*property_map)[0xC0541B25] = "m_fReticleLegendOffsetMouse";
        (*property_map)[0x9EC578B3] = "m_fTargetPathOffsetMin";
        (*property_map)[0xF10DAAC8] = "m_View_pctScale";
        (*property_map)[0x4E2CFBE3] = "m_fCompareWith";
        (*property_map)[0x67F0A992] = "m_fMaxDistanceToIntermediates";
        (*property_map)[0xBA9E9F3D] = "m_nWidthTessellation";
        (*property_map)[0x1F5E8B67] = "m_fPowerCrop";
        (*property_map)[0x754EA840] = "m_bIsComplete";
        (*property_map)[0xE77A084] = "m_pActEscortTemplate";
        (*property_map)[0xD0707029] = "m_bEmitFrozen";
        (*property_map)[0xBD2CF4AF] = "m_aCachedAvailableSaves";
        (*property_map)[0xA639098C] = "m_pCamera";
        (*property_map)[0x3CFFF920] = "__dummy";
        (*property_map)[0x3E3E5B1D] = "m_fFontHeight";
        (*property_map)[0x191405B4] = "m_events";
        (*property_map)[0x88AF92EE] = "m_RequiredItem";
        (*property_map)[0xA023340C] = "m_fQ";
        (*property_map)[0x233DF04B] = "waitTime";
        (*property_map)[0xD35A983C] = "AddAgentData_NPC_Pool_SaveData";
        (*property_map)[0xEF04A02E] = "m_NumSlash1HEmitters";
        (*property_map)[0xE0C6C08] = "m_bDetectShots";
        (*property_map)[0xAD9B8A68] = "m_nRuns";
        (*property_map)[0x525BDC1B] = "m_AIAreas";
        (*property_map)[0x974E67ED] = "m_PlaybackAttenuationCurve";
        (*property_map)[0xDCC8541F] = "m_pTextureRID";
        (*property_map)[0x41AB81CC] = "m_fStreamInTime";
        (*property_map)[0x49D3670A] = "m_tLastDisturbance";
        (*property_map)[0x41F6E668] = "m_fBodyPartModifierTorso";
        (*property_map)[0xC1D42460] = "m_aActorData";
        (*property_map)[0x195F3AD1] = "m_rZ";
        (*property_map)[0x9EA4F869] = "m_PatternSequenceData";
        (*property_map)[0xA8295C92] = "ElevationDifferenceRange";
        (*property_map)[0x31E76FA] = "m_nGlowPlayerSeenInLVAFadeSpeed";
        (*property_map)[0x811519C6] = "m_bIncludeItemsNotVisibleInInventory";
        (*property_map)[0xEDCAF7C7] = "m_bCombineAIModifierScopes";
        (*property_map)[0xE13C6286] = "m_bExlusiveRoomTest";
        (*property_map)[0x553089D3] = "m_rSource";
        (*property_map)[0xB8EC5145] = "m_fMaxAlternativeShootTargetDistance";
        (*property_map)[0xCD0D5AFA] = "m_AmmoProperties";
        (*property_map)[0x4CD5D95C] = "m_rTarget";
        (*property_map)[0x876AB227] = "m_bUseSpots";
        (*property_map)[0x1EF6D5E5] = "m_fInvestigateCautiousPropagationLOSMaxDistance";
        (*property_map)[0x57C7BC38] = "m_pNavigationRoot";
        (*property_map)[0x8550A657] = "m_nNumColumns";
        (*property_map)[0xAF132202] = "m_TriggerOnThrownItem";
        (*property_map)[0x2492ADD7] = "m_rSafeRoomNode";
        (*property_map)[0xCD4969BB] = "m_AgilityPeripheralAttentionDuration";
        (*property_map)[0x724522F3] = "m_fTime";
        (*property_map)[0x6AF88F67] = "m_bSilentInvestigation";
        (*property_map)[0xEA6E0B89] = "m_vVelocityOut";
        (*property_map)[0x30B0D1C2] = "bVisiblyArmed";
        (*property_map)[0x68CDCA3] = "m_fAngularVelocityInAngle";
        (*property_map)[0x50840C56] = "m_bDrawScatter";
        (*property_map)[0xC8FFDD56] = "m_rAimTarget";
        (*property_map)[0x20581B9B] = "m_fFOV";
        (*property_map)[0x8B1C08EF] = "includeWeapons";
        (*property_map)[0xEE22509A] = "m_pHUDVsCrateTimerController";
        (*property_map)[0x2C731405] = "m_tGreetingCooldown";
        (*property_map)[0x276B04A] = "m_vAngularVelocityOutAxis";
        (*property_map)[0xF2D591D] = "m_bRespondToExplosions";
        (*property_map)[0x191DF84C] = "m_rSlash1HDecal";
        (*property_map)[0xC4ABA4D9] = "m_nBodyIndex";
        (*property_map)[0x94D22C1B] = "m_pRuntimeNetworkResource";
        (*property_map)[0xA61C6887] = "m_bHarmlessImpact";
        (*property_map)[0xE94AA583] = "m_eCenterOfMass";
        (*property_map)[0x15D7F9E] = "m_rFaceTarget";
        (*property_map)[0xC40A0DB4] = "m_rBaseCharacter";
        (*property_map)[0x356A9D27] = "vMax";
        (*property_map)[0x1A129C39] = "m_pHelper";
        (*property_map)[0x98844DD7] = "m_pSpatialGhostFactory";
        (*property_map)[0xEB1A32AD] = "m_curve0";
        (*property_map)[0xFA7F4DAA] = "m_bFlipSurfaceNormal";
        (*property_map)[0xE06C207] = "m_IDHigh";
        (*property_map)[0xABD88BD5] = "m_fSpeedMultiplier";
        (*property_map)[0xB0F8D6AF] = "ZAxis";
        (*property_map)[0xF1936780] = "m_UseableCondition";
        (*property_map)[0x14053CB0] = "m_bAutoGet";
        (*property_map)[0x21F2558E] = "m_bAllowedRestore";
        (*property_map)[0xE84B49D8] = "m_fMargain";
        (*property_map)[0x86D62B9D] = "m_rCheck";
        (*property_map)[0x322F4A0F] = "m_fMaxTranslationAlongGravity";
        (*property_map)[0xFE2F95DA] = "m_aHandColliders";
        (*property_map)[0x618781B3] = "m_aDisguisesFriskExempt";
        (*property_map)[0x9CAB3518] = "m_fRagdollImpactUpKickGlobal";
        (*property_map)[0xD72FEB33] = "m_fLLegDamageScalar";
        (*property_map)[0xA0DC0513] = "m_aScopePath";
        (*property_map)[0x86282D55] = "m_fDownsteamGain";
        (*property_map)[0x5A322C40] = "m_sTitle";
        (*property_map)[0xB6FAC058] = "m_OutfitEntity";
        (*property_map)[0xE0D1CED7] = "m_rRelocateTarget1";
        (*property_map)[0xF1811CA] = "m_fRagdollImpactUpKick";
        (*property_map)[0x20EB77CE] = "m_pBoxReflectionCache";
        (*property_map)[0x2345523A] = "m_bIsPrivate";
        (*property_map)[0xEE749165] = "m_rChannelDummy";
        (*property_map)[0xD233FFF] = "m_bCheatSpawnGroup_Standard";
        (*property_map)[0x8EB16273] = "stoppingDistance";
        (*property_map)[0xD1A35AC] = "m_nMaxRifleAmmo";
        (*property_map)[0x1CC22AE6] = "m_materialArch";
        (*property_map)[0x447556F9] = "actor";
        (*property_map)[0x8C0F0188] = "m_rReporter";
        (*property_map)[0x573C566F] = "m_MuzzleFlashEffect";
        (*property_map)[0x6C127AE0] = "m_bIsStopping";
        (*property_map)[0xA2CB4C19] = "m_fMapHitTestThreshold";
        (*property_map)[0x262D5665] = "m_eVariationResourceMaxEmotionState";
        (*property_map)[0xC78E5D54] = "m_AmmoType";
        (*property_map)[0x7F461E95] = "m_pSpecialEntity";
        (*property_map)[0x51210139] = "m_Tags";
        (*property_map)[0x8E3B61DD] = "m_fSpringBounceOffsetSmall";
        (*property_map)[0xBD8DB182] = "m_fMinDuration";
        (*property_map)[0xCEC669D] = "m_bAcknowledged";
        (*property_map)[0xF2455450] = "m_eOption";
        (*property_map)[0xE289C615] = "m_Definition";
        (*property_map)[0x7180CA8D] = "m_fMaxDuration";
        (*property_map)[0x188C9137] = "m_pSharedKnowledge";
        (*property_map)[0xDC749FC9] = "m_bIsDone";
        (*property_map)[0xA4F9DD21] = "m_bValue";
        (*property_map)[0x62858676] = "m_rOpportunity";
        (*property_map)[0x5B57EC59] = "m_GlowEnemiesOutlineColor";
        (*property_map)[0x396E6680] = "m_bUseDistanceScale";
        (*property_map)[0xEDB59F9C] = "m_eActorRank";
        (*property_map)[0xBC90A79A] = "m_fMinDelay";
        (*property_map)[0xC929D5EE] = "m_nNumOutputChannels";
        (*property_map)[0x6347A095] = "m_bTargetDead";
        (*property_map)[0xFCFFD884] = "bIsVR";
        (*property_map)[0x6E5F0090] = "m_fWaitAfterHandledDuration";
        (*property_map)[0x70C640BB] = "m_rGuide";
        (*property_map)[0xC819E325] = "m_GlowContractTargetColor";
        (*property_map)[0x44CA7356] = "m_rTargetLocation";
        (*property_map)[0x4E51209E] = "m_bBystanderFieldsFailed";
        (*property_map)[0xB11E817F] = "m_eMoveOrderColor";
        (*property_map)[0x62E17169] = "m_sRightHandPositionOffset";
        (*property_map)[0xC62BF56F] = "m_eLeavePhase";
        (*property_map)[0xB90D49EC] = "m_MuzzleExit";
        (*property_map)[0x34447429] = "m_fMaxCLPTime";
        (*property_map)[0x8A1238B9] = "m_rSoundEvent_Footstep_Vegetation_LVA";
        (*property_map)[0x720D2C3A] = "m_rDrama";
        (*property_map)[0x4E5D2961] = "m_vSubPosition0";
        (*property_map)[0x82C96A9B] = "m_nParticleFxSnap";
        (*property_map)[0x8C4F6825] = "m_rKeyword";
        (*property_map)[0xF1007D92] = "m_MainTextLine";
        (*property_map)[0xD44F5005] = "m_rTransitionOperator";
        (*property_map)[0x9BCFDF91] = "m_bTestAliveActors";
        (*property_map)[0xA053484D] = "m_vSubPosition2";
        (*property_map)[0xED1B56AC] = "m_GameCamControls";
        (*property_map)[0x957913F1] = "m_aControls";
        (*property_map)[0xA20553DF] = "m_fMaxSightDistance";
        (*property_map)[0x3B195AFF] = "m_IsChangeHandlingEnabledOnStart";
        (*property_map)[0xC04708BE] = "m_BoneName";
        (*property_map)[0x72B80B82] = "m_fSubAngle0";
        (*property_map)[0x482B30E2] = "m_nResetCount";
        (*property_map)[0x6293513B] = "m_bFastWakeUp";
        (*property_map)[0x34EA2B52] = "m_rContainsItem";
        (*property_map)[0x455136ED] = "m_ValueInt";
        (*property_map)[0x15A2A063] = "m_bCountsAsFiberWire";
        (*property_map)[0xE2F0EE7A] = "m_bLerpDistortionBarrel";
        (*property_map)[0x4075DD80] = "m_pMaterialOverrideResource2";
        (*property_map)[0xB3E4CAB9] = "m_bSignalOnActivate";
        (*property_map)[0xB91AA170] = "from";
        (*property_map)[0x99565D00] = "m_fStatsFireRateDelta";
        (*property_map)[0xD87028CE] = "m_fPitchMinOffset";
        (*property_map)[0x9F4B66E2] = "m_sNotificationBody";
        (*property_map)[0xF2C4A947] = "m_bUseTargetSteering";
        (*property_map)[0x9CB66AAE] = "m_fSubAngle2";
        (*property_map)[0x71BE5D85] = "m_bNPCShootOverCover";
        (*property_map)[0xBA801B8C] = "m_bPureWater";
        (*property_map)[0x8090FD68] = "m_nNumDeaths";
        (*property_map)[0x2F253BBA] = "bExplosive";
        (*property_map)[0xC0370CF0] = "m_fOpenCloseChangeTime";
        (*property_map)[0xBCA64B28] = "m_fSettingDurationVariance";
        (*property_map)[0x23F3BDD3] = "m_nNumScared";
        (*property_map)[0xBE630AE5] = "m_nRefCount";
        (*property_map)[0xDFD3035D] = "m_waypoints";
        (*property_map)[0x1935A622] = "WaveFunction";
        (*property_map)[0x59321ED] = "m_bCrop";
        (*property_map)[0x7B24031E] = "m_bNotSelfieMode";
        (*property_map)[0x17EB0A5A] = "propertyAliases";
        (*property_map)[0x361F66B9] = "m_bWarzoneSpawned";
        (*property_map)[0xAC480846] = "m_eIllegalItemEval";
        (*property_map)[0x8915752] = "m_pDetectedInPrivateGroup";
        (*property_map)[0x9FAF7FF1] = "m_RayLayer";
        (*property_map)[0x163DE79C] = "m_rAct";
        (*property_map)[0x7F7D4360] = "m_fDamageCloseBonus";
        (*property_map)[0xE19358D9] = "m_aDetachmentEffect";
        (*property_map)[0xCC3FE59D] = "m_bUseSkinningNegNormalConstrains";
        (*property_map)[0xBA12B46E] = "m_GhostFadeStart";
        (*property_map)[0x631FB64E] = "m_bUsePerVertexStiffness";
        (*property_map)[0x7CC4F7D3] = "m_fMoveStrength";
        (*property_map)[0x738BC30D] = "m_fTravelDistanceInterpolationThreshold";
        (*property_map)[0xF4523B35] = "rCrosshairLocation";
        (*property_map)[0x8C03B94E] = "m_rApplyAnimVariationCondition";
        (*property_map)[0x1E994CA9] = "m_pEventConsumerCollection";
        (*property_map)[0xDCF925DF] = "m_pDefaultIntroMovie";
        (*property_map)[0x24B0DBAC] = "warningText";
        (*property_map)[0x132EC608] = "m_bDisableLeftHandGrip";
        (*property_map)[0x4780C470] = "m_bUsePerVertexDamping";
        (*property_map)[0x8C0BE0AB] = "m_linkedEntity";
        (*property_map)[0xD907A19] = "m_aEffectorManipulators";
        (*property_map)[0x294DB78B] = "m_vPacifyRadii";
        (*property_map)[0xC708196E] = "m_fAnalogPrecisionModeThreshold";
        (*property_map)[0xAA4919FC] = "m_bWrapIndex";
        (*property_map)[0x82E90A65] = "m_rSoundEvent_BulletImpact";
        (*property_map)[0xF709A468] = "m_bIsFunctional";
        (*property_map)[0x7F6EBF95] = "m_CarryOnBack_widget";
        (*property_map)[0x793A52B8] = "m_eHasInputEval";
        (*property_map)[0x4C181215] = "m_bDestroyed";
        (*property_map)[0x480760AF] = "m_curveGreen";
        (*property_map)[0xB01F3C73] = "entityTypeResourceIndex";
        (*property_map)[0x1747C67] = "m_bHasRunData";
        (*property_map)[0x71206441] = "propertyValues";
        (*property_map)[0x7BB54CD8] = "platformSpecificPropertyValues";
        (*property_map)[0x8AE08784] = "m_aHolders";
        (*property_map)[0x9D2D1E8B] = "m_fCombatAge";
        (*property_map)[0x42190F15] = "m_pBonesAndCollisionResource";
        (*property_map)[0xDC933FAB] = "m_aBodyParts";
        (*property_map)[0x93BF3F6F] = "m_bAllowPausedDramesToBeActive";
        (*property_map)[0x535F2E10] = "m_vBodypartBoundingMarginMax";
        (*property_map)[0x4665975A] = "m_bHeroDumpedBodyHidden";
        (*property_map)[0xD36D6258] = "m_fLongOffset";
        (*property_map)[0xFB1E8091] = "m_aPostFilterParametersData";
        (*property_map)[0xFE23F07F] = "m_BoneAnimator";
        (*property_map)[0xF6F7F407] = "m_aOutputPipes";
        (*property_map)[0xC89330E4] = "m_fReloadCooldown";
        (*property_map)[0xBB20E565] = "m_CompareMethod";
        (*property_map)[0xED9A08A6] = "m_bFixYAxis";
        (*property_map)[0xFC38B9EC] = "m_bExemptFromDamage";
        (*property_map)[0xE976CAFD] = "m_bAmbient";
        (*property_map)[0x8C64F6C8] = "m_fReloadTimeSafetyTimeout";
        (*property_map)[0xEA4C5B29] = "m_aActorProviders";
        (*property_map)[0x20417574] = "m_fProximityTriggerDistance";
        (*property_map)[0xFBD8E7CA] = "m_pTextLine";
        (*property_map)[0x7288A388] = "m_activeHint";
        (*property_map)[0x3041782E] = "sFontLocaleID";
        (*property_map)[0x40E788D8] = "m_aQueue";
        (*property_map)[0x499E8D35] = "m_nMaxSizeScale";
        (*property_map)[0xE49EECBD] = "m_rSelectedIndex";
        (*property_map)[0xA24A01E0] = "m_aSetActions";
        (*property_map)[0x4FDF2408] = "m_localOnly";
        (*property_map)[0x9D1991D3] = "m_fMaxEventDistance";
        (*property_map)[0x284DF55C] = "m_rLeaveSequence";
        (*property_map)[0xF758AC7F] = "YAxis";
        (*property_map)[0xBC90F823] = "m_bReachedSignalSent";
        (*property_map)[0xF2D46845] = "m_pLinkedEntity";
        (*property_map)[0x8291CE6E] = "m_eMaxActorEmotionState";
        (*property_map)[0x40E039E0] = "m_fVelocityContributionFactor";
        (*property_map)[0x5280FA01] = "m_bAlignToVelocity";
        (*property_map)[0x91BDFB4] = "m_rSpatial1";
        (*property_map)[0xFDFBD3EC] = "m_bActor";
        (*property_map)[0x41922F5D] = "m_bAllowCrowdBlendin";
        (*property_map)[0xCD79C1A0] = "m_bEnablePitchLevelOut";
        (*property_map)[0xEC2D9ACA] = "situation";
        (*property_map)[0x10D5F163] = "m_bDisableCSMLODOffset";
        (*property_map)[0x33C083E8] = "m_bValidNormal";
        (*property_map)[0x4FE3C78F] = "m_fWaitAfterHandledDurationVariance";
        (*property_map)[0x68B6552] = "m_prevState";
        (*property_map)[0x5252A187] = "m_eUseType";
        (*property_map)[0x55C5416F] = "m_eRequiredEmotionState";
        (*property_map)[0x2433BB3C] = "m_pSoundMaterialDescriptor";
        (*property_map)[0xF3E9E26E] = "m_cPrimeToIdleCurve";
        (*property_map)[0xAA29E6DD] = "fMaxAge";
        (*property_map)[0xED6AFD8A] = "m_pItem";
        (*property_map)[0x97A18808] = "m_startAnimationTime";
        (*property_map)[0x490E0198] = "m_bLoop";
        (*property_map)[0xDB4AC875] = "m_pTarget";
        (*property_map)[0x5431FA5B] = "EventMetaData";
        (*property_map)[0xD2EDD47B] = "m_triggerIfSpanningOverride";
        (*property_map)[0x8A0F5926] = "m_fCellIntersectMargin";
        (*property_map)[0x23ACE592] = "m_nFlowWidth";
        (*property_map)[0x2306C6CD] = "m_nMaxLeaveDelaySec";
        (*property_map)[0x4653A9FE] = "m_sweetSecondary";
        (*property_map)[0x1C3E6E34] = "m_fCartridgeEjectForceMultiplier";
        (*property_map)[0x24E8BCB0] = "m_bNPCUseEnable";
        (*property_map)[0x2186B001] = "m_Item";
        (*property_map)[0xDC55256E] = "m_bShowOnMiniMap";
        (*property_map)[0xBDB78D3F] = "bWeaponSilenced";
        (*property_map)[0x1BE27F83] = "m_fScaleRange";
        (*property_map)[0x6C332E08] = "showDuration";
        (*property_map)[0xC8BBA092] = "m_pHitBody";
        (*property_map)[0x7640CD60] = "m_bBlurBufferEarly";
        (*property_map)[0x6A743500] = "m_bPlaceItem";
        (*property_map)[0xC7B70792] = "m_nMaxGunAmmo";
        (*property_map)[0x44418E36] = "m_tLastBackupGroupEnded";
        (*property_map)[0xC08E0A4D] = "m_bPrintRecursively";
        (*property_map)[0x6EC87D04] = "m_nAmmoCountShotgun";
        (*property_map)[0x523FEC48] = "m_rDramaRole";
        (*property_map)[0x3FC6AE18] = "m_bIsLegacyLayoutActive";
        (*property_map)[0x159D82A9] = "m_nMaxRevolverAmmo";
        (*property_map)[0xE18AC8EF] = "m_GamePostfilterControllerEntity";
        (*property_map)[0xFED921DA] = "m_bVRNoAlignment";
        (*property_map)[0x38D9AE4D] = "m_bFirskRequestRepeated";
        (*property_map)[0xC225AD46] = "m_pSender";
        (*property_map)[0x93838391] = "m_nMaxShotgunAmmo";
        (*property_map)[0x886B383A] = "m_TypeExplodeableItem";
        (*property_map)[0x42DECF9C] = "m_nMaxTranqAmmo";
        (*property_map)[0x2F85B191] = "m_nMaxShotgunBeanbagAmmo";
        (*property_map)[0x1091C259] = "m_fInitialRotationDeviation";
        (*property_map)[0x3F93BA0E] = "_h";
        (*property_map)[0x8E7FA198] = "m_pDecalMesh";
        (*property_map)[0xDED79D44] = "m_nIterations";
        (*property_map)[0x79F4CDCE] = "m_fInterpolationTime";
        (*property_map)[0xCC18B48E] = "m_bInterpolateOnlyWhileMoving";
        (*property_map)[0x9D0ACF5B] = "m_nGlowInteractionSelectedFadeSpeed";
        (*property_map)[0x5C07DB09] = "m_rSituation";
        (*property_map)[0x61F6A958] = "m_Condition";
        (*property_map)[0xB3A7FE46] = "m_bAffectsDontEscalateOnLineCrossingDisguises";
        (*property_map)[0xB76D6F8E] = "m_pMaterialOrgResource2";
        (*property_map)[0x3F45959C] = "m_VisibleCondition";
        (*property_map)[0xF1DF013A] = "m_Holster_widget";
        (*property_map)[0xC50DD958] = "m_fSSRSharpenTopBottom";
        (*property_map)[0xAB9B08EF] = "m_uiModRole";
        (*property_map)[0x2CEC5F48] = "m_vModulation";
        (*property_map)[0x14DFE03E] = "m_fGlowStashedGunsOutlineWidth";
        (*property_map)[0x61283D4] = "m_pMaterialRagdollBody";
        (*property_map)[0x30E7307A] = "m_sPromptDescriptionText";
        (*property_map)[0x6BD3670] = "m_rKeyboardPromptTextOverrideResource";
        (*property_map)[0xB0744136] = "m_bHelpOrderAssigned";
        (*property_map)[0xCC8860E4] = "m_rPs4PromptTextOverrideResource";
        (*property_map)[0x601F7B7F] = "m_bSoundCuriousEnabled";
        (*property_map)[0x6EE5D43E] = "m_rXBoxOnePromptTextOverrideResource";
        (*property_map)[0xD13B6DD7] = "m_vDragAngles";
        (*property_map)[0x24925975] = "tTime";
        (*property_map)[0x4CB1C3B] = "FlagScaleY";
        (*property_map)[0x868CE9B1] = "m_rVROverrideSpatial";
        (*property_map)[0xFB03FD6F] = "m_bCanBeOperatedWithItemsInHand";
        (*property_map)[0x4A88C41C] = "m_bUseOrientedPrompts";
        (*property_map)[0xD02B47D4] = "m_nWarningCount";
        (*property_map)[0xBB93BD56] = "m_nDurationTicks";
        (*property_map)[0xD13AF3EF] = "m_eLedgeEndState";
        (*property_map)[0xB7EF7BF2] = "m_bDebugDrawSplines";
        (*property_map)[0x6DC3EF1] = "m_bKeepPosOnLedge";
        (*property_map)[0x5339B946] = "m_bCheckLeftHand";
        (*property_map)[0x6304F4D6] = "m_glowColor1";
        (*property_map)[0xC70F51CE] = "m_bUseWorldsDown";
        (*property_map)[0x2EEC111E] = "m_eDebugTensionState";
        (*property_map)[0x4BCB6F70] = "m_aPrevDramaSituations";
        (*property_map)[0xF93F7C07] = "m_bCornerRightEnabled";
        (*property_map)[0xE5A215BD] = "m_eWantedSpeed";
        (*property_map)[0x61653A9F] = "m_bPacifyPushVictim";
        (*property_map)[0x96F0FA67] = "m_fPropagationLOSMinDistance";
        (*property_map)[0xED88A6DE] = "m_nEventID";
        (*property_map)[0x45B93FCC] = "nImpactBodyPart";
        (*property_map)[0x4AB54E62] = "m_ParticleCollider";
        (*property_map)[0x4B114F52] = "m_DramaActorData";
        (*property_map)[0x2F3E169] = "m_fTargetStrength";
        (*property_map)[0xFE3A5DDC] = "isContainer";
        (*property_map)[0x7C5BA6FE] = "m_eWantedGait";
        (*property_map)[0x70756FB5] = "m_eResetTransitionCurve";
        (*property_map)[0xB805966E] = "m_bGreetedInstruction";
        (*property_map)[0x644CC57A] = "m_fReferenceValue";
        (*property_map)[0x4673E602] = "m_pLegendView";
        (*property_map)[0x8CFC5572] = "m_nAnimSourceID";
        (*property_map)[0x1DF638CC] = "m_nAnimSourceIndex";
        (*property_map)[0x4C017765] = "EventIndex";
        (*property_map)[0xFF2B89F4] = "m_bOpponent";
        (*property_map)[0xE1195483] = "m_vPosition";
        (*property_map)[0xE0D99C30] = "m_bTrajectoryEnabled";
        (*property_map)[0x3D4911F4] = "m_aCastList";
        (*property_map)[0xEF7CC502] = "m_pSpeaker";
        (*property_map)[0x11F7B428] = "m_VertexPaintData";
        (*property_map)[0x17964AC1] = "m_tNextStanddownTime";
        (*property_map)[0x651DE496] = "m_fRadiusY";
        (*property_map)[0x2D3939F5] = "m_fSpeed";
        (*property_map)[0xC24458A7] = "m_bStartAttached";
        (*property_map)[0x59486E90] = "m_eMood";
        (*property_map)[0xCC063DB4] = "m_fNearRadius";
        (*property_map)[0x170E482E] = "m_aCrowdAiPoolActorsFemale";
        (*property_map)[0xF4F9C52B] = "FlagPositionY";
        (*property_map)[0x5B90052F] = "m_fPs3SplashSpeedScaleMin";
        (*property_map)[0xA0AFBE54] = "m_fScreenDecalsColorScale";
        (*property_map)[0x818E0C44] = "m_MuzzleFlashEffectGroup";
        (*property_map)[0x82F9C8F0] = "m_CompletedStringID";
        (*property_map)[0x33AB1744] = "m_rInitialNode";
        (*property_map)[0xC290BE1B] = "m_eHeadAnchorMode";
        (*property_map)[0xE7C99B1A] = "m_fVignetteGradientLightSize";
        (*property_map)[0x186B2A7E] = "m_rDefaultVoiceVariations";
        (*property_map)[0x216913C6] = "m_ConversationParts";
        (*property_map)[0x16F5B125] = "m_tLastTimeGuardHadLOS";
        (*property_map)[0xD6A8AFC2] = "m_eHitmanGreetState";
        (*property_map)[0x4FD28E18] = "m_nObjectivesCounterDeactivate";
        (*property_map)[0x38A91D33] = "m_sCommonName";
        (*property_map)[0x9214A72D] = "m_Actors";
        (*property_map)[0x4C0AF3E1] = "m_PosLeftPrimingHandAttach";
        (*property_map)[0xBB930157] = "nFourCC";
        (*property_map)[0xBE00E5DB] = "m_eSoundType";
        (*property_map)[0xB2CBC178] = "m_pParentOutfit";
        (*property_map)[0xBFEC3295] = "m_eGlowOverride";
        (*property_map)[0x234BE06] = "m_fNPCMaxDistanceToHitman";
        (*property_map)[0xA42109CC] = "m_tLastTauntTime";
        (*property_map)[0xDD0523BD] = "m_bIsFemale";
        (*property_map)[0x20939E3D] = "m_bHeroDisguiseAvailable";
        (*property_map)[0xEAFA3431] = "m_bLerpDistortionWobble";
        (*property_map)[0x6237DB69] = "m_vContactPoint";
        (*property_map)[0xD5CAE537] = "stype";
        (*property_map)[0xFE911260] = "m_bAllowRadioAct";
        (*property_map)[0xD98A3257] = "m_fHitPoints";
        (*property_map)[0x961B7F3D] = "m_bDefaultFadeToBlackDuringTransition";
        (*property_map)[0x3B02681A] = "m_DamageMultipliers";
        (*property_map)[0x6A8AF431] = "m_bLerpSSAO";
        (*property_map)[0x6E674E2B] = "m_bWorldPostion";
        (*property_map)[0xC3BC32AB] = "m_rBodyContainer";
        (*property_map)[0x3E526BAD] = "m_pReticleLegendView";
        (*property_map)[0x31C6CB40] = "m_rValue2";
        (*property_map)[0xB73C8A07] = "m_bWasPlacedAndAttached";
        (*property_map)[0xA5C7CF41] = "m_nCollisionLayer";
        (*property_map)[0xD363D2D0] = "m_bResetCamera";
        (*property_map)[0x394DC8CA] = "m_eSoundFootwearType";
        (*property_map)[0xB3678563] = "m_bCanCallForHelp";
        (*property_map)[0x5F0C8004] = "m_pPhysicsSystem";
        (*property_map)[0xC1B38E97] = "m_fLinearVelocityDamping";
        (*property_map)[0xD4E10A22] = "sContractId";
        (*property_map)[0x88CB59ED] = "m_bAccidentShy";
        (*property_map)[0xD7F3E376] = "m_vScaleResolutionBase";
        (*property_map)[0xF88A07EE] = "m_fLateralMarginLeft";
        (*property_map)[0xBD0D2DBC] = "rtEditable";
        (*property_map)[0xA36CBA32] = "m_rTraversalCheck";
        (*property_map)[0x53F4EF6D] = "m_eResizeMode";
        (*property_map)[0xA3B643BB] = "m_rSpecialImpactAct";
        (*property_map)[0xC6C3E7C5] = "m13";
        (*property_map)[0xE9122050] = "m_eActorType";
        (*property_map)[0x475961EF] = "m_rCCProfile";
        (*property_map)[0xF3FADAC2] = "m_pHuntProfile";
        (*property_map)[0xDB3CC51B] = "m_GlowTagFocusUntaggedColor";
        (*property_map)[0xF9A8F668] = "m_aActivityBoxes";
        (*property_map)[0x8B93AFF6] = "m_bShowChildrenOnFocused";
        (*property_map)[0x18F81126] = "CROWD_RAGDOLL";
        (*property_map)[0x431FA093] = "m_fConeAngleFadeRange";
        (*property_map)[0x6C1C8738] = "m_AvailableGrenadeKeys";
        (*property_map)[0xEFC5FD5] = "m_aAnimationSets";
        (*property_map)[0xCD943024] = "m_fTetherDistanceMax";
        (*property_map)[0xA29BE856] = "m_aDynamicTemplates";
        (*property_map)[0x5F5AFBD9] = "m_fMinimalDistance";
        (*property_map)[0x54136672] = "m_nBasePriority";
        (*property_map)[0xC97E3610] = "m_eOutfitAICategory";
        (*property_map)[0x8B6CAA17] = "AngleTrigger";
        (*property_map)[0x2D7864C3] = "m_fCooldownForAnyRand";
        (*property_map)[0x268997DA] = "exposedInterfaces";
        (*property_map)[0x78E40D73] = "m_rFormattingResource";
        (*property_map)[0x123F0031] = "m_eOutfitType";
        (*property_map)[0x38C79223] = "m_fMapMouseHitTestThreshold";
        (*property_map)[0x5188DF3C] = "m_nArmorRating";
        (*property_map)[0xEEEBFB12] = "m_fYawSpeed";
        (*property_map)[0x4C8FD952] = "m_fWindConstant";
        (*property_map)[0x32FB99F5] = "m_fHDRLensFlareThresholdA";
        (*property_map)[0x3AA9C8E4] = "m_aAllowedWeaponKeywords";
        (*property_map)[0xE3E9A79B] = "m_uiModItem";
        (*property_map)[0x6DFBCAF8] = "m_aWeaponsAllowedExceptionKeywords";
        (*property_map)[0x9474526C] = "comment";
        (*property_map)[0xB88AE0EF] = "m_vImpulse";
        (*property_map)[0xFE5FAC84] = "m_SpatialEntity";
        (*property_map)[0x5B83ADDC] = "m_fAISoundModifierValue";
        (*property_map)[0xA97ADDE7] = "m_fTravelDistanceInterpolationTime";
        (*property_map)[0xB77B0B3E] = "m_fTransSpring";
        (*property_map)[0x4EE6D091] = "m_bSignalsInEditMode";
        (*property_map)[0x12823C15] = "m_fSensitivity";
        (*property_map)[0xF1930FD8] = "m_bPauseMenuEnabled";
        (*property_map)[0x622921C9] = "m_bContextKeywordsExclude";
        (*property_map)[0x382B0168] = "m_fCSMDisableDistance";
        (*property_map)[0x38F26C12] = "m_bRespectTargetIntrovercy";
        (*property_map)[0x13C82CF9] = "m_distance";
        (*property_map)[0x3B8BA7C7] = "text";
        (*property_map)[0x4C7AD911] = "nConditionModifiers";
        (*property_map)[0x71064A5] = "m_fPositionWarningTime";
        (*property_map)[0x492B2C8] = "m_rEntity";
        (*property_map)[0x8D1FECD6] = "m_bInvertPeekMovementSequence";
        (*property_map)[0xE629AA2B] = "m_fCornerAngleElevatingThreshold";
        (*property_map)[0x36009024] = "m_pChannel";
        (*property_map)[0xFE3B7B82] = "m_bAllowActorGroupB";
        (*property_map)[0xCD3F8A8F] = "m_fPrecisionShotAimTime";
        (*property_map)[0x4E7FB65A] = "m_nSharedIndex";
        (*property_map)[0xD39A3951] = "m_pAccessoryItemPool";
        (*property_map)[0xFAD288BD] = "m_nBooleanEvents";
        (*property_map)[0xF6484642] = "m_knownWorldMatrix";
        (*property_map)[0xF3B24C91] = "m_bShouldSubscribeGeneralEvents";
        (*property_map)[0x2E631887] = "m_vGlowContractNonCriticalVignetteRadii";
        (*property_map)[0x6C401254] = "m_fWindMultiplier";
        (*property_map)[0x8DBD6A53] = "m_playerId";
        (*property_map)[0xD7AD80A] = "m_Properties";
        (*property_map)[0x2F94E0C6] = "m_tLastWorldMatrixUpdate";
        (*property_map)[0x5A95DC94] = "m_fParticleRefractScale";
        (*property_map)[0xED8C28F8] = "m_fPeripheralAngle";
        (*property_map)[0x9685B41] = "m_nGoals";
        (*property_map)[0xF6742D66] = "m_bUseSimpleRendering";
        (*property_map)[0xB468E34E] = "m_bIsPIP";
        (*property_map)[0x236AD573] = "m_rVRConfig";
        (*property_map)[0xF7B4DD14] = "m_rReturnNode";
        (*property_map)[0x85B5E5EB] = "m_rPickupBehavior";
        (*property_map)[0x5F436D3B] = "m_ScatterData";
        (*property_map)[0x5C4C2969] = "m_nPs3SplashMaxSpawn";
        (*property_map)[0xCF48A500] = "m_eContainedType";
        (*property_map)[0xA2572027] = "m_fRadius";
        (*property_map)[0x631A0677] = "m_DramaSituationData";
        (*property_map)[0x329CEE2F] = "Recievers";
        (*property_map)[0x6FFD46A1] = "m_bExportScreenshot";
        (*property_map)[0x999684D2] = "m_bBlurBufferPassive";
        (*property_map)[0x19E69416] = "Materials";
        (*property_map)[0xD7913651] = "m_eStretchConstrainType";
        (*property_map)[0xA9AAB6D0] = "m_Delegate";
        (*property_map)[0x6D2EE51] = "m_bHairTriggerKillRelevant";
        (*property_map)[0x6A2DA22D] = "m_fAnchorTension";
        (*property_map)[0x9E8C100E] = "m_bBanterCandidate";
        (*property_map)[0x27408643] = "m_pFontsEntity";
        (*property_map)[0x1B76005B] = "m_nRandomSeed";
        (*property_map)[0x6C6E0AD0] = "m_fExpirationFadeTime";
        (*property_map)[0xBA33027F] = "m_pVersusRacePiPGhostFactory";
        (*property_map)[0xF4B8603C] = "m_bUseTextureOcclusion";
        (*property_map)[0x53CDF28E] = "m_fCurveStartFractionSpine1";
        (*property_map)[0x552C676E] = "m_eBrush";
        (*property_map)[0xAA92DC5C] = "m_bStartIdle";
        (*property_map)[0x6A8C99A5] = "m_fAnchorAdvanceDistance";
        (*property_map)[0x57D02E97] = "m_bUseInventory";
        (*property_map)[0xAF66A5F7] = "m_nBrushIntensity";
        (*property_map)[0x8C6464FF] = "m_bShowUnusableAsGrayedInteraction";
        (*property_map)[0xD9287864] = "m_bAccident";
        (*property_map)[0xC11D693A] = "m_nBrushMaterial";
        (*property_map)[0x2A871EFC] = "m_eMovementType";
        (*property_map)[0xD2318036] = "m_fBrushRadius";
        (*property_map)[0x61A739E6] = "m_nReactionFarDelay2";
        (*property_map)[0x6B2A48DA] = "m_eCCD";
        (*property_map)[0xCB3926F3] = "m_bBrushFalloff";
        (*property_map)[0x8461B602] = "m_fCameraPitchLimitTriggerSpeed";
        (*property_map)[0x187A70B3] = "m_fCurveStartTimeSpine2";
        (*property_map)[0x56E76FD1] = "m_eCanvasSize";
        (*property_map)[0xFE73FCF2] = "m_eClearMode";
        (*property_map)[0x1C025D46] = "m_bIgnoreZeroDamageShots";
        (*property_map)[0x811D817A] = "m_rRootSpatial";
        (*property_map)[0x937AB034] = "character";
        (*property_map)[0xED7144DA] = "m_bRecordingEnabled";
        (*property_map)[0x41E3C860] = "m_bIsStartCondValid";
        (*property_map)[0xF4D4BC20] = "m_bInvertWholeConditionCheck";
        (*property_map)[0xD5FCF93B] = "m_sExpectedLabel";
        (*property_map)[0x5A90DAFB] = "m_bStopAnimationProgram";
        (*property_map)[0xB0E528AE] = "m_bPrintToTraceSink";
        (*property_map)[0xBE1C085D] = "m_BusConnectionType";
        (*property_map)[0x318A2F90] = "m_eInputNavigateLeft";
        (*property_map)[0x6D95FB0B] = "m_bAllowScopeTimeSlowdown";
        (*property_map)[0xAEE79C5D] = "m_desiredLookAtLocation";
        (*property_map)[0x7722D36A] = "m_ParticleHeir";
        (*property_map)[0x15FF03ED] = "m_MurderDisguiseAttentionDuration";
        (*property_map)[0x1A42833] = "m_pExpectedWeaponKey";
        (*property_map)[0x8A74CF5F] = "m_bCombatIcon";
        (*property_map)[0x12ECFB8B] = "m_bEscalate";
        (*property_map)[0x90187F5D] = "m_TrackingLocations";
        (*property_map)[0x905FF70E] = "m_bDebugCoarseSplines";
        (*property_map)[0xA4DF0A35] = "m_pExpectedWeaponKeys";
        (*property_map)[0x1E28F979] = "m_bLethalThrowRelevant";
        (*property_map)[0x97B84FA4] = "m_AnchorTransforms";
        (*property_map)[0x1A6074BE] = "m_bExpectedExplosion";
        (*property_map)[0xD704FFE4] = "m_bHeadShotRelevant";
        (*property_map)[0xA68DA49E] = "m_bExpectedHeadShot";
        (*property_map)[0x2728EE96] = "m_pPerfDebugView";
        (*property_map)[0xCF0B3376] = "m_nHasEntered";
        (*property_map)[0x10C51F24] = "m_bTagExactMatch";
        (*property_map)[0x9ED7B9AE] = "m_bDefaultHeroSpawn";
        (*property_map)[0x47864E25] = "m_allContacts";
        (*property_map)[0x9D0C9023] = "m_rMapControl";
        (*property_map)[0x38C7BD08] = "m_bDeathMarkedRelevant";
        (*property_map)[0xE748C922] = "m_bExpectedDeathMark";
        (*property_map)[0x184B4D98] = "m_tDeepTrespassingTime";
        (*property_map)[0xD6CB6C11] = "m_bExpectedPush";
        (*property_map)[0x76E0F901] = "m_bDisableHelpCivilian";
        (*property_map)[0x1DCC043F] = "m_aTrackedTargets";
        (*property_map)[0x7646ED2F] = "m_pAssistant";
        (*property_map)[0xA432C406] = "m_rSequenceEntity";
        (*property_map)[0xFF6F1B3B] = "m_bPullRelevant";
        (*property_map)[0x3030DBE1] = "m_fDamageModifier";
        (*property_map)[0x8EC2421E] = "m_bActorMustSeeHitman";
        (*property_map)[0x896DBC96] = "ACTOR_RAGDOLL";
        (*property_map)[0x1CFCA696] = "m_bExpectedPull";
        (*property_map)[0xDA7843D2] = "m_fCameraPitchLimitDivider";
        (*property_map)[0x59EA5EB6] = "m_nOutputSwitchIndex";
        (*property_map)[0x405358FA] = "m_bWeaponClassRelevant";
        (*property_map)[0x22EEA4D] = "m_nAge";
        (*property_map)[0x342CF09] = "m_bEnablePositionProjection";
        (*property_map)[0xE9FF8E59] = "m_bKilledByPropExpected";
        (*property_map)[0x68FB067] = "m_bIsCivilianRelevant";
        (*property_map)[0x702A354F] = "m_bAlignPosition";
        (*property_map)[0x21D8297] = "m_bActivatedByPinSignal";
        (*property_map)[0xC1D21229] = "fDynamicScaleAlpha";
        (*property_map)[0xE171D10B] = "m_sConfigVarName";
        (*property_map)[0xBAEAA98D] = "m_sBodyText";
        (*property_map)[0x6819FF1C] = "m_MathLerpsData_SVector4";
        (*property_map)[0xDE5DD984] = "m_rHitEntity";
        (*property_map)[0x19F9EA0C] = "m_fFriction";
        (*property_map)[0x235C0C11] = "m_ColorCurveEntity";
        (*property_map)[0xB73D9BA9] = "m_bPreparing";
        (*property_map)[0xE4D64A88] = "m_rHelpingGuardMember";
        (*property_map)[0x795C7F9A] = "m_bWeaponIsHidden";
        (*property_map)[0x8185B822] = "m_bIsCivilianExpected";
        (*property_map)[0x696E37B5] = "m_bFlipLeftNormal";
        (*property_map)[0x2AE04F9A] = "m_bIsTargetRelevant";
        (*property_map)[0x8926D264] = "m_ChestAvoidanceEnableFade";
        (*property_map)[0x55BA72AC] = "m41";
        (*property_map)[0xA744572] = "m_bUpperBody";
        (*property_map)[0x7F41D209] = "m_eAssistantState";
        (*property_map)[0xF2D19838] = "m_bRespondToShots";
        (*property_map)[0x1DA49D23] = "m_bPositiveX";
        (*property_map)[0x714A24F6] = "m_InventoryItemKeys";
        (*property_map)[0x97A422B8] = "m_bNegativeX";
        (*property_map)[0xE0A3122E] = "m_bNegativeY";
        (*property_map)[0x9E2C6328] = "PoolSize";
        (*property_map)[0xE3E03B6] = "m_nAmmoCountFake";
        (*property_map)[0xE86A9545] = "m_bDynamicRestLength";
        (*property_map)[0x41571068] = "m_bHighPriority";
        (*property_map)[0xF1557240] = "m_AskBeforeVRModeDeactivation";
        (*property_map)[0xF3AAFC0F] = "m_bPositiveZ";
        (*property_map)[0x8924D97C] = "m_aRoles";
        (*property_map)[0xA7746D16] = "m_fMaxAzimuthDistance";
        (*property_map)[0x29B20015] = "fScaleFactor";
        (*property_map)[0x6B551A4C] = "m_bBlendUpperBodyAtStart";
        (*property_map)[0xCB5FE779] = "m_fPuddleDamping";
        (*property_map)[0xE804CE4C] = "m_pGetHelpGroup";
        (*property_map)[0x3BFE6E47] = "m_bEnableTravelDistance";
        (*property_map)[0xDF465310] = "_b";
        (*property_map)[0x93D59086] = "m_aResources";
        (*property_map)[0x6F374FB4] = "m_GlobalLightOverrideBackColor";
        (*property_map)[0xCEEC5041] = "m_rHeroRole";
        (*property_map)[0x61E8F3DD] = "m_bBillboard";
        (*property_map)[0x904A919A] = "m_AngleThreshold";
        (*property_map)[0x2FA69781] = "m_DiffuseColor";
        (*property_map)[0x1F1B251E] = "item";
        (*property_map)[0xDF51096C] = "m_fFaceDamageMultiplier";
        (*property_map)[0x60BA551E] = "StimulusTypeId";
        (*property_map)[0x3DD3AE61] = "m_aStartupInventory";
        (*property_map)[0x45F1A388] = "m_region";
        (*property_map)[0x1875FC0A] = "m_bAttachToAnchorTransform";
        (*property_map)[0xC18C4A7A] = "m_bPhysicsEnabled";
        (*property_map)[0xF7FC59A] = "rSource";
        (*property_map)[0xAF2CA79F] = "_g";
        (*property_map)[0x9BEF667] = "m_aStartConditions";
        (*property_map)[0x4425AD02] = "m_bSimulationEnabled";
        (*property_map)[0x92C62808] = "m_bEnableSmootingAtLimits";
        (*property_map)[0x690F0C28] = "m_bIgnoreWeapons";
        (*property_map)[0x84407828] = "m_fLinearDampening";
        (*property_map)[0xBC342F17] = "m_pAct";
        (*property_map)[0x965D585D] = "bInfiniteAmmo";
        (*property_map)[0xA56DAA2E] = "m_pTipsConfig";
        (*property_map)[0xE0274CED] = "m_arTargets";
        (*property_map)[0x1BE678B5] = "a0";
        (*property_map)[0x9ED725B4] = "m_rEffects";
        (*property_map)[0xBF81DDD3] = "m_fShearStiffness";
        (*property_map)[0x679D3A76] = "m_fPs3SplashSpeedScaleMax";
        (*property_map)[0x4D10526] = "distractionType";
        (*property_map)[0x3BB8880C] = "reason";
        (*property_map)[0xA55D5F16] = "m_bOverrideExistingAct";
        (*property_map)[0x71D6C8E0] = "m_tHitmanGreet";
        (*property_map)[0x135CB1CA] = "m_bNodeFilterGeom";
        (*property_map)[0x83B6B1FC] = "m_A";
        (*property_map)[0xBDC84B02] = "m_fMaxTime";
        (*property_map)[0x4D2750BB] = "m_bystanderPoints";
        (*property_map)[0x47E0A38B] = "m_bRemoveNeverFlee1";
        (*property_map)[0x7624FAF7] = "m_vLinkedPosition";
        (*property_map)[0x1ABFE046] = "m_B";
        (*property_map)[0x6FFC76D1] = "m_sVideoId";
        (*property_map)[0x11D8CF40] = "m_bHDRGlareEnabled";
        (*property_map)[0xE08DDC97] = "m_eRequiredActorTension";
        (*property_map)[0x8FCF2595] = "bIsTarget";
        (*property_map)[0xB3E5707C] = "m_pActor";
        (*property_map)[0xD28B062B] = "m_pDynamicObject";
        (*property_map)[0x49029512] = "m_pNPCFootstepEvent";
        (*property_map)[0xA9EEC2A7] = "m_bRemoveNeverFlee3";
        (*property_map)[0x3CD10499] = "m_bSendSignal";
        (*property_map)[0xE50F7A3E] = "m_fCapsuleAttacherFwdOffset";
        (*property_map)[0xC9CA206D] = "m_animationLibrary";
        (*property_map)[0x943A3B53] = "m_LinkedEntity";
        (*property_map)[0x4BAFEEF8] = "m_escalate";
        (*property_map)[0x2CFFDE21] = "m_fMaxPitchDeg";
        (*property_map)[0x45C3C359] = "m_pGuardDutyPoint";
        (*property_map)[0x3896E7D] = "m_pOperator";
        (*property_map)[0x24188B32] = "sContractTitle";
        (*property_map)[0x84DF0D33] = "m_pInteractionController";
        (*property_map)[0x51DAD501] = "m_tExpiry";
        (*property_map)[0xF6122611] = "m_animSets";
        (*property_map)[0xD33B1D5A] = "m_nValue";
        (*property_map)[0x37E76ECF] = "m_fXAxisSpeed";
        (*property_map)[0xBE491AD2] = "m_ZMaxDeviationAllowed";
        (*property_map)[0xD82505F1] = "m_fYAxisSpeed";
        (*property_map)[0xAEAE13EB] = "m_bStartAsleep";
        (*property_map)[0x6D2A6CF7] = "m_fCullDistanceZN";
        (*property_map)[0x76CCC7D1] = "m_nPriorityOffset";
        (*property_map)[0xF8EAE62B] = "m_DefaultStateCategory";
        (*property_map)[0x4FF0BA00] = "m_aHigherPriority";
        (*property_map)[0x34E7598F] = "m_bGlobalCollider";
        (*property_map)[0x919E6119] = "m_rVIPController";
        (*property_map)[0x31A67C0D] = "fNearScale";
        (*property_map)[0xCB8F38B6] = "m_pSubtitlesTextList";
        (*property_map)[0xD9CD5D6D] = "m_bSetDefaultAnimVariationsOnLeaveSituation";
        (*property_map)[0x45073193] = "m_body";
        (*property_map)[0x610D5131] = "m_fSettingCooldownEval";
        (*property_map)[0x3D142473] = "m_rItemCanTurnOff";
        (*property_map)[0x43AB8F4A] = "m_aHandled";
        (*property_map)[0x27A2DEA0] = "m_fTimeOutPendingDelay";
        (*property_map)[0xAE091F13] = "m_bDragBody";
        (*property_map)[0xB69CEB92] = "m_fMaxVerticalDistanceToIntermediates";
        (*property_map)[0x28D67D2] = "m_fShadowedMieDepthScale";
        (*property_map)[0xC2F764A5] = "m_fShootFromCoverTimerMin";
        (*property_map)[0x576F56C8] = "m_fOverrideTetherDistanceMin";
        (*property_map)[0x1640DEC] = "bIsReloading";
        (*property_map)[0x879452DB] = "eSaveType";
        (*property_map)[0xCECD0067] = "m_nAngle";
        (*property_map)[0x1757F16D] = "m_vGlowBackgroundUnmaskedVignetteRadii";
        (*property_map)[0x36BD8918] = "m_bStartFromBeginning";
        (*property_map)[0x1886DB12] = "legalType";
        (*property_map)[0x6B626991] = "m_fOverrideTetherDistanceMax";
        (*property_map)[0xF80E928E] = "StimulusTypeName";
        (*property_map)[0xD622F2CC] = "m_bRestoreAborted";
        (*property_map)[0x9EC489FF] = "m_aValueYEntities";
        (*property_map)[0x9417D869] = "m_nPs3SplashNumParticles";
        (*property_map)[0x4875F392] = "m_bSearchingForMorgue";
        (*property_map)[0x96821F27] = "m_rCombatArea";
        (*property_map)[0x2E620BCB] = "m_rCompleteCondition";
        (*property_map)[0x6E4E7CC6] = "m_nCooldown";
        (*property_map)[0x93E56DCD] = "maxTime";
        (*property_map)[0x4EA6833F] = "m_bDrawGates";
        (*property_map)[0xA0899598] = "m_eReactionBehaviorMode";
        (*property_map)[0x962276B0] = "m_bStartSearchOnTargetDead";
        (*property_map)[0xA6E56426] = "m_nGetHelpGroup";
        (*property_map)[0x83469852] = "m_rActAmbient";
        (*property_map)[0x2BD524F5] = "m_bPaused";
        (*property_map)[0xFE88C5E4] = "m_CrowdCoreIndex";
        (*property_map)[0x358EFCC0] = "m_eMarkerClipAlgorithm";
        (*property_map)[0x66748508] = "m_fStartSearchOnTargetDeadDelay";
        (*property_map)[0x4DD6E874] = "m_eTrackPointQueryType";
        (*property_map)[0x8047F684] = "m_rKeywordsToFind";
        (*property_map)[0xA04A2BFA] = "m_fModifier";
        (*property_map)[0xB3ED8A17] = "m_fEffectsStrength";
        (*property_map)[0x76C15066] = "m_fTextureBlockScale";
        (*property_map)[0xDF16DDA9] = "m_PenaltySpawnPointAtTrespassing";
        (*property_map)[0xC56B111A] = "m_bShouldSave";
        (*property_map)[0xE1D783D0] = "m_Spatial";
        (*property_map)[0xB6EBCF8D] = "m_pGeneric03Sound";
        (*property_map)[0xE9D78FE0] = "m_fHazardZoneRadius";
        (*property_map)[0xC5341BA7] = "m_bVisibleOnStart";
        (*property_map)[0xFC151319] = "otherslots";
        (*property_map)[0xB0072125] = "m_AxeCleaveEmitter";
        (*property_map)[0xED900E8D] = "Stimuli";
        (*property_map)[0x20AF0184] = "m_fParticleSizeMin";
        (*property_map)[0x4FCA6437] = "m_bUseSSR";
        (*property_map)[0xF1A073B6] = "m_aTypeBlacklist";
        (*property_map)[0x6A6DD679] = "m_fMinTetherRange";
        (*property_map)[0xABF2C84F] = "m_fHDRLensFlareThresholdB";
        (*property_map)[0x5A1B215E] = "m_bNotebookInventoryEnabled";
        (*property_map)[0xF3D15345] = "m_nSubSound";
        (*property_map)[0x34505FB] = "m_rAnimationSetDefinition";
        (*property_map)[0xCA4C8B4E] = "m_referenceObject";
        (*property_map)[0xA4750A92] = "m_eDrawLayer";
        (*property_map)[0x89089CB6] = "m_fWarnDistance";
        (*property_map)[0x9D607F02] = "m_sIconID";
        (*property_map)[0x5E56CC2C] = "m_bDecalsOnWeightedMeshes";
        (*property_map)[0xDFCC980] = "m_rCondition";
        (*property_map)[0xADDC5FB6] = "m_TextLineHasPrimaryUser";
        (*property_map)[0xCBECEF78] = "m_pLayoutEntity";
        (*property_map)[0xEB60D889] = "m_Color";
        (*property_map)[0x7AFF5532] = "m_bWireframe";
        (*property_map)[0xFD0EB95F] = "m_rBoxVolume";
        (*property_map)[0xEA8C40CB] = "m_bIsPerceptible";
        (*property_map)[0xC5AEC4BA] = "m_fTest";
        (*property_map)[0xC2178042] = "m_bRandomFlipU";
        (*property_map)[0x721A482F] = "m_aKillBoxes";
        (*property_map)[0x65289CB4] = "m_aLightData";
        (*property_map)[0xBCD78BCB] = "aSubStructArray";
        (*property_map)[0x336A1CE2] = "m_fGlowTagFocusUntaggedOutlineWidth";
        (*property_map)[0x8D74686E] = "m_GlowContractTargetSpecialUnconsciousColor";
        (*property_map)[0x5E6D0C7E] = "m_aMaterialData";
        (*property_map)[0xAE3D69EA] = "m_aBoneAttachData";
        (*property_map)[0xEB8F6B3B] = "m_eTensionLimit";
        (*property_map)[0xBB2F8B2B] = "m_bBlockDeadlyThrow";
        (*property_map)[0x4E7016E0] = "m_fProgressionDistance";
        (*property_map)[0x8CDADE68] = "m_vEmitDirection";
        (*property_map)[0xD05443DD] = "m_aFeatures";
        (*property_map)[0x17B071EC] = "m_pSummaryView";
        (*property_map)[0x4A58AFA9] = "m_eResetMode";
        (*property_map)[0x92FDBC5D] = "m_pPerfDebugViewDropShadow";
        (*property_map)[0x52198B0A] = "m_rHUDMessage";
        (*property_map)[0xF098FA5] = "m_eLookUpAxis";
        (*property_map)[0xAA65FED1] = "m_KeepLetterboxAfterFade";
        (*property_map)[0x302E627A] = "m_bTest";
        (*property_map)[0x40C5783B] = "m_sourceA";
        (*property_map)[0xA88FF25A] = "m_fStationaryTargetSpeedMultiplier";
        (*property_map)[0xA02FE64E] = "m_Anchors";
        (*property_map)[0x431CC7C7] = "m_ConstrainAnchors";
        (*property_map)[0x8073FB4D] = "m_fCurveDurationSpine";
        (*property_map)[0xE9815F1] = "m_nMaxEnterDelaySec";
        (*property_map)[0xFC621945] = "m_bDefaultAnchors";
        (*property_map)[0xC86E5BDB] = "m_nScopeSelection";
        (*property_map)[0xFFC04708] = "m_BoneNameA";
        (*property_map)[0x66C916B2] = "m_BoneNameB";
        (*property_map)[0xF78387E] = "m_fExitEventTimeMax";
        (*property_map)[0x2EEDC006] = "m_ragdoll";
        (*property_map)[0xD57DC7BA] = "m_eLetterBoxAspect";
        (*property_map)[0x4E57FE1F] = "m_sEndGamePageTransitionOverride";
        (*property_map)[0xF2F8711D] = "m_fCapsuleHeadAttacherOffset";
        (*property_map)[0x7A1C5E64] = "m_nDrawTessellation";
        (*property_map)[0xB4CF869D] = "m_bUseItems";
        (*property_map)[0x9232113B] = "m_AmbienceSide0";
        (*property_map)[0x9633CD76] = "m_eEvaluation";
        (*property_map)[0x3F471211] = "m_fExclusionHitmanDistance";
        (*property_map)[0xF9F6F977] = "m_vRoomMin";
        (*property_map)[0xD19D6641] = "m_bScatter";
        (*property_map)[0x67134578] = "m_pBackupGroup";
        (*property_map)[0x806EBE01] = "m_fShotsPerMinuteBonus";
        (*property_map)[0x39566FC5] = "m_nMaxNumberOfGuards";
        (*property_map)[0x777405BF] = "m_fAttentionGainRange";
        (*property_map)[0x3D3EB32A] = "m_fAngleYaw";
        (*property_map)[0x13BBE1B7] = "m_MorphemeEntityID";
        (*property_map)[0x41615173] = "m_BaseId";
        (*property_map)[0xBB812416] = "m_fAnglePitch";
        (*property_map)[0xC735AE6C] = "m_Fallback";
        (*property_map)[0xF511A3F9] = "m_pFollow";
        (*property_map)[0x5C3F4AF0] = "m_bDepthOnly";
        (*property_map)[0xD44BA0BE] = "m_rOrigin";
        (*property_map)[0x46317D9D] = "m_rStartPos";
        (*property_map)[0xEB0BEE61] = "m_sideBoxDepthMarginNear";
        (*property_map)[0x11297BB6] = "m_fAttenuation";
        (*property_map)[0xFD002C6F] = "m_commandQueue";
        (*property_map)[0xE68D6BEE] = "m_AudioEmitter";
        (*property_map)[0xFA39BF92] = "m_sExportMapName";
        (*property_map)[0x675C917A] = "m_sequenceTime";
        (*property_map)[0xBA7DE43B] = "m_fMoveCameraSeekStrengthAlternative";
        (*property_map)[0xFFF3CC4E] = "m_sequenceOrigin";
        (*property_map)[0xA86C42BC] = "m_RadiusWhileInCloset";
        (*property_map)[0x4468EC42] = "m_effectiveSequenceTime";
        (*property_map)[0x94012453] = "m_rDoorLocked";
        (*property_map)[0x6870F52A] = "m_bLeftHandForceEnableIKIfFarAway";
        (*property_map)[0xBDAA81D7] = "intField";
        (*property_map)[0xED415CA7] = "m_fHoldTime";
        (*property_map)[0xC7CBF6F9] = "m_nEpicActorCount";
        (*property_map)[0xC627EE26] = "m_vRandomRotation";
        (*property_map)[0xF313B7DE] = "bProjectile";
        (*property_map)[0x8612FF60] = "m_eStateFlags";
        (*property_map)[0x7BE342A1] = "m_aSubactions";
        (*property_map)[0xC12CE2B9] = "m_eSenderEvent";
        (*property_map)[0x576BC0C2] = "m_bDumpedBodyHidden";
        (*property_map)[0xCA866321] = "m_ColorArray";
        (*property_map)[0x308A1628] = "m_bOnGameStopped";
        (*property_map)[0x709F1D52] = "m_bArmorPiercing";
        (*property_map)[0x8F5420BF] = "m_ePerceptionSensitivity";
        (*property_map)[0xEB71FB4B] = "m_fRandomizeSizeFactor";
        (*property_map)[0x604E64E0] = "m_fFairnessWeight";
        (*property_map)[0xCDBE243] = "m_bCornerLeftEnabled";
        (*property_map)[0x8C303165] = "m_FogColor";
        (*property_map)[0x5A568B4B] = "m_fVisitationWeight";
        (*property_map)[0xF07C3E1E] = "m_fRangeCloseBonus";
        (*property_map)[0x99468F88] = "m_pDebugTarget";
        (*property_map)[0x7A3D7EDE] = "m_rAttachSpatial";
        (*property_map)[0x6322317B] = "m_ParticleModifiers";
        (*property_map)[0xB7E8CF83] = "m_standDown";
        (*property_map)[0x27394F3A] = "m_ePrintDestination";
        (*property_map)[0xDF70BA11] = "m_AzimuthRolloffCurve";
        (*property_map)[0xF81B5572] = "m_vHitNormal";
        (*property_map)[0x48847E0C] = "m_sDebugChannel";
        (*property_map)[0x40652E18] = "AI_VISION_BLOCKER";
        (*property_map)[0x7B482DF1] = "bBulletCausesHitReaction";
        (*property_map)[0xDA43A638] = "m_rExitZone";
        (*property_map)[0x7B09F5F4] = "m_fScreenDrawDuration";
        (*property_map)[0xDFD0E835] = "m_vEnvCollisionsBoxSizeFist";
        (*property_map)[0x56930D92] = "m_actorTemplate0";
        (*property_map)[0x938FEF13] = "m_rObjectProxy";
        (*property_map)[0x85D94462] = "count";
        (*property_map)[0xFA574ECB] = "m_pCollisionEmitterProxy";
        (*property_map)[0xB93CC2A] = "m_templateUIControl";
        (*property_map)[0x21943D04] = "m_actorTemplate1";
        (*property_map)[0x888EEB] = "m_IsFoundOutsideNavmeshAndIgnored";
        (*property_map)[0xE17F8233] = "m_pApproachLookAtPoint";
        (*property_map)[0xF579AED] = "m_nFromPinID";
        (*property_map)[0x5B71A019] = "m_GlowInteractionSelectedOutlineColor";
        (*property_map)[0xF3F4338D] = "m_ExcludeBox";
        (*property_map)[0x8453A84E] = "m_CombatSituation";
        (*property_map)[0x19D35FB4] = "m_bEnableTAA";
        (*property_map)[0x447C412A] = "m_eActorGroup";
        (*property_map)[0xB1661CFD] = "m_fsmState";
        (*property_map)[0x3AB16BC6] = "m_Targets";
        (*property_map)[0x87CEBF2A] = "m_rCameraConfig";
        (*property_map)[0xFA0DA56C] = "m_glowColor2";
        (*property_map)[0x2ECC261C] = "m_eSoundMaterialType";
        (*property_map)[0x258E2512] = "sCurrentValue";
        (*property_map)[0x2928F130] = "m_bHitmanIsGuard";
        (*property_map)[0xFF3D9F15] = "m_bSubscribe";
        (*property_map)[0x5CA15AAB] = "m_fAspectWByH";
        (*property_map)[0x2F2D94FA] = "m_eEnableEval";
        (*property_map)[0xE406A4E4] = "m_eDisturbanceType";
        (*property_map)[0x548DC1CE] = "m_eDeathTypeBack";
        (*property_map)[0xDBA41BF0] = "m_fMovingTime";
        (*property_map)[0xD9A5BA] = "AddAgentData_HumanoidWorldState_Pool_SaveData";
        (*property_map)[0x3115545C] = "m_fMiddleGrayMultiplier";
        (*property_map)[0x8B5B4BB9] = "m_fRunningTime";
        (*property_map)[0xAD775B43] = "m_tWaitToComplyTime";
        (*property_map)[0x3EDC76A1] = "m_bVisibleInBoxReflection";
        (*property_map)[0x9961C4CB] = "m_bWeaponFound";
        (*property_map)[0xDA61D51D] = "m_nSize";
        (*property_map)[0x3412F3D7] = "m_bMoveWarning";
        (*property_map)[0x7AC819B5] = "m_bSuppressSocialGreeting";
        (*property_map)[0x290EF2C3] = "m_fReleaseTime";
        (*property_map)[0x387CF292] = "m_nAudioFireType";
        (*property_map)[0x8655B920] = "m_rGuardNodes";
        (*property_map)[0xD11F6C2D] = "m_nIdentifier";
        (*property_map)[0x944F5C82] = "m_fParticleFxFadeRange";
        (*property_map)[0xD4BFF4AA] = "m_bIsSneaking";
        (*property_map)[0x8182B6CE] = "m_tLastInfluenceCalc";
        (*property_map)[0x8B7ED275] = "m_bBaseMapEnabled";
        (*property_map)[0x28A8207C] = "m_fRecoilZ";
        (*property_map)[0xD41EECFA] = "m_vFogNearDistances";
        (*property_map)[0xCE5B5FA4] = "m_tTimeIdling";
        (*property_map)[0x2AD69902] = "m_iGridId";
        (*property_map)[0x22E2C9FE] = "m_nBulletImpactsHeard";
        (*property_map)[0xC8491F1F] = "m_bStartsDual";
        (*property_map)[0x9A631A98] = "m_bCivOccupant";
        (*property_map)[0xE2EED86F] = "m_rRequiresActivation";
        (*property_map)[0x8FD61593] = "m_bPausesGame";
        (*property_map)[0xD2806EEE] = "m_fRadiusXBottom";
        (*property_map)[0xA87F279] = "m_sLabelKey";
        (*property_map)[0xE20C95B] = "m_bRecievingNewHuntTarget";
        (*property_map)[0x2E6ABA4C] = "m_spatial";
        (*property_map)[0xFF1A74DE] = "m_bLockdownEnforcer";
        (*property_map)[0x396026B8] = "m_eCameraControls";
        (*property_map)[0x92335D43] = "bHeadshot";
        (*property_map)[0xF150BD4] = "m_fFiredAtTime";
        (*property_map)[0x67322A38] = "m_bAllowActorGroupA";
        (*property_map)[0xC9A303DC] = "m_rSuperSpecialTriggerEffect";
        (*property_map)[0xF5E81999] = "a2";
        (*property_map)[0x9B5704AD] = "m_bStartsActive";
        (*property_map)[0x5DE97435] = "bLongRange";
        (*property_map)[0x13454A55] = "m_Perspective_zNear";
        (*property_map)[0xC5A2AF0B] = "m_fArrayIndexMax";
        (*property_map)[0x9D1C35F7] = "m_bAttachChildrenOnSelected";
        (*property_map)[0x876064AF] = "m_bEnableBottomRadii";
        (*property_map)[0x11612BF1] = "m_fTetherDistanceStop";
        (*property_map)[0x3BCB90E4] = "m_fObstructionSmoothingOverride";
        (*property_map)[0xD9DBFA18] = "m_TextLine";
        (*property_map)[0xEF8F7E2A] = "ammo";
        (*property_map)[0xAAC8664E] = "maxDeathContext";
        (*property_map)[0x42FAAB65] = "m_vContactForce";
        (*property_map)[0xBC239518] = "m_GlowTaggedOutlineColor";
        (*property_map)[0xF1E9AF4B] = "rAccidentSetup";
        (*property_map)[0x15BADDD9] = "bFirearmPacifiesTarget";
        (*property_map)[0x36AF3AB7] = "m_bValidAccuseUnconsciousOutfit";
        (*property_map)[0x602EFFEF] = "m_aAvoidDangerousAreaGroups";
        (*property_map)[0xC8AEEA24] = "m_bNPCUseCover";
        (*property_map)[0x90A72929] = "m_fMaxApproachDistance";
        (*property_map)[0xD776CFF7] = "eDeathSpeak";
        (*property_map)[0xFF5EA955] = "m_pVariationResource";
        (*property_map)[0xAB079D39] = "fAgilityElementUncosciousTime";
        (*property_map)[0xFABDCEC7] = "m_fAAMaxDistToCamAxis";
        (*property_map)[0x9E3A4CCA] = "m_bInstinctOnly";
        (*property_map)[0xBB08DDA1] = "m_bShowMilliseconds";
        (*property_map)[0xA6FB5A00] = "m_fCombatLastKnownNodeTime";
        (*property_map)[0xB1ADEE2] = "m_fColorIntensity";
        (*property_map)[0xEBAD597D] = "m_nDeathType";
        (*property_map)[0xB5C94DDB] = "m_eAnimationCategory";
        (*property_map)[0x1EA6F4BE] = "m_bResuming";
        (*property_map)[0x2A20869E] = "m_nNumDeathsBeforeWarzone";
        (*property_map)[0xBD4F2882] = "m_CartridgeEject";
        (*property_map)[0xE1BD9777] = "m_rDollyController";
        (*property_map)[0x79359A7C] = "m_SoundSetup";
        (*property_map)[0x85642D40] = "m_LeftHandPos";
        (*property_map)[0x878C7AAB] = "m_fSpacing";
        (*property_map)[0x44FD4AFD] = "m_fMuzzleEnergyMultiplier";
        (*property_map)[0x94F0730B] = "m_rPayload";
        (*property_map)[0x70510D30] = "m_aCurrentEffects";
        (*property_map)[0xA19D463] = "m_ScopePosition";
        (*property_map)[0x39F894C0] = "m_bUseRaycastObstruction";
        (*property_map)[0x8CD187AF] = "iCategory";
        (*property_map)[0xA8DA421F] = "m_bAssistantOrderAssigned";
        (*property_map)[0x9E30FE9] = "m_bStartWhenAllReady";
        (*property_map)[0x8EBEE339] = "m_bUseObstructionRTPC";
        (*property_map)[0x7F62A675] = "m_aObjectsToFollowDirectionOfMotion";
        (*property_map)[0xAD7DC1E1] = "m_bDebugInjectAIEvent";
        (*property_map)[0x7F585E0B] = "m_ObstructionShapeRoot";
        (*property_map)[0x75C431E4] = "shouldRun";
        (*property_map)[0x4ED42566] = "m_fObstructionMinRadius";
        (*property_map)[0xD671BB59] = "m_rDragBodybagEntity";
        (*property_map)[0x6E2EEE95] = "m_bLegalProfessions";
        (*property_map)[0xCA94793A] = "m_bY";
        (*property_map)[0x95DB85F8] = "m_bAllowStuckItem";
        (*property_map)[0xBD09FF8C] = "m_fAttenuationScaling";
        (*property_map)[0x2EF73787] = "m_bWorldAlignRotateHorizontalOnly";
        (*property_map)[0xCE032E74] = "m_aChildren";
        (*property_map)[0xCC255276] = "m_CollisionResourceID";
        (*property_map)[0x1238F0DB] = "m_fCurveDurationSpine2";
        (*property_map)[0x9041C854] = "m_Value";
        (*property_map)[0x236CA17F] = "m_bEnableAzimuth";
        (*property_map)[0xF4D275F2] = "m_fMinAzimuthDistance";
        (*property_map)[0x39B082A2] = "AudibleInMicrophones";
        (*property_map)[0xDDCA290D] = "m_bFrameUpdateActive";
        (*property_map)[0x2DFA02CC] = "m_bMountDismountRight";
        (*property_map)[0x5F0BBB9C] = "m_bPropagationEnabled";
        (*property_map)[0xED17400] = "m_fAngleSpine";
        (*property_map)[0x9955AF7E] = "m_nEjectForce";
        (*property_map)[0x1ADB6F79] = "m_fAngleSpine2";
        (*property_map)[0x1B2E70F8] = "m_fVolumetricGain";
        (*property_map)[0x378DB097] = "m_fAngleNeck";
        (*property_map)[0xB06F9058] = "m_fAngleHead";
        (*property_map)[0xA19000A8] = "m_nMaxRowsPerBlock";
        (*property_map)[0x21EC2143] = "eDifficultyLevel";
        (*property_map)[0x28E98CD2] = "variantField";
        (*property_map)[0x153B7D69] = "sLastEventToken";
        (*property_map)[0x3C0DD0DB] = "m_bUseListenerPos";
        (*property_map)[0x4532A70E] = "m_bAnimationResourceLoading";
        (*property_map)[0x176568B6] = "m_targetLOSField";
        (*property_map)[0x6596A96C] = "m_fFalloffDistance";
        (*property_map)[0x873AEEDB] = "m_fCurveStartFractionNeck";
        (*property_map)[0x925ADE05] = "m_fCurveStartFractionNeck1";
        (*property_map)[0xDADC34AD] = "m_fHFSpeed";
        (*property_map)[0xC25DB4C3] = "m_fCurveStartTimeSpine";
        (*property_map)[0x473C727F] = "m_pCrowd";
        (*property_map)[0x55CE2EDF] = "m_SupportItemID";
        (*property_map)[0x925F9344] = "translucency";
        (*property_map)[0x4C9282D] = "m_fSettleTime";
        (*property_map)[0x63E12292] = "m_vHitDistance";
        (*property_map)[0xA2175D4C] = "m_rObject";
        (*property_map)[0x30D8C6EC] = "m_fCurveStartTimeHead";
        (*property_map)[0x8D4C876] = "m_bActivateOnStart";
        (*property_map)[0xDC598756] = "m_bHighShadowPriority";
        (*property_map)[0xAC0F24C5] = "m_fCurveDurationHead";
        (*property_map)[0xCB2F4107] = "m_bIsPointOfInterest1";
        (*property_map)[0x82064802] = "m_eEmitterEffect";
        (*property_map)[0x294A69E] = "m_fTwistTime";
        (*property_map)[0x145E0547] = "m_fRotationSpeed";
        (*property_map)[0x1885411] = "m_nAudioWeaponClass";
        (*property_map)[0x5FD737FD] = "m_fTimeToNextEvent";
        (*property_map)[0xE67F5D9C] = "m_materialDisc";
        (*property_map)[0x30B5D6C8] = "m_tEscapeTime";
        (*property_map)[0xD4540818] = "m_rRepositoryKeyToFind";
        (*property_map)[0xF93CDB0E] = "m_fSpatialBlurriness";
        (*property_map)[0x53367312] = "m_nReactionTimeOut1";
        (*property_map)[0x6C5CE947] = "m_nMaxNumSubSteps";
        (*property_map)[0x6FE9CB1D] = "m_fFallbackRange";
        (*property_map)[0x9CC65FA3] = "m_GameCamProfiles";
        (*property_map)[0x6098DA27] = "m_rSituationTarget";
        (*property_map)[0x401B1E44] = "m_GameCamTransitions";
        (*property_map)[0x75E277A9] = "mainslots";
        (*property_map)[0x6B56823D] = "m_rCoverPlane";
        (*property_map)[0xAF3236E9] = "m_nCoverAnimNodeID";
        (*property_map)[0xB40426F2] = "m_pAudioEventRes";
        (*property_map)[0xE119D32E] = "m_bLeftSideFacing";
        (*property_map)[0x886D59B8] = "m_worldSpaceSettings";
        (*property_map)[0x688F7E4F] = "m_EntityListB";
        (*property_map)[0x3A8BD4CF] = "m_fCausticsAnimationSpeed";
        (*property_map)[0x6B27A6A6] = "m_collapsedAdditionalScaleFactor3D";
        (*property_map)[0xEFA14E0C] = "m_LOSMachineGunFirePatterns";
        (*property_map)[0x5C3C7A90] = "m_rVIP";
        (*property_map)[0x12A4F3A9] = "m_bForceEnableRequiresCondition";
        (*property_map)[0xD4145826] = "m_vipsUnderFire";
        (*property_map)[0x58079DA] = "m_fPitch";
        (*property_map)[0xFD9860C5] = "m_fParticleCollisionSizeFactor";
        (*property_map)[0x524D8A26] = "m_HDRLensFlareColorA";
        (*property_map)[0xF3B6E85C] = "m_rDoorOpenFromOutside";
        (*property_map)[0xD002001D] = "m_rAccessoryItemKeyword";
        (*property_map)[0x9B4CF066] = "m_fMotionStretchFactor";
        (*property_map)[0xE89E7780] = "m_sHUDMessageDebug";
        (*property_map)[0x61E8BBD5] = "m_fTimeToLiveSecondsMax";
        (*property_map)[0xB075FC9F] = "arrayField";
        (*property_map)[0x5FB2E469] = "m_StunDuration";
        (*property_map)[0x214A778E] = "m_fRotationSpeedMax";
        (*property_map)[0xDA18AFC2] = "m_fInitialRotation";
        (*property_map)[0x1526473B] = "m_pUIControlFactor";
        (*property_map)[0xEB5042E8] = "m_ePivotLocation";
        (*property_map)[0xE8EBF56E] = "m_bSizeDependantFadeDistances";
        (*property_map)[0x3F7C10BC] = "m_fTossHeight";
        (*property_map)[0x32D7DB3D] = "m_nDensityTotalMax";
        (*property_map)[0xC692E06D] = "m_bEmitterSpaceInitialUpVector";
        (*property_map)[0xB507834] = "m_Target";
        (*property_map)[0x953A5BA1] = "m_Clients";
        (*property_map)[0x5941242B] = "m_bDrawWater";
        (*property_map)[0x71D2E1D0] = "m_fMass";
        (*property_map)[0x88B3243] = "m_vGravity";
        (*property_map)[0x410FD046] = "m_aLocalized";
        (*property_map)[0xCB78BF7E] = "localOffset";
        (*property_map)[0xB810D842] = "m_fFrequency";
        (*property_map)[0x7258C299] = "m_fDragConstant";
        (*property_map)[0x52F783B1] = "m_fBendCurvature";
        (*property_map)[0x6ED5C19C] = "m_bInputValue";
        (*property_map)[0x57CD0309] = "m_eBendConstrainType";
        (*property_map)[0x7D563956] = "bodyheadline";
        (*property_map)[0xD49817DC] = "m_fSlowDownStickLimit";
        (*property_map)[0x26A8FA59] = "m_LocalColliders";
        (*property_map)[0xD059A070] = "m_fTeleportDistance";
        (*property_map)[0x46BC4D5C] = "m_bInteractions";
        (*property_map)[0x8BBDFF10] = "m_bCloseCombatSnapNeck";
        (*property_map)[0xB4C07186] = "m_tLoiteringCooldown";
        (*property_map)[0x26631A12] = "m_bGlobalCollidersEnabled";
        (*property_map)[0x6DDAF4E4] = "m_fChangePositionMinTime";
        (*property_map)[0x32481510] = "m_bLimitTranslation";
        (*property_map)[0xC7CB6652] = "m_bAllowItemPickup";
        (*property_map)[0xE000440A] = "m_MaxDistance";
        (*property_map)[0x9BAE3D3A] = "m_fCurrentValue";
        (*property_map)[0xB8BCF397] = "m_fMaxTranslationPerpGravity";
        (*property_map)[0x3B9219EA] = "m_SlashEffects";
        (*property_map)[0x7BA41A01] = "rCameraLocation";
        (*property_map)[0x5DC3D644] = "m_sStr";
        (*property_map)[0xC9EC62A7] = "m_bAccidentBystander";
        (*property_map)[0xBA2F973A] = "m_QualityScorpio";
        (*property_map)[0xDCF899EC] = "m_pIntelControl";
        (*property_map)[0x7786141C] = "m_eGait";
        (*property_map)[0xF7C0246A] = "size";
        (*property_map)[0xFED63E6] = "m_fJumpOverCoverDistance";
        (*property_map)[0x501BB6D1] = "m_bCollideCamera";
        (*property_map)[0x40C72289] = "m_Boxes";
        (*property_map)[0x3229B1C] = "m_eGesture";
        (*property_map)[0xDC10F5B9] = "m_bAllOnNoVolumes";
        (*property_map)[0x707825D5] = "m_bEmissiveOnly";
        (*property_map)[0x626FF779] = "STATIC_COLLIDABLES_ONLY_TRANSPARENT";
        (*property_map)[0x2E37845C] = "m_nKnowledgeIndex";
        (*property_map)[0xEA119EDC] = "m_fSize";
        (*property_map)[0xA931FFF6] = "RecenterHintAngle";
        (*property_map)[0xDB2E17DB] = "m_fAspectXByY_Actual";
        (*property_map)[0x4ED07A79] = "m_sideBoxSize";
        (*property_map)[0x5818E728] = "m_fActionCooldownTime";
        (*property_map)[0xACE40554] = "m_pDramaTemplate";
        (*property_map)[0xF8861DBE] = "m_sBodyHeadline";
        (*property_map)[0x31006DC0] = "m_fEmissionCntLeft";
        (*property_map)[0x40B6262C] = "m_pMainItem";
        (*property_map)[0x24968AA4] = "m_bUnhide";
        (*property_map)[0x76F178CB] = "m_rSetpiece";
        (*property_map)[0xC4D5CB47] = "m_eEnterPhase";
        (*property_map)[0x8AAEDF0B] = "m_eCurrentPhase";
        (*property_map)[0x1335E2FC] = "activityId";
        (*property_map)[0x7EA82CF5] = "m_VariationResourceID";
        (*property_map)[0xA352D42F] = "m_rEmitter";
        (*property_map)[0x8278BEDC] = "m_nSetId";
        (*property_map)[0xF459C176] = "m_rAddKeywords";
        (*property_map)[0xC3DD51CA] = "m_rGeomCollectionSetup";
        (*property_map)[0x6DDED6A2] = "m_nGlowInteractionDeselectedFadeSpeed";
        (*property_map)[0x69C000F6] = "m_fCameraHitmansFortuneAngle";
        (*property_map)[0x3B1D91F2] = "m_rRemoveKeywords";
        (*property_map)[0x93FC3220] = "KINEMATIC_COLLIDABLES_ONLY";
        (*property_map)[0x1B9767F] = "m_rExitPoint";
        (*property_map)[0x290F2CFF] = "bAutoAttachOnInit";
        (*property_map)[0x9BA02A2C] = "m_bScreenDecalsEnabled";
        (*property_map)[0x17C10467] = "m_fCloseDistance";
        (*property_map)[0x3D6ECAA4] = "m_pSourceControl";
        (*property_map)[0xCF4699EB] = "m_fPoweredJointImpactStrengthScale";
        (*property_map)[0x8662C4BF] = "m_rSubcontrollerContainer";
        (*property_map)[0x8FC2108] = "m_Zones";
        (*property_map)[0x68BE3DEA] = "m_pResource";
        (*property_map)[0x1BABDDC7] = "m_eReplicationStrategy";
        (*property_map)[0xA01B292A] = "m_nIntegrationRate";
        (*property_map)[0x4D991C38] = "m_bForceVisible";
        (*property_map)[0x4EAF65F8] = "m_rBackAttacherBoneAnimator";
        (*property_map)[0xD6B446D3] = "m_rLinkedEntityReplicator";
        (*property_map)[0x333A3BFC] = "m_pCoverPlane";
        (*property_map)[0xD698150F] = "toID";
        (*property_map)[0x13143E49] = "m_fWrappedDiffuse";
        (*property_map)[0x729B4A15] = "m_bIsGrenadeDroppable";
        (*property_map)[0x48A1E3EE] = "m_rLinkedEntityReplicaGhost";
        (*property_map)[0xC70A469B] = "m_bDrawRefractions";
        (*property_map)[0x9E8BB811] = "propertyValue";
        (*property_map)[0xD9567479] = "m_fDecalFadeDistance";
        (*property_map)[0x5E9E89CB] = "location";
        (*property_map)[0xCC886517] = "m_nRoute";
        (*property_map)[0x6021276] = "m_fInterruptibleActionDuration";
        (*property_map)[0xB358EE5B] = "m_gait";
        (*property_map)[0x335A537F] = "m_aParameters";
        (*property_map)[0x45EA807C] = "m_fPropagationProximitySpreadPerSecond";
        (*property_map)[0x1D20ACBA] = "m_aTimerEntityCommands";
        (*property_map)[0x43D14192] = "m_speed";
        (*property_map)[0x4679E73C] = "m_eAnimBlendMode";
        (*property_map)[0x467E44A2] = "m_GlowInteractionDeselectedOutlineColor";
        (*property_map)[0x6D8B4921] = "floatField";
        (*property_map)[0x46CEC08E] = "m_fPulseIncreaseOnShooting";
        (*property_map)[0x1C0FC5D2] = "m_rRevealingCondition";
        (*property_map)[0x5BE6794] = "m_nEffectSmokeSelection";
        (*property_map)[0xC566BE93] = "m_bMirrored";
        (*property_map)[0x16F35B70] = "m_rFlavorResource";
        (*property_map)[0x1F861918] = "m_aMurderEvents";
        (*property_map)[0x14866AFC] = "m_pOwnerActor";
        (*property_map)[0x99754198] = "m_eSituation";
        (*property_map)[0xF05A1DA8] = "m_fMinTimeSame";
        (*property_map)[0xAA6CB56E] = "m_tCurrentStateEnterTime";
        (*property_map)[0x5F62AC1C] = "m_fPoweredRagdollsImpactBlendInTime";
        (*property_map)[0x205AE972] = "m_pContainerControl3D";
        (*property_map)[0xB290562F] = "m_bUseOcclusionFiltering";
        (*property_map)[0x57A24D63] = "m_CrowdEntity";
        (*property_map)[0x78D4E413] = "m_bIgnoreCharacters";
        (*property_map)[0xAB53D3FE] = "m_bBubbleCancelAction";
        (*property_map)[0xD5C830CE] = "m_aShards";
        (*property_map)[0x30CEBA41] = "m_bPreferredHeroSpawn";
        (*property_map)[0xB887A5B9] = "m_rendObjList";
        (*property_map)[0x84EC6EEF] = "m_secSleepWhenNoOrders";
        (*property_map)[0x50EC64E5] = "entities";
        (*property_map)[0x34C0F257] = "m_sTemplatePath";
        (*property_map)[0x4B93A9F8] = "m_eRangeParameter";
        (*property_map)[0x8667E800] = "m_bSupportStreaming";
        (*property_map)[0xE35E1339] = "m_bIsSpeaking";
        (*property_map)[0x6FF2FBDE] = "m_bUseLowLimit";
        (*property_map)[0x2B0B4592] = "m_bTarget";
        (*property_map)[0x92FB8549] = "m_fLowClampLimit";
        (*property_map)[0xA235C8C8] = "sDisplayName";
        (*property_map)[0x7BD84B26] = "m_fLowClampReplace";
        (*property_map)[0x981D3601] = "m_aSequenceSaveData";
        (*property_map)[0xDE670E7D] = "m_nDeathContext";
        (*property_map)[0x312B2350] = "m_eSecondWarning";
        (*property_map)[0x6EA1363C] = "m_bWaitingForPrecache";
        (*property_map)[0xFC59F7C9] = "m_fMagneticBulletDistance";
        (*property_map)[0x880C8A6B] = "m_bSSAOEnabled";
        (*property_map)[0x992BE94A] = "m_pSymbolsDefinitions";
        (*property_map)[0xFA282706] = "m_bUseHighLimit";
        (*property_map)[0x453F4A4C] = "m_bForceSearch";
        (*property_map)[0x89358ECE] = "m_fHighClampLimit";
        (*property_map)[0xB8619321] = "m_fSlidingEventCooldownTimeRagdoll";
        (*property_map)[0x98C887EC] = "m_fHighClampReplace";
        (*property_map)[0x1747381E] = "m_fLookAtRange";
        (*property_map)[0x656D751] = "m_pathToSubTarget";
        (*property_map)[0xCDE803A2] = "m_sTriggerName";
        (*property_map)[0x12C8E91E] = "m_bActivationValue";
        (*property_map)[0xDE5C12CB] = "m_rSwitchAudioTailType";
        (*property_map)[0xC2BF55D5] = "m_sHeader";
        (*property_map)[0x58073C14] = "m_nDeactivate";
        (*property_map)[0x1BB28B30] = "m_eModifierScope";
        (*property_map)[0xD194C41D] = "m_nLabelLen";
        (*property_map)[0x5890F648] = "m_eSpecialSituation";
        (*property_map)[0xC3C86F42] = "m_dExtraData";
        (*property_map)[0x90854798] = "m_fDelay";
        (*property_map)[0xB890D81D] = "m_vOriginalPositionLeader";
        (*property_map)[0x34E92A12] = "m_bWrapNavigation";
        (*property_map)[0x52928A8F] = "m_GateFlags";
        (*property_map)[0xC23C878E] = "m_SnapBones";
        (*property_map)[0xB02B8442] = "m_eInputEvaluation";
        (*property_map)[0x74A3362C] = "m_eTargetEmotionState";
        (*property_map)[0x75F53F4D] = "m_fNeutralBlendWeight";
        (*property_map)[0xE6A6239F] = "m_fProjectionBlendDistance";
        (*property_map)[0xDE8F032E] = "m_bStanddown";
        (*property_map)[0xB82483E7] = "m_fShockAbsorption";
        (*property_map)[0x2C9640FC] = "m_pPerceptibleSpatial";
        (*property_map)[0x67E9CFAF] = "m_fCloseRange";
        (*property_map)[0x937BDCEC] = "m_eReason";
        (*property_map)[0xA7805EB5] = "m_pValueData";
        (*property_map)[0xFA933AC1] = "m_fEjectForceMax";
        (*property_map)[0x85118F9F] = "m_eRootBone";
        (*property_map)[0xEB55B070] = "m_fPenaltyMultiplier";
        (*property_map)[0xD35FB1A9] = "m_pDistDepthMap";
        (*property_map)[0x9A0CA743] = "m_eConditionType";
        (*property_map)[0x6BA79E88] = "m_fConeDistance";
        (*property_map)[0xA162BD72] = "m_fBreakableForce";
        (*property_map)[0xAAF46CF] = "m_extraDataProperties";
        (*property_map)[0xFABB598A] = "m_DescriptionStringID";
        (*property_map)[0xDFD74AA] = "m_rPoint";
        (*property_map)[0xE732C56C] = "m_aInput";
        (*property_map)[0x9052646D] = "m_aOutput";
        (*property_map)[0x60009D90] = "m_pOutfitCondition";
        (*property_map)[0x5B1BFF16] = "m_pFootstepSound";
        (*property_map)[0x6DE44026] = "description";
        (*property_map)[0xDE3FBF9D] = "m_pBeingObservedControl";
        (*property_map)[0xBCF5058F] = "m_rPosition";
        (*property_map)[0xCF370EFE] = "containedLabel";
        (*property_map)[0xA96C0F2] = "m_eScaleType";
        (*property_map)[0xC46D498] = "m_nTensionLimit";
        (*property_map)[0x75458C00] = "m_bUsePlacementAttacherOnPlace";
        (*property_map)[0x62A80871] = "m_vGlowPlayerInLVAFadeNearFar";
        (*property_map)[0xAD7116E7] = "m_pPushMenuContextObject";
        (*property_map)[0xA22F530B] = "m_bDisableAIBehaviorLast";
        (*property_map)[0xF3574F48] = "m_pHeader";
        (*property_map)[0x8FF15261] = "m_bWakeUpOnPlace";
        (*property_map)[0x4C03166E] = "m_bSeen";
        (*property_map)[0x975A3CD3] = "COLLIDE_WITH_ALL";
        (*property_map)[0x1C62447A] = "m_bVisibleOnlyUpdate";
        (*property_map)[0x15192D8D] = "m_source";
        (*property_map)[0x2A1E9B75] = "m_rItemTurnedOn";
        (*property_map)[0x4975A961] = "m_eRayCollidables";
        (*property_map)[0x96EC3CF4] = "m_GlowTagFocusUntaggedUnconsciousOutlineColor";
        (*property_map)[0x148490A2] = "m_rFallbackEffect";
        (*property_map)[0xBDC6DDD8] = "m_vSampleOffset";
        (*property_map)[0xA9AF114D] = "m_eIllegalityEval";
        (*property_map)[0x6F377F11] = "m_eBone";
        (*property_map)[0x16EBC7D6] = "m_aFrom";
        (*property_map)[0x19FF110D] = "m_nRayFilter";
        (*property_map)[0xB58AA7FE] = "m_bAgainstCrowds";
        (*property_map)[0x935EC1DA] = "m_eActorNearbyRequirements";
        (*property_map)[0xEE7B7CF3] = "nNeighbor4";
        (*property_map)[0x833C0C7A] = "isDroppable";
        (*property_map)[0xDF2BF0E5] = "actors";
        (*property_map)[0x4DECD656] = "m_fLFVerticalInt";
        (*property_map)[0xA5EFD402] = "m_bSelectHighestPriorityGroupOnly";
        (*property_map)[0xF40CF8A9] = "m_OverrideAssociations";
        (*property_map)[0x71A9EA6F] = "m_RuntimeNetworkResourcePreloaded";
        (*property_map)[0xFBCE3E7A] = "subject";
        (*property_map)[0x817ADB2F] = "m_vTranslationScale";
        (*property_map)[0xCA4D6410] = "m_pCrowdReaction";
        (*property_map)[0x8A96541B] = "m_nDensity1Max";
        (*property_map)[0x1A8E2278] = "m_tShareHuntTargetCoolDown";
        (*property_map)[0x5745552] = "m_bDead";
        (*property_map)[0xB3227704] = "m_pAmbienceElement";
        (*property_map)[0x546AE028] = "m_FilterToInvert";
        (*property_map)[0xFB236E86] = "m_pGeneric01Sound";
        (*property_map)[0xDA94B5C3] = "eViewportLock";
        (*property_map)[0x9F62BEF6] = "m_aListeningForActors";
        (*property_map)[0x1D96072F] = "m_bBlindFire";
        (*property_map)[0x9CECA00B] = "m_rKeywordsSupported";
        (*property_map)[0x3617BFFE] = "m_ePlacementType";
        (*property_map)[0x2ABAE44D] = "m_bWorksInContractsMode";
        (*property_map)[0xE43CC384] = "m_pEventResource";
        (*property_map)[0x86E1A4C8] = "m_bEventEndedOnStop";
        (*property_map)[0x6DCDC65D] = "m_SecondaryAIIconState";
        (*property_map)[0x4EC4CC3F] = "m_fCuredHealthThreshold";
        (*property_map)[0x37DFC19B] = "m_bRespondToCollisions";
        (*property_map)[0x6557F6] = "m_nLeaderTargetNodeIndex";
        (*property_map)[0x37DA1AE5] = "fViewportGutter";
        (*property_map)[0x88E31A11] = "m_fDamage";
        (*property_map)[0xF47E83E] = "m_bLeaderOrderAssigned";
        (*property_map)[0x9887A22F] = "m_bSearchCompleted";
        (*property_map)[0xC92EFB06] = "m_count";
        (*property_map)[0x7769B681] = "m_fTetherDistanceStart";
        (*property_map)[0xADB74007] = "m_Explosion";
        (*property_map)[0x7A3DE5D3] = "sFilePath";
        (*property_map)[0x2E45DAAE] = "weaponCategory";
        (*property_map)[0x4798CC20] = "m_bNoticed";
        (*property_map)[0xBB4F29D8] = "m_NumBlunt2HEmitters";
        (*property_map)[0xEC35249C] = "m_aKeywordStringData";
        (*property_map)[0xA215A270] = "m_fSpringBounceOffsetLarge";
        (*property_map)[0x8681152] = "m_bNeedsVolumeUpdate";
        (*property_map)[0xA8D9FFF5] = "m_bUseOriginalA";
        (*property_map)[0xC4EB1A24] = "m_nObstacleBlockageFlags";
        (*property_map)[0x85599925] = "m_bOnKill";
        (*property_map)[0x96B84CAB] = "m_tExpiresAt";
        (*property_map)[0x6CCF6FEF] = "m_rRevealHintCondition";
        (*property_map)[0x2DE79766] = "m_DomainConfig";
        (*property_map)[0xCDF5A865] = "entityName";
        (*property_map)[0x15F7B990] = "m_rBypassMenuCondition";
        (*property_map)[0x828B6F7E] = "m_rDraggedActor";
        (*property_map)[0xC1AF1228] = "m_aConditions";
        (*property_map)[0xC3B0C8BF] = "PCOnly";
        (*property_map)[0xB94D10C4] = "m_nPeriodSkip";
        (*property_map)[0x87C331C7] = "properties";
        (*property_map)[0xEBAD42BE] = "m_rMayPlayGreetingDialogCondition";
        (*property_map)[0xEC25FB70] = "m_rSoundEvent_Curious";
        (*property_map)[0xD3D9F901] = "m_rSoundEvent_Alerted";
        (*property_map)[0x3B35182] = "m_rParentArea";
        (*property_map)[0xA21A8580] = "m_sSweetStateTextIfOff";
        (*property_map)[0x8588C5BF] = "m_fLidAutoCloseTime";
        (*property_map)[0xD4CAD14A] = "m_bAllowKinematicKinematicDestruction";
        (*property_map)[0xEDF6E6CC] = "m_rSoundEvent_Arrest_Weapon";
        (*property_map)[0x55AAFF21] = "m_rSoundEvent_Arrest_Killer";
        (*property_map)[0xD39CF5CE] = "m_bOverrideItemThrowRotation";
        (*property_map)[0x3DCA7FE0] = "m_AdditionalTargetControls";
        (*property_map)[0x4723E139] = "m_nDimension";
        (*property_map)[0xABBCAA64] = "m_bStraightRestPose";
        (*property_map)[0x5879C096] = "m_nDepthCubemapIndex";
        (*property_map)[0xC69E0598] = "m_fEjectForceMin";
        (*property_map)[0x8549A4C1] = "m_fBlurriness";
        (*property_map)[0x53F28AFA] = "m_rEvacuationNode";
        (*property_map)[0xB0FC428A] = "m_fCombatThreatRadiusAggressive";
        (*property_map)[0x1165FC8A] = "m_fNearZ";
        (*property_map)[0xDB11226E] = "m_bOverridePreviousAct";
        (*property_map)[0xDC939CBB] = "m_bConversationHelperFast";
        (*property_map)[0x1180E44B] = "m_iUnlockPrice";
        (*property_map)[0xFD7ACFFC] = "m_fFarZ";
        (*property_map)[0x44E61339] = "m_bAlignToWorld";
        (*property_map)[0xE1237D92] = "m_bUseCollisionManagerForActorSelection";
        (*property_map)[0x4DA7DB64] = "m_eInputFlag";
        (*property_map)[0xB1090CA6] = "m_nRequiredNumberOfParticipants";
        (*property_map)[0x7C7489B3] = "m_fCapsuleHeadAttacherOffsetSneaking";
        (*property_map)[0x3295AFE7] = "m_rDeadActor";
        (*property_map)[0xE632509D] = "m_fSynchronisedTimeOffset";
        (*property_map)[0xD52372AC] = "m_pChild";
        (*property_map)[0xB3D13600] = "m_rAvailableThrowTargetCondition";
        (*property_map)[0x31C99E5D] = "m_Stage1SpreadEvaluator";
        (*property_map)[0xF3DFE0AB] = "m_bDeactivateOnWalk";
        (*property_map)[0x734269ED] = "m_nGlowSetPieceFadeSpeed";
        (*property_map)[0x7116854B] = "m_bIsLegalTarget";
        (*property_map)[0x667FA4F] = "m_eEye";
        (*property_map)[0x425A6AD2] = "m_fValidAngle";
        (*property_map)[0x77EDD39F] = "m_rSoundEvent_Discharge";
        (*property_map)[0xFAC6937F] = "m_CandidateActor";
        (*property_map)[0xAA0F9D68] = "m_pActorEntity";
        (*property_map)[0xEDDA0EB1] = "m_rKeywordRequirements";
        (*property_map)[0x213C670C] = "pinConnections";
        (*property_map)[0xB24A496E] = "m_pAudioEvent";
        (*property_map)[0x42D59EB0] = "m_PosBackAttach";
        (*property_map)[0x9A58282B] = "eType";
        (*property_map)[0xD942DE82] = "attack_origin";
        (*property_map)[0x9F69A378] = "m_tSafeRoom";
        (*property_map)[0xB05D23F9] = "bIsToggledOn";
        (*property_map)[0x214DA039] = "m_rReservedFor";
        (*property_map)[0xF98FC4B7] = "m_rPeekCameraPosition";
        (*property_map)[0x513510E1] = "m_nStart";
        (*property_map)[0xC52CEE62] = "m_bIsAwake";
        (*property_map)[0x1076A09E] = "m_fGateDistanceExpansion";
        (*property_map)[0x4D38375A] = "subsets";
        (*property_map)[0x290E6318] = "m_eMovementIsMovingRequirement";
        (*property_map)[0x539D2880] = "m_bZ";
        (*property_map)[0xCC160D9] = "m_bStopped";
        (*property_map)[0xF3E250EF] = "m_HighPriority";
        (*property_map)[0xBC7BFAFD] = "m_bForceNoEnterAnimation";
        (*property_map)[0xC62463FB] = "m_nAmmoCountDartTranquilizer";
        (*property_map)[0xA9274F29] = "m_fSideChangeTime";
        (*property_map)[0x16EC0D51] = "m_fCullDistanceX";
        (*property_map)[0x91765E7B] = "m_bIgnoreLowNoise";
        (*property_map)[0x9930E83F] = "m_bIsCoin";
        (*property_map)[0x61EB3DC7] = "m_fCullDistanceY";
        (*property_map)[0xF8E26C7D] = "m_fCullDistanceZ";
        (*property_map)[0xB3C1F295] = "m_pMapLegendDataProvider";
        (*property_map)[0x9552DF9F] = "m_fGuardSightRange";
        (*property_map)[0xF90286FA] = "m_nMinNeighbours";
        (*property_map)[0x8BF37BB2] = "m_bIsVisible";
        (*property_map)[0x94F7B43A] = "m_eHorizontalAlignment";
        (*property_map)[0x4B236817] = "m_aLegalKeywords";
        (*property_map)[0xD9CC2981] = "m_sourceB";
        (*property_map)[0x605C0948] = "m_aFracturedEffect";
        (*property_map)[0xFAACAC35] = "Activities";
        (*property_map)[0x574F3710] = "m_bStopActivityWhenPushed";
        (*property_map)[0xE266AED1] = "m_fEmissionTimeLeft";
        (*property_map)[0x3E6CA1E1] = "m_aSuspiciousAreas";
        (*property_map)[0x6A1070E] = "m_fEmissionTimeFraction";
        (*property_map)[0x140F0BD6] = "m_nRearArcDeg";
        (*property_map)[0x4CB14AD8] = "m_fIllegalDuration";
        (*property_map)[0xA9838671] = "m_nHideCoverValue";
        (*property_map)[0x39F66650] = "m_nNewDelaySec";
        (*property_map)[0xF9614E93] = "m_accidentScaleContext";
        (*property_map)[0xCAB37A28] = "m_fCombatFirepatternPercentage";
        (*property_map)[0xC1FE854] = "m_sweetPrimary";
        (*property_map)[0x77CEA494] = "m_DisposalType";
        (*property_map)[0xB328C57E] = "m_bExpired";
        (*property_map)[0x33AE341B] = "m_eGeomGlowType";
        (*property_map)[0xD3C24766] = "m_aGuardPoints";
        (*property_map)[0xC8AAB7CF] = "m_rPickupAction";
        (*property_map)[0x42B0631D] = "m_GlowTagFocusTaggedUnconsciousColor";
        (*property_map)[0xEB481762] = "m_TextLineStadia";
        (*property_map)[0xCFD40D] = "m_bOnlyHitman";
        (*property_map)[0x98D87026] = "m_rGeomentity";
        (*property_map)[0xDDDAA06] = "m_rTransformParent";
        (*property_map)[0xD36F868A] = "m_rPhysicsAccessor";
        (*property_map)[0x18ABD29F] = "MicrophoneBusName";
        (*property_map)[0xB1F8B849] = "detectedDuringFrisk";
        (*property_map)[0x14359D18] = "m_Offset";
        (*property_map)[0x81BA9083] = "m_aEntityDatas";
        (*property_map)[0xFDC6F20D] = "m_GlowCameraColor";
        (*property_map)[0x4EA07BCD] = "m_bInvestigationGroupActive";
        (*property_map)[0x38BA6B41] = "m_PosHandAttach";
        (*property_map)[0x5E25CB78] = "m_PosPlacementAttach";
        (*property_map)[0xB81C01FD] = "m_bWaitForBlendOut";
        (*property_map)[0xF11D8C2] = "m_pCursorView";
        (*property_map)[0xBF8BD6F6] = "m_aGroupData";
        (*property_map)[0x74017C6B] = "m_tLastStandDown";
        (*property_map)[0x9B2FEBC9] = "m_visibilityPolicy";
        (*property_map)[0xA935AA16] = "m_isSetupPhaseComplete";
        (*property_map)[0x8BC3B341] = "m_PlacementVolume";
        (*property_map)[0x7F50D736] = "m_rRTEditRef";
        (*property_map)[0x19CD197E] = "m_bClothed";
        (*property_map)[0x268687C2] = "m_fWakeTimeOnPlaceInWorld";
        (*property_map)[0xAC9D8A3F] = "m_bHitmanAutoreload";
        (*property_map)[0xA43400D0] = "m_bOnlyUnscaled";
        (*property_map)[0x19AF25AB] = "m_bCivilianTransferredKnowledge";
        (*property_map)[0xB6343AA8] = "m_rItemContainer";
        (*property_map)[0x4E0BB78C] = "m_aPreferredTacticalNodes";
        (*property_map)[0xF8982F59] = "m_sLevelID";
        (*property_map)[0xEE898C2F] = "m_EventConsumerCollection";
        (*property_map)[0xE8824D09] = "m_bCheckInventory";
        (*property_map)[0xAB45F1A4] = "m_rBeaconSpatial";
        (*property_map)[0xA52A353F] = "m_aMoveToActs";
        (*property_map)[0x9B3115C6] = "m_rVisibleInInventory";
        (*property_map)[0x9C65D4AF] = "m_rFirstValueInt";
        (*property_map)[0xCE66E220] = "m_CombatMetricsSaveData";
        (*property_map)[0xAE62CA5C] = "m_tKnownSince";
        (*property_map)[0xDF4A090A] = "m_rItemOwned";
        (*property_map)[0x9F3D30A8] = "fromID";
        (*property_map)[0xCD005C17] = "m_rItemCanTurnOn";
        (*property_map)[0xB6960351] = "m_bIsOwnedByLocalPlayer";
        (*property_map)[0x4F975BB7] = "m_eAgilitySubstate";
        (*property_map)[0x31575862] = "m_bShouldPauseWorld";
        (*property_map)[0xA6A8DED6] = "draw_box";
        (*property_map)[0x2352A656] = "m_nOnNavGrid";
        (*property_map)[0x5A154014] = "m_fSmoothness";
        (*property_map)[0x6F84A575] = "m_vAspectVec2Val";
        (*property_map)[0xE7AB732] = "m_nAudioNodeId";
        (*property_map)[0xF221C315] = "m_bValidTargetDistanceField";
        (*property_map)[0xD142812D] = "actionAndKillTypes";
        (*property_map)[0xD1D87C58] = "m_pCustomInfo";
        (*property_map)[0x270BF8CF] = "m_aGreetingZones";
        (*property_map)[0xE49E6E51] = "m_aPoses";
        (*property_map)[0x35E2FC28] = "m_bSkipAllowed";
        (*property_map)[0xB9B8BC46] = "ControlName";
        (*property_map)[0x21AB5EC3] = "externalSceneIndex";
        (*property_map)[0x5B04340F] = "m_eOpportunityState";
        (*property_map)[0x930DA3E1] = "m_pEntity";
        (*property_map)[0x10840F9D] = "m_aSmuggleLocations";
        (*property_map)[0x3575AD68] = "act_true";
        (*property_map)[0xD7289557] = "m_aBodyPartCollections";
        (*property_map)[0xE0A86235] = "m_bDebugApplyGeotagDialogSwitch";
        (*property_map)[0xB3E2E26] = "m_tLastTargetVisible";
        (*property_map)[0x319CFAB8] = "m_bFrisk";
        (*property_map)[0x73F1057] = "m_NoLOSSMGFirePatterns";
        (*property_map)[0x5BA5DBC0] = "m_pMapViewControl";
        (*property_map)[0x4FE05028] = "m_WaitMinSeconds";
        (*property_map)[0xBE52B51A] = "m_bStopWarningGiven";
        (*property_map)[0xFBF52663] = "speak";
        (*property_map)[0x790D7223] = "m_rSoundEvent_Footstep";
        (*property_map)[0xAAF78B2] = "m_vNewVelocity";
        (*property_map)[0x52B8752C] = "m_aPockets";
        (*property_map)[0x4398999F] = "m_eActiveEval";
        (*property_map)[0x84F8AE97] = "m_fAngularDampening";
        (*property_map)[0x3B93197C] = "m_vPortalSize";
        (*property_map)[0xBD3F7F89] = "m_pActorInstance";
        (*property_map)[0xE9407EC5] = "m_Quality";
        (*property_map)[0x27AAC793] = "m_bAllowKinematicKinematicContactNotification";
        (*property_map)[0x9F3EDC4A] = "m_QualityDurango";
        (*property_map)[0x52552569] = "m_bNotebookEnabled";
        (*property_map)[0xDEEAFA7B] = "m_bNodeFilterScatter";
        (*property_map)[0xBEE4CAD5] = "m_QualityOrbis";
        (*property_map)[0x2C3EFD7C] = "m_nGateTraversalDepth";
        (*property_map)[0x41A11923] = "m_bInvertFilterMask";
        (*property_map)[0x9CFDBFB6] = "m_nBlurLevels";
        (*property_map)[0xAA29649D] = "m_fWaterReflectionAndRefractionCullDistance";
        (*property_map)[0x163F1C9F] = "m_bRequireInputToEnter";
        (*property_map)[0x4DC1F266] = "m_nLODOffsetLinked";
        (*property_map)[0xB9DEF0C1] = "m_TokenToFind";
        (*property_map)[0x9FF84F40] = "m_bNodeFilterParticleEmitter";
        (*property_map)[0xB68AFDB] = "m_bFlagFilterLinked";
        (*property_map)[0xE7E16084] = "m_bFlagFilterWeighted";
        (*property_map)[0xF2335851] = "m_eMinPerception";
        (*property_map)[0x70FFBCA3] = "m_bPrependAttachedEntitiesOnEvaluate";
        (*property_map)[0x49FBA2EE] = "m_bExemptFromTagging";
        (*property_map)[0x752614C1] = "m_aVideos";
        (*property_map)[0xEA750E8] = "label";
        (*property_map)[0x627F8A1E] = "m_bFlagFilterWater";
        (*property_map)[0x964A9B1E] = "m_fGlowInteractionOutlineWidth";
        (*property_map)[0x97C282BA] = "m_fTrespassingAttentionDistanceModifier";
        (*property_map)[0xF6D19E36] = "m_bFlagFilterIsDynamic";
        (*property_map)[0x113D1A0B] = "m_mTransform";
        (*property_map)[0x5F7FE495] = "m_bFlagFilterIsPlayer";
        (*property_map)[0x6E1A8423] = "m_nNumToCheck";
        (*property_map)[0xC879A62E] = "m_Perspective_zFar";
        (*property_map)[0xADFC174B] = "m_MirrorObjectsRecursive";
        (*property_map)[0xE93A6BDA] = "m_pItemStorage";
        (*property_map)[0x394854C9] = "m_InsidenessInterpolationCurve";
        (*property_map)[0x2EA5429B] = "m_State";
        (*property_map)[0x47B4D5E0] = "m_vTransLimitOffset";
        (*property_map)[0xFD7725F1] = "m_eSpawnMode";
        (*property_map)[0x62D277AF] = "z";
        (*property_map)[0xBE27634A] = "m_bLerpRadial";
        (*property_map)[0x9AA13476] = "m_nRayCollisionRadius";
        (*property_map)[0xDD6B7A5F] = "m_bInFrontOfHM";
        (*property_map)[0x953099D1] = "m_bOutwardEmission";
        (*property_map)[0x3F38106C] = "m_pMapLayer";
        (*property_map)[0x107FD402] = "m_eDecalEffect";
        (*property_map)[0x5DA005F9] = "m_bIsOnlyActiveInContractCreationMode";
        (*property_map)[0xD0F961A7] = "m_aInclusionZones";
        (*property_map)[0xD97FE572] = "m_rHero";
        (*property_map)[0x7CCF4164] = "m_fShadowFadeDistance";
        (*property_map)[0x1B63308A] = "m_sExpression";
        (*property_map)[0x82D54C4B] = "m_Interaction3";
        (*property_map)[0x911C145F] = "m_uiModBehavior";
        (*property_map)[0x11D0D0C7] = "m_MorphemeEntity";
        (*property_map)[0xEE5AA22E] = "m_rCurrentSpeakEntity";
        (*property_map)[0x8EBE422A] = "m_eRevealState";
        (*property_map)[0x6979C023] = "m_searchNode";
        (*property_map)[0x73530611] = "m_bIllegal";
        (*property_map)[0x439E6FDF] = "m_rContainerItemKey";
        (*property_map)[0xBAD16DE9] = "m_bForceActive";
        (*property_map)[0x3813672B] = "m_bAllowSpeedModifier";
        (*property_map)[0xC7D1CFC0] = "m_fMin";
        (*property_map)[0xD183E78] = "m_GlowCivilianUnconsciousColor";
        (*property_map)[0xF827CDF1] = "m_bFailedStage";
        (*property_map)[0x6C3F3DF5] = "m_bIsInPhysicsWorld";
        (*property_map)[0xB3A64528] = "m_bIsKinematicBody";
        (*property_map)[0x6164C0A7] = "nIntProp";
        (*property_map)[0x6DDF9795] = "m_nState";
        (*property_map)[0x507E1BF0] = "m_fVignetteMax";
        (*property_map)[0x53AF591E] = "m_sVertexPaintSourceResourceId";
        (*property_map)[0x5E38267F] = "m_bStickToPlane";
        (*property_map)[0x4B9F03BC] = "m_eSeamFix";
        (*property_map)[0x9C1D023B] = "m_curve1";
        (*property_map)[0x5145381] = "m_curve2";
        (*property_map)[0x69C080E1] = "m_fLevelOutThresholdTimeAim";
        (*property_map)[0x3275855A] = "m_fMoveCameraTargetVelocityV";
        (*property_map)[0x98CBE0C1] = "m_bRemovePhysics";
        (*property_map)[0xAC72D191] = "m_bStartSleeping";
        (*property_map)[0xA1BAB3BF] = "m_fRestitution";
        (*property_map)[0xC93AA4A9] = "m_rVolumeTrigger";
        (*property_map)[0xDD639B59] = "m_fHoldMaxDistanceToPeek";
        (*property_map)[0x6952CA03] = "m_metricName";
        (*property_map)[0x1EBE03BC] = "m_ValueArray";
        (*property_map)[0x3B03A9D4] = "m_GhostThreshold";
        (*property_map)[0x7E70DB86] = "m_fPositionFactor";
        (*property_map)[0x5E8757E5] = "m_eAnimSet";
        (*property_map)[0x82B256D3] = "m_fDistanceToOthersFactor";
        (*property_map)[0x756E67E1] = "m_bOverrideListenerOrientation";
        (*property_map)[0x522610BD] = "m_bIsPointOfInterest2";
        (*property_map)[0x265E72F] = "m_nMin";
        (*property_map)[0x1462C98C] = "m_aPerks";
        (*property_map)[0x4F998B48] = "m_nSimulateOnNthFrame";
        (*property_map)[0xC2EAA965] = "m_fVelocityMax";
        (*property_map)[0x4CD93E81] = "m_bIsTxtDirReversed";
        (*property_map)[0xB819C7F8] = "m_fRangeModifier";
        (*property_map)[0xAF5F4B8B] = "m_vASSAOFadeInRange";
        (*property_map)[0xCCE33520] = "m_bNaked";
        (*property_map)[0xCF20E9FA] = "AddAgentData_GuardHeadDomain_Pool_SaveData";
        (*property_map)[0x7902952] = "m_eReentryBehavior";
        (*property_map)[0x5569F876] = "m_aConnectionData";
        (*property_map)[0xC1845F3] = "m_fLoudnessModifier";
        (*property_map)[0xBA7E7A08] = "m_eJoinReason";
        (*property_map)[0x4256761C] = "m_Owner";
        (*property_map)[0xE63F6554] = "m_nCSMMode";
        (*property_map)[0x49669C3] = "m_fBlendOut";
        (*property_map)[0x7FC2C3EC] = "m_bReverseFilter";
        (*property_map)[0x835DE3AD] = "m_fSpringMassLarge";
        (*property_map)[0xB216206C] = "m_bSendFromClient";
        (*property_map)[0xBE6C3252] = "m_vGlowContractTargetSpecialVignetteRadii";
        (*property_map)[0xD7332F1D] = "m_aBoxVolumes";
        (*property_map)[0x36823C2F] = "m_nNumPacifyBeforeKill";
        (*property_map)[0x22BFFF23] = "m_nCooldownOnPause";
        (*property_map)[0xA205EFF] = "m_nAmmoCountMG";
        (*property_map)[0xF9B9A8F] = "m_fPitchMaxOffset";
        (*property_map)[0x6FBC3086] = "m_bPerceptibleEnabled";
        (*property_map)[0x6289DEEB] = "m_LightEntity";
        (*property_map)[0x67078B2D] = "m_rOriginalB";
        (*property_map)[0x9B8C4C47] = "m_rSoundEvent_Take_Disguise";
        (*property_map)[0x8EFBAD0B] = "m_rSoundEvent_Takedown";
        (*property_map)[0x865F80C0] = "duration";
        (*property_map)[0x98A1CAB6] = "m_bUseBGRA";
        (*property_map)[0x3AD088B3] = "m_vPanDirection";
        (*property_map)[0xB7B86AFA] = "m_rSoundEvent_Drag_Body";
        (*property_map)[0xA7391556] = "m_FooArray";
        (*property_map)[0xB4FA5D46] = "m_fSneakNotMovingPrecisionModifier";
        (*property_map)[0x640E3BB4] = "m_rSoundEvent_Dump_Body";
        (*property_map)[0x1478EFB2] = "m_exitId";
        (*property_map)[0x74254EF9] = "m_bAggressive";
        (*property_map)[0x323644EA] = "m_rSoundEvent_Kick_Victim";
        (*property_map)[0xA2D56BC4] = "m_vAnchorPosOffset";
        (*property_map)[0xC0703C2D] = "m_rRagdollImpactAudioEvent";
        (*property_map)[0x4F7AE2C4] = "m_KnockdownsWhileConscious";
        (*property_map)[0x8643EFDD] = "m_bHasExited";
        (*property_map)[0x95FBB12E] = "m_path";
        (*property_map)[0x1650F97C] = "MetaData";
        (*property_map)[0xE67B2B29] = "fFarFov";
        (*property_map)[0x570FA0CB] = "m_rSoundEvent_Pull_Victim";
        (*property_map)[0xE8D71A3B] = "m_vStopDirection";
        (*property_map)[0xE8F2460F] = "m_eMoveSpeed";
        (*property_map)[0xCD89E6C7] = "m_bCanStandOnLedge";
        (*property_map)[0xE7D59CE0] = "m_bRegistered";
        (*property_map)[0x17CCE512] = "m_bEnablePickupOnThrowRelease";
        (*property_map)[0x9C98E907] = "m_pCompletedTextListResource";
        (*property_map)[0xBE3BAB4D] = "m_fMinTime";
        (*property_map)[0x37B0CC3C] = "m_nSkipRequestsPerDecal";
        (*property_map)[0xF9ECF870] = "m_vQuaternion";
        (*property_map)[0xE7888226] = "m_OutfitID";
        (*property_map)[0x9504E5CD] = "aScaleCurve";
        (*property_map)[0x36D6CCFB] = "m_eCameraRotationAllowed";
        (*property_map)[0x82D82718] = "cbMappingNames";
        (*property_map)[0x875AA3E9] = "bSpawnedByHitman";
        (*property_map)[0x44928BE4] = "distractionTarget";
        (*property_map)[0xB7234851] = "m_pMenuRoot";
        (*property_map)[0x53F10F26] = "m_rCrowdEntity";
        (*property_map)[0xEF556C2A] = "m_MathMultipliesData_Vector4";
        (*property_map)[0xCCC417A3] = "m_pMenuControllerEntity";
        (*property_map)[0x50A7CF37] = "m_bNetSynchOpenCloseEvents";
        (*property_map)[0x72235F8E] = "m_bWarning3Given";
        (*property_map)[0xC7EA82D3] = "m_pPeekCameraLookAtSurface";
        (*property_map)[0xEEEE898D] = "m_aExclusionZones";
        (*property_map)[0xDB1E625B] = "m_bStrafe";
        (*property_map)[0x5D068E05] = "m_bPauseGame";
        (*property_map)[0x390D46E9] = "m_bFadeToBlack";
        (*property_map)[0x6B5EB9A1] = "m_rInventoryItems";
        (*property_map)[0x60E99CA9] = "m_rObjectInteractingProxy";
        (*property_map)[0x392A65B6] = "m_fR";
        (*property_map)[0x26FABB6D] = "m_nTally";
        (*property_map)[0xFFAFF43E] = "m_bEnableBoneAttachmentOptimizationForCharacters";
        (*property_map)[0xEDB1569C] = "m_eSyncType";
        (*property_map)[0x80566B6B] = "m_rY";
        (*property_map)[0x67EE466C] = "m_rW";
        (*property_map)[0x9288DD0E] = "m_rLinkedEntity";
        (*property_map)[0x316C8A7E] = "m_aObserverGroupIDs";
        (*property_map)[0xE8B7BE43] = "a";
        (*property_map)[0x71BEEFF9] = "b";
        (*property_map)[0xB2CA6977] = "m_identifier";
        (*property_map)[0x69E2B068] = "m_ValueColorRGB";
        (*property_map)[0x3F1E03B9] = "m_sCollection";
        (*property_map)[0x256D2849] = "m_aBoneAttachers";
        (*property_map)[0xEDA07963] = "m_bGuardsOnly";
        (*property_map)[0x455FFC3F] = "m_fWorldMultiplier";
        (*property_map)[0xC9FA7F53] = "m_pText";
        (*property_map)[0x5A1A76E7] = "m_bGuarded";
        (*property_map)[0xD97C8C3A] = "m_pMaterialOverrideResource1";
        (*property_map)[0xE08469EE] = "m_fCurveTime";
        (*property_map)[0x1E55D0C6] = "m_repositoryId";
        (*property_map)[0x11B2192] = "m_BROutputs";
        (*property_map)[0x94CD042C] = "m_imageRID";
        (*property_map)[0x818E1B2E] = "m_fImageHeight";
        (*property_map)[0xCA3E5033] = "m_fStandNotMovingPrecisionModifier";
        (*property_map)[0xDD515C95] = "m_eCoverSize";
        (*property_map)[0x8BE84D5B] = "m_bIncludePlayer";
        (*property_map)[0x21D103E6] = "m_bDisableStaticShadows";
        (*property_map)[0xCC49E395] = "m_bCollision";
        (*property_map)[0xBB1962AF] = "m_fCoverLength";
        (*property_map)[0x4E69049D] = "m_fCoverDepth";
        (*property_map)[0x98A9BF86] = "selected";
        (*property_map)[0x7B20E16D] = "m_fScopeFOV";
        (*property_map)[0x100466FD] = "m_vAngularVelocity";
        (*property_map)[0x56B02ED] = "m_fPs3SplashLifetime";
        (*property_map)[0x20CD8214] = "m_bForceCoverSpot";
        (*property_map)[0x5D4ADAA5] = "m_bWindowCover";
        (*property_map)[0x5227B2A4] = "m_3dPromptPosition";
        (*property_map)[0xB98FCBEA] = "m_bStartEnabled";
        (*property_map)[0x8E8503E9] = "m_bRailStartEnabled";
        (*property_map)[0x215750E5] = "m_bHeroPushNPCOverRail";
        (*property_map)[0xDF11C39C] = "m_fExpiredTime";
        (*property_map)[0xAE05822C] = "m_fWidth";
        (*property_map)[0xD233232D] = "m_UseDiscreteVariance";
        (*property_map)[0xDE1F06] = "m_bHeroPacifyPushedNPC";
        (*property_map)[0xA7CBF4E7] = "disguiseName";
        (*property_map)[0xD26544F2] = "m_bHeroGiveFocusOnDumpBody";
        (*property_map)[0x90B4EAC1] = "m_bBackwards";
        (*property_map)[0xF52704CB] = "m_tTacticalPoint";
        (*property_map)[0x32AD22DA] = "m_pImage";
        (*property_map)[0x60D39B18] = "m_pDescription";
        (*property_map)[0x6D6BC05E] = "m_nEquippedItemIndex";
        (*property_map)[0x8830C0F2] = "m_bDisableRightHandGrip";
        (*property_map)[0x198A88F3] = "m_aValueTextEntities";
        (*property_map)[0x3D9AC49A] = "m_Crowd";
        (*property_map)[0x964F179B] = "sParamName";
        (*property_map)[0x95916441] = "m_eidParent";
        (*property_map)[0x1FAEDFBD] = "m_rDoorBurst";
        (*property_map)[0xFFA197F6] = "m_nGlowContractFadeSpeed";
        (*property_map)[0x14E43766] = "m_bUseEntityPosition";
        (*property_map)[0x3A9E6993] = "m_rActArmedTwohanded";
        (*property_map)[0x65053777] = "m_bTurnTowards1";
        (*property_map)[0x791FB705] = "m_GreenOfGreen";
        (*property_map)[0xEC07106B] = "m_bExitsOnInputRequest";
        (*property_map)[0xCFD71AB0] = "m_fTessellationMaxDist";
        (*property_map)[0xCB44DB9C] = "m_HDRLensFlareColorB";
        (*property_map)[0x16A8B05E] = "m_aDisableTextureStreaming";
        (*property_map)[0x922ADC11] = "m_fFogTurbulenceChaos";
        (*property_map)[0x4E2FDFD1] = "m_rController";
        (*property_map)[0x77722D49] = "nNeighbor7";
        (*property_map)[0x739DD18] = "m_bIgnoreTrespassing";
        (*property_map)[0xF525AB94] = "m_bStopActFast";
        (*property_map)[0x70E45352] = "m_aPauseActions";
        (*property_map)[0xE9581386] = "m_bFocusParentOnCancel";
        (*property_map)[0x117C12B9] = "m_aStopActions";
        (*property_map)[0x3440D3C9] = "m_fMaximum";
        (*property_map)[0x9C1F8171] = "m_aDialogs";
        (*property_map)[0xCE81CD9D] = "m_bEnableOutfitModifiers";
        (*property_map)[0x6ADF92E2] = "m_aDoneActions";
        (*property_map)[0x6564759D] = "m_bLevelCheck";
        (*property_map)[0x7D0EE860] = "m_bScreenAligned";
        (*property_map)[0xC8E2C45F] = "m_nReactionDirection2";
        (*property_map)[0x9D09C665] = "m_bProvidersStarted";
        (*property_map)[0xF6E1D981] = "m_pPlaystation4ImageRID";
        (*property_map)[0x36AD7FDF] = "m_bTerminateOnDone";
        (*property_map)[0x38200A6E] = "m_bInSequence";
        (*property_map)[0xF6BCCE70] = "m_nDramaStateFlags";
        (*property_map)[0x5FE0A18] = "m_bIsOnCooldown";
        (*property_map)[0xF121627F] = "m_bForceHideLoadingIndicator";
        (*property_map)[0x50F95492] = "canReload";
        (*property_map)[0xEB62CC7B] = "m_bInspected";
        (*property_map)[0xFBDCF099] = "m_fMax";
        (*property_map)[0xF2087A84] = "m_bFiberWireKill";
        (*property_map)[0x92DDAF7] = "m_bNeverSpectate";
        (*property_map)[0x9FE762FB] = "m_IgnoreVIP";
        (*property_map)[0xEE8C69CE] = "m_aPerceptionVolumes";
        (*property_map)[0x9DCF1897] = "m_Perspective_aspect";
        (*property_map)[0xF9473902] = "m_rPerceptorKeywordHolder";
        (*property_map)[0xB1989D4A] = "m_fRange";
        (*property_map)[0x7BFF3AC1] = "m_eAcknowledgeSoundDef";
        (*property_map)[0x15E4008A] = "m_bIsFalseAlarm";
        (*property_map)[0x3C3C8089] = "m_eStandDownSoundDef";
        (*property_map)[0x42501968] = "bContainedItemDetectedDuringFrisk";
        (*property_map)[0xF1CD5713] = "m_vBaseRotation";
        (*property_map)[0xB7F4444D] = "toPinName";
        (*property_map)[0xEA409A90] = "m_aBones";
        (*property_map)[0xE20B0B2F] = "m_bBlockDisguisePickup";
        (*property_map)[0x8715B235] = "m_aBoneIndices";
        (*property_map)[0x498B4BC] = "m_HitMaterial";
        (*property_map)[0x68E47F0D] = "m_fTriggerTime";
        (*property_map)[0xCC0C0838] = "m_eDistribution";
        (*property_map)[0xC0A2274B] = "m_bUseTraversalDistance";
        (*property_map)[0xAC14D67C] = "m_pVRHeadReplacement";
        (*property_map)[0x15E21493] = "m_bWalkable";
        (*property_map)[0x76C53330] = "m_rAccessoryItem";
        (*property_map)[0x3CA1ED6C] = "m_nGlowTagFocusTaggedFadeSpeed";
        (*property_map)[0x1C173300] = "fromPinName";
        (*property_map)[0x151DF000] = "m_bFootstepsLooping";
        (*property_map)[0xB577442B] = "m_fForce";
        (*property_map)[0xA4F03AA1] = "m_bLerpHDR";
        (*property_map)[0xE563605] = "m_fAmbientInscatterFraction";
        (*property_map)[0xD61973C9] = "Environments";
        (*property_map)[0xBDD7BE7D] = "m_nSelectedOptionID";
        (*property_map)[0x903F59AB] = "m_rSoundEvent_CloseCombat_Choke_Pulsing";
        (*property_map)[0x6B4F10B9] = "m_rVIPActor";
        (*property_map)[0x630F2041] = "m_bDisableAIBehavior";
        (*property_map)[0x86782B0D] = "m_fReloadSpeedFactorBonus";
        (*property_map)[0x66542349] = "m_aStartupAgencyPickupSelection";
        (*property_map)[0x9586FC1F] = "m_fGlobalLightOverridePhong";
        (*property_map)[0xA64B3887] = "m_tCornered";
        (*property_map)[0x72C5A166] = "m_bFirstFrame";
        (*property_map)[0x4D53F871] = "m_fDistortionBarrelScale";
        (*property_map)[0x60B8C1D6] = "m_AgentData";
        (*property_map)[0xF66FC29A] = "m_tSafeRoomStandDown";
        (*property_map)[0xD7C55EBF] = "m_DramaSetupData";
        (*property_map)[0xE037D515] = "m_fRadiusFalloff";
        (*property_map)[0xE15A5D4F] = "m_bInitialFlee";
        (*property_map)[0xB34D8111] = "m_safeRoomVIPSentActingStarted";
        (*property_map)[0x536DE27] = "m_nGridNodeIndex";
        (*property_map)[0x50A98C6D] = "m_rStateMachineCoordinator";
        (*property_map)[0xF616ADDB] = "m_rScheduler";
        (*property_map)[0xB5C0F593] = "m_guid";
        (*property_map)[0x42B382FE] = "m_aTimerEntityState";
        (*property_map)[0xD290D0CF] = "m_InitialState";
        (*property_map)[0x37469AC4] = "m_fReloadFactor";
        (*property_map)[0xD681B806] = "m_rInteraction";
        (*property_map)[0xB071FF9D] = "STATIC_COLLIDABLES_ONLY";
        (*property_map)[0x6C4DD8C8] = "m_bAutoSize";
        (*property_map)[0x30DDA9EB] = "m_rBodybag";
        (*property_map)[0xA060AE43] = "m_rActorRef";
        (*property_map)[0x31DF2321] = "m_bNeedsKnowledgeUpdate";
        (*property_map)[0x39F85C80] = "m_fWaitBetween";
        (*property_map)[0x1E787011] = "m_fInitialValue";
        (*property_map)[0xFCD8BF2E] = "m_uiModVolume";
        (*property_map)[0x607EC15] = "m_DistractionIsActive";
        (*property_map)[0x7740E4F1] = "m_uiModOutfit";
        (*property_map)[0x170DAC4E] = "m_bNotDisableHelpCivilian";
        (*property_map)[0xE2F17F88] = "fFarDistance";
        (*property_map)[0x67FBF89B] = "m_nCollisionMask";
        (*property_map)[0x7907114A] = "m_rPhysicsEntity";
        (*property_map)[0xA60C31EC] = "fFarScale";
        (*property_map)[0xC954F60C] = "m_secOrderDuration";
        (*property_map)[0x75DB8261] = "fNearFov";
        (*property_map)[0xE0DCE866] = "m_pSpatial";
        (*property_map)[0x1698A13B] = "m_bIgnoreZeroWeightEvents";
        (*property_map)[0x64B22844] = "fNearScaleFov";
        (*property_map)[0x63876AE] = "nWeaponType";
        (*property_map)[0xBF46E9FA] = "fFarScaleFov";
        (*property_map)[0xD0060E3] = "fNearAlpha";
        (*property_map)[0x9AAA2D02] = "fFarAlpha";
        (*property_map)[0xE59D5A49] = "fAlphaFactor";
        (*property_map)[0xB6292C79] = "aAlphaCurve";
        (*property_map)[0x1E0234FA] = "m_rForbiddenKeywords";
        (*property_map)[0x68D158AD] = "m_nToEntityID";
        (*property_map)[0xE635BA92] = "m_idScreenDecalsPairATex";
        (*property_map)[0xED5F4B1E] = "m_nToPinID";
        (*property_map)[0x5A1F34C2] = "m_sExposedEntity";
        (*property_map)[0x2BDEBBD] = "m_bSomeoneCanSeeBody";
        (*property_map)[0x418ABC46] = "m_MoveSpeed";
        (*property_map)[0x4536085A] = "m_sRawNavigationLabel";
        (*property_map)[0xEEFAA726] = "m_bDivide";
        (*property_map)[0x3A19CE14] = "m_aDialogIdentifiers";
        (*property_map)[0x824ABEBB] = "m_bMountableFromBottom";
        (*property_map)[0x64CAB092] = "m_fAimBlendInTime";
        (*property_map)[0x2B783E37] = "perks";
        (*property_map)[0x40DE0D8B] = "silencer";
        (*property_map)[0xBF396750] = "id";
        (*property_map)[0x36A4E3E0] = "nAmmoRemaining";
        (*property_map)[0xEBEEECF8] = "m_sCategory";
        (*property_map)[0xA1627914] = "nAmmoInClip";
        (*property_map)[0x740DB0DF] = "m_nAudioLowPassFilter";
        (*property_map)[0x3E7771A8] = "nItemHUDType";
        (*property_map)[0xC1D64CFC] = "m_EnterActs";
        (*property_map)[0x802FBCBE] = "itemCategory";
        (*property_map)[0x14C7C868] = "m_bAllowMouseCursor";
        (*property_map)[0x73578952] = "itemCategoryLKey";
        (*property_map)[0xC8681ECF] = "m_GrenadeTarget";
        (*property_map)[0x9F71678C] = "inventoryCategoryIcon";
        (*property_map)[0x507892DB] = "m_NumBlunt1HEmitters";
        (*property_map)[0x3530563D] = "sLabel";
        (*property_map)[0x41E30C44] = "m_rDoorMoving";
        (*property_map)[0x2B688700] = "m_fPrecisionUnaimedMaxDegree";
        (*property_map)[0xA126DF78] = "m_fTakeDownWindowIncFraction";
        (*property_map)[0x95DAB469] = "containsItem";
        (*property_map)[0x4C517186] = "m_sStringValue";
        (*property_map)[0x659DCF87] = "m_fBoneRadiusOverride";
        (*property_map)[0x2D8F5B65] = "m_bIsInside";
        (*property_map)[0xF290D181] = "nContainedItemHUDType";
        (*property_map)[0x20415756] = "m_SeasonOneHead1";
        (*property_map)[0xC5026F52] = "bContainedItemIllegal";
        (*property_map)[0x214EB7BD] = "m_fLegDamageMultiplier";
        (*property_map)[0x5FB1D744] = "m_vSlowDownScreenSize";
        (*property_map)[0x3DDF2B14] = "m_fShadowedRayleighDepthScale";
        (*property_map)[0xE5FF5C81] = "suspicious";
        (*property_map)[0x37CB1CB7] = "m_bPlacementOrientToHM";
        (*property_map)[0x935BA3A2] = "bContainedItemSuspicious";
        (*property_map)[0xEC48B318] = "m_fClampRadiusBlendSpeedOut";
        (*property_map)[0xDD38B5BD] = "m_bSlowDownTime";
        (*property_map)[0xD9342E24] = "illegal";
        (*property_map)[0xF6A56FAD] = "sPoisonType";
        (*property_map)[0x50302B7F] = "goalType";
        (*property_map)[0x459229A4] = "m_nFrontArcDeg";
        (*property_map)[0x9FB25E83] = "m_vOffSet";
        (*property_map)[0x5FE26FC6] = "m_rDetectionBox";
        (*property_map)[0xF3005BA7] = "m_aDetectionGeoms";
        (*property_map)[0x2BA1D468] = "notininventory";
        (*property_map)[0xF389286E] = "m_bAddChildren";
        (*property_map)[0x789A4EE6] = "m_bFPSSeperateFOV";
        (*property_map)[0x9681CDBA] = "m_bUseDynamicWaypoints";
        (*property_map)[0x8E805682] = "actionName";
        (*property_map)[0xEC82CE42] = "m_PenaltyLTMArrest";
        (*property_map)[0x1653E118] = "m_nReactionNearDelay2";
        (*property_map)[0xE0D5A86E] = "m_coliOut";
        (*property_map)[0x2271F94B] = "longDescription";
        (*property_map)[0x2593D6D9] = "m_pSvgMap";
        (*property_map)[0xDC461994] = "m_bAllowCloseCombat";
        (*property_map)[0xCB635669] = "m_eExitActionScared";
        (*property_map)[0x55CF8041] = "_dummy2";
        (*property_map)[0xDF21F98E] = "m_AnimList";
        (*property_map)[0x99997144] = "m_bShuffle";
        (*property_map)[0x63D5076E] = "m_bAimAt";
        (*property_map)[0xCCEC6F16] = "m_sSlotName";
        (*property_map)[0x8DD9739B] = "m_rPlayerSpreading";
        (*property_map)[0x37081EDB] = "m_bRunAIInGaps";
        (*property_map)[0x46299E09] = "m_bSynchFrameTimeWhenHidden";
        (*property_map)[0x4A16A1A7] = "m_ridDecalController";
        (*property_map)[0x8DF97497] = "m_eSpace";
        (*property_map)[0x5FEC3014] = "m_bAllowedInSequence";
        (*property_map)[0x386180E4] = "m_nSkipCollisionsPerDecal";
        (*property_map)[0x60D6E8D6] = "rSafeZone";
        (*property_map)[0x36DE7947] = "m_nTilesY";
        (*property_map)[0xDEEFC98C] = "m_bitfieldTestsEnabled";
        (*property_map)[0xB0ADFA99] = "m_bDisableBackgroundImageHandling";
        (*property_map)[0xB077416E] = "m_fAspectRatio";
        (*property_map)[0x2AFC50C4] = "m_fFocusConeRange";
        (*property_map)[0x6188E2EF] = "m_bTriggerToRayDistanceCheck";
        (*property_map)[0x84A81393] = "m_fLethalForce";
        (*property_map)[0xF472F84C] = "m_sShellImpactsSwitch";
        (*property_map)[0x6C1BD25B] = "m_aReservedApproachNodeIndices";
        (*property_map)[0xC9BCA90E] = "m_rSwitchHighPassFilter";
        (*property_map)[0x70BE4EF1] = "m_ItemType";
        (*property_map)[0xA2F8ADC0] = "m_objectiveType";
        (*property_map)[0x945C802D] = "m_eSizeConstraints";
        (*property_map)[0xDD4034E6] = "m_sRepositoryId";
        (*property_map)[0x44B9274D] = "m_fDecalSizeA";
        (*property_map)[0x83FEF5BD] = "FlagPositionX";
        (*property_map)[0xDDB076F7] = "m_fDecalSizeB";
        (*property_map)[0xA97143D] = "m_rWeaponOwner";
        (*property_map)[0xDBD9FD47] = "m_bOpenLogbook";
        (*property_map)[0x11A0D9C6] = "m_bAllowSecondaryGuardsToEvacuate";
        (*property_map)[0x8D3F9915] = "m_rBehaviorEntity";
        (*property_map)[0x5238A612] = "m_fGlowContractTargetSpecialAlternate";
        (*property_map)[0x87E9EE88] = "m_bEnableOverlay";
        (*property_map)[0x3BB4B9B8] = "m_helpType";
        (*property_map)[0xF3B25E03] = "m_nHitBoneIndex";
        (*property_map)[0xD680C660] = "m_fBloodSplatModifier";
        (*property_map)[0x9C3AA58E] = "m_vHitPos";
        (*property_map)[0x7F5BA8F] = "m_bUseConeLimits";
        (*property_map)[0xD3146E32] = "m_coliIn";
        (*property_map)[0xE5E19C1B] = "m_aListOptions";
        (*property_map)[0xD3C27D05] = "m_SourceGaits";
        (*property_map)[0xB642ABA0] = "m_Projectile";
        (*property_map)[0x6ACE0891] = "m_aExclusions";
        (*property_map)[0xF0DABAA3] = "eAttacher";
        (*property_map)[0xCEB8FDFA] = "m_TextLinePS4";
        (*property_map)[0x7DE0028E] = "m_bForwardDataToChildren";
        (*property_map)[0x27F7652E] = "m_aForceIncludes";
        (*property_map)[0xDCE6A2EE] = "m_OutfitId";
        (*property_map)[0x9E96AF3F] = "m_aProviders";
        (*property_map)[0x5C9F485B] = "m_aFilters";
        (*property_map)[0x80CC13DD] = "m_vFadeDistancePositive";
        (*property_map)[0xD5AB91FF] = "m_rOriginal";
        (*property_map)[0xB276BAC7] = "m_aInvestigateDisguiseGroups";
        (*property_map)[0x40518F11] = "m_bLOSMemberWantsToKill";
        (*property_map)[0x3CEBD0CA] = "m_fFastSpeedDelay";
        (*property_map)[0xD15BA34C] = "m_sOverrideIconId";
        (*property_map)[0x6F63FC0C] = "m_bDisplayAsNPCTarget";
        (*property_map)[0x97945C2C] = "m_bRequiresVisibleSignal";
        (*property_map)[0xD91A47D4] = "m_rIsBroken";
        (*property_map)[0xE514BAF5] = "m_aTargetKeywords";
        (*property_map)[0x5352C96] = "m_fCollisionForceReportingThreshold";
        (*property_map)[0xE8DC43E1] = "m_sMatchName";
        (*property_map)[0xEF5B0D00] = "m_deadCrowdBodiesDiscovered";
        (*property_map)[0x707A6524] = "stringField";
        (*property_map)[0x34089A8F] = "m_bCollisionGroup5";
        (*property_map)[0x14227498] = "m_bIsInLockdown";
        (*property_map)[0x7FE3F53B] = "aIcons";
        (*property_map)[0x9A4BFFA] = "m_fAnimTime";
        (*property_map)[0x532003B0] = "m_fMaxDist";
        (*property_map)[0xF7A34F37] = "m_tEngageTime";
        (*property_map)[0x763B7F8D] = "m_aSearchActs";
        (*property_map)[0x23B37A0D] = "m_bTurnOn";
        (*property_map)[0x97E7FBCE] = "m_fFrame";
        (*property_map)[0x48C037B4] = "m_bIsCompletable";
        (*property_map)[0x14995236] = "m_pProjectedReaction";
        (*property_map)[0x103547FE] = "m_bPosition";
        (*property_map)[0xA4E0C28C] = "m_bCanBeInterrupted";
        (*property_map)[0x61197735] = "m_aActorIKControllers";
        (*property_map)[0x6429FFD6] = "m_completed";
        (*property_map)[0x6741F2B0] = "m_aActorTemplates";
        (*property_map)[0xDD24DB10] = "m_rSwitchAudioHeadType";
        (*property_map)[0xDC8CE0D7] = "m_SwitchArmedItemKeyword";
        (*property_map)[0x1B628C14] = "m_rSwitchExoticType";
        (*property_map)[0x470EC9FA] = "m_fAlphaScale";
        (*property_map)[0xB79B695B] = "m_fFocusConeBackWidth";
        (*property_map)[0xA0A65977] = "m_rSwitchLowPassFilter";
        (*property_map)[0x3613446] = "m_fAttentionGainModifier";
        (*property_map)[0xFC994C4] = "m_bEnabledInExternalBuilds";
        (*property_map)[0xDCC211E9] = "m_fPrecisionShotFOV";
        (*property_map)[0x7D05CC92] = "m_sKeyName";
        (*property_map)[0x99A84FA4] = "nValue";
        (*property_map)[0x1CFA952F] = "m_HeadOffsetNear";
        (*property_map)[0x815E48D7] = "m_fCameraZoomTime";
        (*property_map)[0xB997755] = "m_eInputMode";
        (*property_map)[0x71469C11] = "propertyName";
        (*property_map)[0x26B74055] = "m_aSecurityDevices";
        (*property_map)[0x97DB9C9E] = "m_bFirstWait";
        (*property_map)[0x19F5EB08] = "m_bIsSitting";
        (*property_map)[0xC5E1C0D1] = "m_aActs";
        (*property_map)[0xB053FF25] = "m_aAreaVolumes";
        (*property_map)[0xF6FF8CC2] = "m_rSoundEvent_BulletImpactBody";
        (*property_map)[0x9B45E0DB] = "m_ImmediateWarningDialog";
        (*property_map)[0x45584A12] = "m_BehaviorTrees";
        (*property_map)[0xB7867547] = "m_rContainerGeom";
        (*property_map)[0xB3E9F5A7] = "m_bAutoCalculateMinDistance";
        (*property_map)[0x970AE174] = "m_RoomRight";
        (*property_map)[0x7316980B] = "m_rConfigurator";
        (*property_map)[0x82A6022A] = "m_rStorage";
        (*property_map)[0xA5D7DF7] = "m_OnlyAllowProfessionsIfInAllRules";
        (*property_map)[0x51A7FD0C] = "m_funcname";
        (*property_map)[0xC6FA0A31] = "m_fFov1Max";
        (*property_map)[0x7445848B] = "m_fSwing1Restitution";
        (*property_map)[0x26FF8C13] = "m_bIsIslandEntrance";
        (*property_map)[0x1F21DD31] = "m_bGetHelpGroupStarted";
        (*property_map)[0xE8429A86] = "m_fFov2Min";
        (*property_map)[0xF718ABFB] = "m_bShowingWarning";
        (*property_map)[0xAF9B6BF4] = "m_fBlendIn1";
        (*property_map)[0xD6AEC848] = "m_fArrayIndexB";
        (*property_map)[0x36923A4E] = "m_fBlendIn2";
        (*property_map)[0xD3441C81] = "m_bAnnounceHunt";
        (*property_map)[0x75A471E0] = "m_fCoolDown";
        (*property_map)[0x5C057629] = "m_eSound";
        (*property_map)[0xFD2347AF] = "m_CarriedItem";
        (*property_map)[0xCDE7D565] = "m_eSoundLoudness";
        (*property_map)[0x42CB28A5] = "m_pIdleAnimNetworkEntity";
        (*property_map)[0xD7158771] = "m_rIActor";
        (*property_map)[0xE9ECC8D7] = "m_eInstinctGlow";
        (*property_map)[0xFBCB6A21] = "m_fLoudness";
        (*property_map)[0xBF7890C0] = "m_eHasOutputEval";
        (*property_map)[0x21881FBF] = "m_pActorModel";
        (*property_map)[0x264C375D] = "m_fGlobalShadowMapNormalOffsetBias";
        (*property_map)[0xD9ED9DDF] = "m_nMinSizeScale";
        (*property_map)[0xDDF4B9A7] = "m_pRagdollResource";
        (*property_map)[0xC8856CC2] = "m_aControlPointEntities";
        (*property_map)[0xCDEDD9AE] = "m_aFocalPoints";
        (*property_map)[0xD03493F2] = "m_bOdometerSneaking";
        (*property_map)[0xC14FABDE] = "m_pBodybagResource";
        (*property_map)[0x2F3A74E9] = "m_nDelayOnTrue";
        (*property_map)[0x8F46A049] = "m_rClothbundle";
        (*property_map)[0x2F776CF4] = "m_bIsValid";
        (*property_map)[0x4F38BA1] = "m_bUseSlimActorCapsules";
        (*property_map)[0x6EE39A54] = "m_aDisguisesAllowedDisabled";
        (*property_map)[0xC06A5F18] = "m_pMaterialOrgResource3";
        (*property_map)[0xAF4D374] = "m_rCrowd";
        (*property_map)[0x65625189] = "m_eIdleGait";
        (*property_map)[0xB618C710] = "m_SpatialTarget";
        (*property_map)[0xDC4176F4] = "m_sHudIconTracked";
        (*property_map)[0xF2D39991] = "m_nPushCollisionRadius";
        (*property_map)[0xD1E4CF6] = "m_bCanActorsBePushed";
        (*property_map)[0x257D0982] = "m_NumAxePullEmitters";
        (*property_map)[0xFA559779] = "m_bCanUseLeftHand";
        (*property_map)[0x42D9C6F9] = "m_bActorCollision";
        (*property_map)[0xDB8D817B] = "m_eval";
        (*property_map)[0xD8889297] = "m_bAllowPossession";
        (*property_map)[0x400A25C5] = "m_vParticleFxLockedCenter";
        (*property_map)[0x3B4ACA4D] = "sContainedItemName";
        (*property_map)[0x3CD79782] = "m_rIntel";
        (*property_map)[0x179566F6] = "m_bSwapHalfSpaces";
        (*property_map)[0x63932D25] = "m_aActivityPullBoxes";
        (*property_map)[0xEC4670EA] = "m_bCameraSide";
        (*property_map)[0x97E2B531] = "m_bUsePerSpotIdleGait";
        (*property_map)[0x279ECECB] = "m_bFillAtInit";
        (*property_map)[0xFE33B78F] = "m_pChildNetwork";
        (*property_map)[0x4F1E33F1] = "m_bAllowScaling";
        (*property_map)[0xE6E29976] = "m_aActivitySpots";
        (*property_map)[0x8E83B475] = "m_aBytes";
        (*property_map)[0xE5B940D6] = "m_aExcludedActorTemplates";
        (*property_map)[0x489AA2D5] = "m_bCanBeDynamicallyCreated";
        (*property_map)[0x933DBE97] = "m_eGroupState";
        (*property_map)[0xE7A7914C] = "m_fTargetAgitationCooldownTimer";
        (*property_map)[0x3764CBFB] = "m_aFilterTypes";
        (*property_map)[0x5FBEE38D] = "m_aCandidates";
        (*property_map)[0x4E2FCF37] = "m_eMonitoredEvent";
        (*property_map)[0x7C6D1FBA] = "m_pCandidate";
        (*property_map)[0x867C6FFD] = "m_eSizeMode";
        (*property_map)[0x6154D18E] = "m_nReactionNearDelay3";
        (*property_map)[0x2814B2B3] = "m_fZoomSensitivity";
        (*property_map)[0x7F875879] = "m_bTargetOnStairs";
        (*property_map)[0xB968064] = "m_fEstimatedDuration";
        (*property_map)[0xBA8CAC69] = "m_pNorthIndicator";
        (*property_map)[0xC59DFC78] = "m_nGC_DebugCurrentTrack";
        (*property_map)[0x26E0C8C8] = "m_bLastGetHelpGroupFailed";
        (*property_map)[0x928B6CF] = "m_futureStateProvider";
        (*property_map)[0x30DF8FEC] = "m_ridAlphaCurveEntity";
        (*property_map)[0xBC1BBAE6] = "m_nGlowObjectivesFadeSpeed";
        (*property_map)[0xD1518EE7] = "sDeepString";
        (*property_map)[0xE59776B5] = "m_rHitmanCharacter";
        (*property_map)[0xF1FF0CAF] = "m_lookToDirection";
        (*property_map)[0xE9133A58] = "m_bReset";
        (*property_map)[0x645EC272] = "bIsEnabled";
        (*property_map)[0x1098ACA] = "m_nNumInputChannels";
        (*property_map)[0x3DB567B] = "m_fSwing2Restitution";
        (*property_map)[0x18718CEA] = "bIsHighlighted";
        (*property_map)[0x3B1BDB45] = "m_rConnectorSpatial";
        (*property_map)[0x43BF257] = "m_bDepthOfFieldUseNew";
        (*property_map)[0x5952DF2E] = "extraData";
        (*property_map)[0x9D3CCA5C] = "m_Spatials";
        (*property_map)[0xCD586E74] = "nSlot";
        (*property_map)[0xF68C1F42] = "m_fSplashOffset";
        (*property_map)[0x8EBBFF14] = "m_rHoldTowardPos";
        (*property_map)[0xA2D43A60] = "sContractType";
        (*property_map)[0x3647C6D5] = "sLocationId";
        (*property_map)[0x21524807] = "m_civilianJoinReason";
        (*property_map)[0x22277EA9] = "m_HDRLensFlareColorD";
        (*property_map)[0x8B0D239B] = "m_aEventResponders";
        (*property_map)[0xC0E4F642] = "m_pTextListResource";
        (*property_map)[0x5792B483] = "bIsOnline";
        (*property_map)[0x6727B49A] = "m_rValueYEntity";
        (*property_map)[0xAEA122CD] = "aScreenShot";
        (*property_map)[0x21C23597] = "m_fAttractDistance";
        (*property_map)[0xCF4BF964] = "aRequirements";
        (*property_map)[0x794CD521] = "m_bHunting";
        (*property_map)[0xA6C0B0CB] = "m_Entities";
        (*property_map)[0x8D000989] = "facing";
        (*property_map)[0xB834B99C] = "m_TriggerOnActorDead";
        (*property_map)[0x25BAF6B] = "m_eChannel";
        (*property_map)[0x56117719] = "m_rWaker";
        (*property_map)[0xA4FB999F] = "m_bUseActual";
        (*property_map)[0xC82BAB4D] = "m_fOcclusionMinRadius";
        (*property_map)[0xA0AED68] = "m_eBoneColor";
        (*property_map)[0x56103966] = "m_eRoomBehaviour";
        (*property_map)[0x2A42CD10] = "m_aSupportedMovements";
        (*property_map)[0xC78401E5] = "m_vDestViewportA";
        (*property_map)[0x32D7B8D2] = "m_nViewportVisibility";
        (*property_map)[0x2E923F67] = "m_rHM53rdPersonCamProfile";
        (*property_map)[0x306941CC] = "m_sInputPin";
        (*property_map)[0xCEDC202] = "m_ResumingConditions";
        (*property_map)[0xAFDFDEF1] = "m_CurveX";
        (*property_map)[0x5F4CCF5D] = "pathPoints";
        (*property_map)[0xF7C7EF8D] = "m_pMeshResource";
        (*property_map)[0x3BC6005D] = "m_pctAlpha";
        (*property_map)[0xAE7BBCAC] = "m_pMaterialOverrideResource0";
        (*property_map)[0xD71F41CC] = "m_bUseCollisionHeight";
        (*property_map)[0x5D892F63] = "m_fLimit";
        (*property_map)[0x2E643E34] = "m_pMaterialOrgResource1";
        (*property_map)[0x36D479CC] = "m_aDebugEntities";
        (*property_map)[0x831F812C] = "m_nGlowTraversalFadeSpeed";
        (*property_map)[0x53F36F66] = "m_pMinimapViewControl";
        (*property_map)[0x1CB2B9B0] = "m_rPlaybackEmitter";
        (*property_map)[0x6264EF42] = "m_fGlowPlayerSeenInLVAOutlineWidth";
        (*property_map)[0x6229EF1F] = "m_fMaxPlaybackDistance";
        (*property_map)[0x3C68A885] = "m_bCoverTakedown";
        (*property_map)[0x13FB2706] = "m_pPreferredActor";
        (*property_map)[0xC65FA6B6] = "m_nNumberOfEvents";
        (*property_map)[0xEA863DE2] = "m_nNumEmitters";
        (*property_map)[0xD7DCF420] = "m_tension";
        (*property_map)[0xC326D131] = "m_bDefaultAddToHistory";
        (*property_map)[0xE3DCACA4] = "m_fConeDistanceFadeRange";
        (*property_map)[0x3FEBC00C] = "m_aApproachPath";
        (*property_map)[0x1CAF4F00] = "m_fLowPassFilter";
        (*property_map)[0x89CE0689] = "m_OpenDir";
        (*property_map)[0x86A392DD] = "bIsFirearm";
        (*property_map)[0x788579D0] = "m_fSpeedSmoothingDuration";
        (*property_map)[0xAAD7361D] = "m_aReferences";
        (*property_map)[0xABD5230F] = "m_bEnteredFromSide0";
        (*property_map)[0x2D17BA18] = "m_fFocusConeHeightAngle";
        (*property_map)[0x8A9C4A49] = "m_rVolumeShape";
        (*property_map)[0xC1653B] = "m_iRoomOcclusionPriority";
        (*property_map)[0x85F793E8] = "StimulusPools";
        (*property_map)[0xC4FB8126] = "m_sObjectivesCounterHeader";
        (*property_map)[0x7453D996] = "m_fDefaultOcclusionFactor";
        (*property_map)[0xEEC282D2] = "m_pDetachPoint";
        (*property_map)[0xDFD2327C] = "m_bAwake";
        (*property_map)[0x3816FACB] = "m_pPauseAllEvent";
        (*property_map)[0xBCA1005E] = "m_bUnconscious";
        (*property_map)[0xF62829FC] = "entityId";
        (*property_map)[0xA9B5DF46] = "m_bAlive";
        (*property_map)[0x4D1CA6B4] = "m_bIsOrderValid";
        (*property_map)[0x5677E8E4] = "m_bPeekingConsideredIllegal";
        (*property_map)[0x51632D8] = "m_Text";
        (*property_map)[0x52F0E79C] = "m_rHitmanPeekingPosition";
        (*property_map)[0xF216FE3C] = "m_rPeekCameraProfile";
        (*property_map)[0x37D05E13] = "m_nFieldSize";
        (*property_map)[0x1A6B25D2] = "m_bAllowSelectInput";
        (*property_map)[0x9EE61B3C] = "m_fWeight";
        (*property_map)[0x96E228B6] = "m_bMustAlwaysBeUsed";
        (*property_map)[0x4C8E88F8] = "m_CrowdAttentionCurve";
        (*property_map)[0x36D849CB] = "m_sourceClips";
        (*property_map)[0x1F50EEB7] = "m_TargetGaits";
        (*property_map)[0x43891DEE] = "m_bCheckLOS";
        (*property_map)[0x861AB5A8] = "m_GlowSetPieceColor";
        (*property_map)[0x97982733] = "m_eReactionType1";
        (*property_map)[0xAA693972] = "m_hint";
        (*property_map)[0x38918BE6] = "m_bReasoning";
        (*property_map)[0x6425ABB6] = "m_aModifierIds";
        (*property_map)[0x51EB95E5] = "m_nReactionDirection1";
        (*property_map)[0xEBEC6B1E] = "m_fLevelOutThresholdTimeStand";
        (*property_map)[0x3BDF4AA4] = "m_nReactionRadius1";
        (*property_map)[0xD73F312B] = "m_nCrowdLODDist0";
        (*property_map)[0x89946A1] = "m_vDistortionBarrel";
        (*property_map)[0x349F5089] = "m_bWaitingValue";
        (*property_map)[0x7F7CA43B] = "m_nReactionAngle1";
        (*property_map)[0xE6D7110C] = "m_nDecalBuffer";
        (*property_map)[0x2B0F46F6] = "m_fFaceLeftRightTarget";
        (*property_map)[0x907D7B9A] = "m_aModelBodyPartSelections";
        (*property_map)[0xF8AE685C] = "m_nReactionFarDelay1";
        (*property_map)[0x5FE2F6CB] = "m_ePendingDeathContext";
        (*property_map)[0x80B7174A] = "m_nBoneID";
        (*property_map)[0xE917689] = "m_eReactionType2";
        (*property_map)[0x9F978926] = "m_bCloseCombatProps";
        (*property_map)[0x6FD2E573] = "m_bStartedInvestigateDialog";
        (*property_map)[0xCA3F22A8] = "m_nReactionTimeOut2";
        (*property_map)[0xA2D61B1E] = "m_nReactionRadius2";
        (*property_map)[0xE675F581] = "m_nReactionAngle2";
        (*property_map)[0x2E06797E] = "m_tLastPropagate";
        (*property_map)[0x21084E9C] = "m_nPOIHeightOffset2";
        (*property_map)[0xDEE9F231] = "m_bRemoveNeverFlee2";
        (*property_map)[0x2DC1B066] = "m_bActivatedStart";
        (*property_map)[0x1E5BFE03] = "m_HintTimeOut";
        (*property_map)[0x54BB785B] = "fUInt32Prop";
        (*property_map)[0x66ACB4FA] = "m_fLinearDamping";
        (*property_map)[0x7996461F] = "m_eReactionType3";
        (*property_map)[0xEC285301] = "m_jsonArguments";
        (*property_map)[0xBFE5F4C9] = "m_nReactionDirection3";
        (*property_map)[0xEE647576] = "fGridSpacing";
        (*property_map)[0xBD38123E] = "m_nReactionTimeOut3";
        (*property_map)[0xEC833A3E] = "m_rTriggerAlarmGroup";
        (*property_map)[0xD5D12B88] = "m_nReactionRadius3";
        (*property_map)[0x7E95287C] = "m_bEnableAimAssist";
        (*property_map)[0x36C07F3A] = "m_bReachedOnNear";
        (*property_map)[0x81808E94] = "m_CombatProgress";
        (*property_map)[0x560F7E0A] = "m_nPOIHeightOffset3";
        (*property_map)[0xDF574942] = "m_eKeywordEval";
        (*property_map)[0xE833BEEA] = "m_StabEmitter";
        (*property_map)[0xEA4EC26] = "m_tStandDownDelay";
        (*property_map)[0x2F1F500] = "m_fMinRotationSpeed";
        (*property_map)[0x75BB0EA6] = "m_bIgnoreForProjection";
        (*property_map)[0xA409C938] = "m_bDestructionEnabled";
        (*property_map)[0x53084E5D] = "m_fMaxRotationSpeed";
        (*property_map)[0x2B9A2D52] = "m_bPassthroughOnClient";
        (*property_map)[0x5837C9C] = "m_eExclusionLayer";
        (*property_map)[0xDA2B2AC2] = "m_entityGroup";
        (*property_map)[0x53A77455] = "m_fRotationOffsetRoll";
        (*property_map)[0x54A4C95C] = "m_eHelpReason";
        (*property_map)[0x19577510] = "editorOnly";
        (*property_map)[0x4B05CCA4] = "m_nIntValue";
        (*property_map)[0xF4FBB477] = "m_pDescriptionResourcePS4Override";
        (*property_map)[0x4A4761EF] = "m_bSetIfEqual";
        (*property_map)[0x69F66C21] = "m_ObjectiveStringID";
        (*property_map)[0x6EE13023] = "m_rLogic";
        (*property_map)[0x8AF989B4] = "m_nTallyRequired";
        (*property_map)[0x969ADB06] = "m_bShowInHUD";
        (*property_map)[0xEA5C3BC8] = "m_aGameChangers";
        (*property_map)[0x1D8B4968] = "m_eStateStatus";
        (*property_map)[0xD7878D8E] = "m_sCustomSaveTitle";
        (*property_map)[0x7CF5ECFD] = "m_bExemptFromAttentionHUD";
        (*property_map)[0xE890357B] = "m_fBlodSplatSizeMultiplier";
        (*property_map)[0xC0F115F6] = "m_fResourceTest";
        (*property_map)[0x5C5D2057] = "m_bRestoreFromStop";
        (*property_map)[0xBC0472AC] = "m_pIdleAct";
        (*property_map)[0xCF1FDEA6] = "m_bAllowMoving";
        (*property_map)[0x118FE6CF] = "m_rPreferredNextNode";
        (*property_map)[0x586EF4DB] = "m_bTwoWay";
        (*property_map)[0x8BB1B0C1] = "m_bStopCurrentOrder";
        (*property_map)[0x89E220C0] = "m_fDistancePenalty";
        (*property_map)[0x453A5CC8] = "m_bDisbanded";
        (*property_map)[0x1B09312F] = "m_bNPCShotProcessed";
        (*property_map)[0x2ABD43F2] = "Id";
        (*property_map)[0x4451ED0F] = "m_fRadialBlurriness";
        (*property_map)[0xAC96C91C] = "m_fPeripheralLow";
        (*property_map)[0x1B59ED8E] = "m_fCausticsDistortionScale";
        (*property_map)[0x88F93B9B] = "m_fSneakMovingPrecisionModifier";
        (*property_map)[0xE3F48D31] = "m_fFocusConeWidthAngle";
        (*property_map)[0x4F4A0309] = "m_Stage";
        (*property_map)[0xED78725D] = "m_vProjectorMapOffset";
        (*property_map)[0x3E68D876] = "m_nMax";
        (*property_map)[0xB9DE6378] = "m_eBoneId";
        (*property_map)[0x4E949DB0] = "m_fCameraCutTriggerDistance";
        (*property_map)[0x9D488733] = "m_aIllegalKeywords";
        (*property_map)[0x1510BFBB] = "m_bClearOnStop";
        (*property_map)[0x6C2A2E01] = "m_rRefencingEntity";
        (*property_map)[0x93C03FF2] = "m_fActivationRadius";
        (*property_map)[0x58E60BDC] = "m_bCastShadowsPC";
        (*property_map)[0xEC53CC08] = "m_bRememberLeavingActor";
        (*property_map)[0xEF8C75E1] = "m_fSeekPosition";
        (*property_map)[0xA7C1BD58] = "m_GreenOfBlue";
        (*property_map)[0xFFE0F9DB] = "m_fDamageThreshold";
        (*property_map)[0x625643A4] = "m_fHeadAnchorOffsetLeft";
        (*property_map)[0xC8114014] = "m_nShotsThreshold";
        (*property_map)[0xEF572905] = "m_bIgnoreStatic";
        (*property_map)[0xAB37352D] = "m_bDrawGraph";
        (*property_map)[0x728357E8] = "m_rEventSender";
        (*property_map)[0x616E1EE6] = "m_fDampening";
        (*property_map)[0xB2CD41CD] = "m_StoredSlot";
        (*property_map)[0x3CE0B505] = "m_aItemRepositoryIds";
        (*property_map)[0xF72961ED] = "m_fLevelOutThresholdTimeStickVert";
        (*property_map)[0xA74A8CCD] = "m_fLevelOutAngle";
        (*property_map)[0x5CEEA9C0] = "m_cInputSensitivityCurve";
        (*property_map)[0xDE041EF6] = "m_aItems";
        (*property_map)[0x89295FFD] = "m_fCameraAllowSmoothingAtLimits";
        (*property_map)[0xAD58418C] = "m_iWarningCount";
        (*property_map)[0xECB6379B] = "m_Curve0";
        (*property_map)[0x4E3F226F] = "m_nIndex";
        (*property_map)[0x3968A08A] = "m_fHealthPercentageToFall";
        (*property_map)[0xE12E9729] = "m_aListeningForData";
        (*property_map)[0xBD49B877] = "m_rBlockingSpatialEntity";
        (*property_map)[0x13345EF6] = "m_cDistanceFalloff";
        (*property_map)[0x768CF8D] = "m_pCustomNetworkEntity";
        (*property_map)[0x18FD3BF1] = "m_fEffectiveRange";
        (*property_map)[0x990710E7] = "m_bOneHitpoint";
        (*property_map)[0x7BEDD2A9] = "m_fMaxRange";
        (*property_map)[0xDEDD26A3] = "m_fFovYDeg";
        (*property_map)[0x7A558BB4] = "mTrajectoryQuaterion";
        (*property_map)[0x4CFB1DE5] = "m_PenaltyLTMCombat";
        (*property_map)[0xE6EC6649] = "m_fZoomedOutLevel";
        (*property_map)[0x32125774] = "m_aPhysicsObjects";
        (*property_map)[0x8B3EA0E2] = "m_bFireWeapon";
        (*property_map)[0xC7CAF13F] = "m_bCloseCombat";
        (*property_map)[0x1B8C1CB7] = "m_fRightRPM";
        (*property_map)[0xDE42DE30] = "m_bSpatial";
        (*property_map)[0xF31BDB55] = "m_bCrossdressingIsOk";
        (*property_map)[0xB1A7D64D] = "sWeaponName";
        (*property_map)[0x611D8CA1] = "m_eGenderRequirement";
        (*property_map)[0xA8CF9AFA] = "m_rValue1";
        (*property_map)[0x7C5D4242] = "m_VolumeTriggersData";
        (*property_map)[0x39380A48] = "m_fAgilityPeripheralAngle";
        (*property_map)[0x57654051] = "m_opportunities";
        (*property_map)[0x10743CD9] = "m_fBottom";
        (*property_map)[0x8E9BA72] = "m_fDamageMedium";
        (*property_map)[0x19D037B3] = "m_rResult";
        (*property_map)[0xAFD087F9] = "skipStandAndObserve";
        (*property_map)[0x6ECECD58] = "m_bSCIcons";
        (*property_map)[0x77637330] = "m_rStartTrack";
        (*property_map)[0xF8054660] = "m_bSCAttention";
        (*property_map)[0x42926567] = "m_ParticleController";
        (*property_map)[0x2456BE78] = "m_fInsidenessInterpolationDistance";
        (*property_map)[0x2BD56221] = "m_ParticleShaderParameters";
        (*property_map)[0x40A12CEA] = "m_ParticleRender";
        (*property_map)[0xFDA09612] = "m_FireMaxSeconds";
        (*property_map)[0xFB65F726] = "act_false";
        (*property_map)[0x83C1FEF1] = "m_TriggerBox";
        (*property_map)[0xDB57D5E3] = "m_testCases";
        (*property_map)[0x7583FFB7] = "m_sourcesProvider";
        (*property_map)[0x997C0326] = "m_fHitmanDamage";
        (*property_map)[0x7BB8EF94] = "m_fSpeedMin";
        (*property_map)[0x21705D49] = "m_rAnimationSet";
        (*property_map)[0x47B5D0CD] = "m_fSpeedMax";
        (*property_map)[0xED638CEC] = "m_fInnerConeAngle";
        (*property_map)[0xA040DBBE] = "m_fOuterConeAngle";
        (*property_map)[0xB3E5083B] = "m_fEmissionTimeSeconds";
        (*property_map)[0x704E288A] = "m_fParticlesPerSecond";
        (*property_map)[0x85A03872] = "m_fParticlesPerSecondMax";
        (*property_map)[0xD01A5253] = "m_fFixedEmissionCount";
        (*property_map)[0x2014333A] = "m_bAssignDynamicEntityId";
        (*property_map)[0xF614E916] = "m_fFudge";
        (*property_map)[0xE2FDEFBD] = "m_fMovementMovingTimeUnchanged";
        (*property_map)[0xB890E503] = "m_fRoomTagDistance";
        (*property_map)[0x63E41730] = "m_fDistStart";
        (*property_map)[0xDA18FE44] = "m_bLedges";
        (*property_map)[0x60D851F7] = "m_aRequiredOutfit";
        (*property_map)[0x97B121FE] = "m_fLabsDofSwirl";
        (*property_map)[0x3F2E5E14] = "m_fEmissionCameraProximity";
        (*property_map)[0x1209A52E] = "m_fInscatterIntensity";
        (*property_map)[0xE925EC0] = "m_View_translation";
        (*property_map)[0xC3143346] = "m_bDbgAreaIsRegistered";
        (*property_map)[0x3606BD5D] = "m_bEmitterSpaceParticles";
        (*property_map)[0x8C686A45] = "m_nFxDrawBuffer";
        (*property_map)[0xDF4FE4AC] = "m_bSendHaveActiveParticles";
        (*property_map)[0x919A4574] = "m_rSubaction";
        (*property_map)[0x8C6DF120] = "m_bLegalizeWeapons";
        (*property_map)[0x8D55BCC0] = "m_fHFVerticalInt";
        (*property_map)[0xD586223C] = "m_Areas";
        (*property_map)[0xF2A1E162] = "m_nAIPoolSize";
        (*property_map)[0x22E0427A] = "defaultSpeak";
        (*property_map)[0x46E178FD] = "m_eMinSoundPerception";
        (*property_map)[0x620F05C] = "m_sDefaultState";
        (*property_map)[0x6B13261B] = "m_pStartPosition";
        (*property_map)[0xAEE87D4] = "m_pDescriptionResource";
        (*property_map)[0x331CE1D7] = "m_pDescriptionResourceKeyOverride";
        (*property_map)[0xC532408E] = "m_pDescriptionResourcePCOverride";
        (*property_map)[0xAB67CB99] = "m_GlowTagFocusUntaggedOutlineColor";
        (*property_map)[0x4768AD48] = "m_pAcceptActionLabel";
        (*property_map)[0x4FB87BA8] = "m_pImageRID";
        (*property_map)[0xEFB56943] = "m_eSplashHintType";
        (*property_map)[0xC4F49B1A] = "m_AdditionalAnimSets";
        (*property_map)[0x234F45AA] = "m_Message";
        (*property_map)[0xFF04BF7F] = "m_eUsableConditionKeywordsBehaviorB";
        (*property_map)[0xC20032EB] = "m_fTriggerAngleMin";
        (*property_map)[0x96A7D8D7] = "m_bExclusive";
        (*property_map)[0xB43EC739] = "m_rTextResource";
        (*property_map)[0xFE0D0DB2] = "m_fTriggerAngleMax";
        (*property_map)[0x2A1DC426] = "m_bDrawTriggerFan";
        (*property_map)[0xE3CABC9D] = "m_vGlowTagFocusUntaggedVignetteRadii";
        (*property_map)[0x85E52E4A] = "m_pMenus";
        (*property_map)[0x5CD3EC6D] = "m_overrideOriginalCamera";
        (*property_map)[0x891B8C8E] = "m_bOriginalCameraDisabled";
        (*property_map)[0x2652E6FF] = "m_Geom";
        (*property_map)[0xFBF9B897] = "m_fAngles";
        (*property_map)[0x500A92CF] = "m_nForcedAnimationNode";
        (*property_map)[0xA2064964] = "m_nDisturbanceRes";
        (*property_map)[0xF6CE0FF7] = "m_aCombatPosition";
        (*property_map)[0xEE3C93ED] = "m_rChildNetworkAct";
        (*property_map)[0x6F45D558] = "m_rListener";
        (*property_map)[0xA81BD70D] = "m_vOriginPosition";
        (*property_map)[0xB1E031B9] = "Trans";
        (*property_map)[0x9F1E2D9C] = "references";
        (*property_map)[0xD4A9484] = "m_sHeadline";
        (*property_map)[0x1AD806B3] = "m_sBodytext";
        (*property_map)[0x726107CA] = "m_rWorldTransform";
        (*property_map)[0x3AF22D55] = "m_aEmitterRefs";
        (*property_map)[0x97BE0700] = "m_pHoverView";
        (*property_map)[0xAD9F1220] = "m_DestBusId";
        (*property_map)[0x3CBE9DAC] = "m_vValue";
        (*property_map)[0x95D7BBA4] = "m_fBaseVolume";
        (*property_map)[0xDEF95B09] = "m_fEmitterListenerVolume";
        (*property_map)[0xF4B2980D] = "m_RedOfBlue";
        (*property_map)[0x941AC3FA] = "m_bTransitionWhileMoving";
        (*property_map)[0x51227850] = "m_bIsMicSend";
        (*property_map)[0xF9F7CC48] = "m_fConditionValidDistance";
        (*property_map)[0x89530716] = "m_aSuspiciousKeywords";
        (*property_map)[0xC8710931] = "m_bTurn";
        (*property_map)[0x587831D4] = "m_bFullAutoOperation";
        (*property_map)[0x799E1BD7] = "m_bPlaying";
        (*property_map)[0xC99F9E4F] = "m_bReenableItemCollision";
        (*property_map)[0xC5634730] = "m_AnimPlayListEntity";
        (*property_map)[0x498D248B] = "m_ActorInstance";
        (*property_map)[0xB928F931] = "m_bAnnouncedHunt";
        (*property_map)[0xCBDB86C9] = "m_fOpenFraction";
        (*property_map)[0x4E973F5F] = "m_fTimeOfDay";
        (*property_map)[0xD8BCDB6A] = "m_fDepth";
        (*property_map)[0xE4F74553] = "AddAgentData_GuardDomain_Pool_SaveData";
        (*property_map)[0x3FB985D8] = "m_bAnimationScalingEnabled";
        (*property_map)[0x4787D99A] = "m_bAllowJumpToLedgeLeft";
        (*property_map)[0x3AF3B5FD] = "m_bDisableSavegame";
        (*property_map)[0x4CAC2817] = "m_fPositionReach";
        (*property_map)[0x161CC82B] = "m_fRotationReach";
        (*property_map)[0x2301BE86] = "m_fPullFactor";
        (*property_map)[0x186F4544] = "m_rDoorFrame1";
        (*property_map)[0x8F059C80] = "m_pArrestReaction";
        (*property_map)[0x6A8D5548] = "m_fRotAngle";
        (*property_map)[0x7C3D7FBD] = "m_fHorizontalAmplitude";
        (*property_map)[0xC4EB211D] = "m_rSequenceEntityExplodeIn";
        (*property_map)[0x4B136A78] = "m_rSequenceEntityExplodeOut";
        (*property_map)[0xC846F4A3] = "m_CellGroups";
        (*property_map)[0xCEC4F14] = "m_rAutoClose";
        (*property_map)[0xE57B06CC] = "DYNAMIC_COLLIDABLES_ONLY";
        (*property_map)[0x19EA5453] = "m_rManualClose";
        (*property_map)[0xD5B42F53] = "m_fCloseTime";
        (*property_map)[0x1E2A0937] = "m_OpenMode";
        (*property_map)[0x93C7AD7A] = "m_nMaxOutPinNum";
        (*property_map)[0x7A7F6046] = "m_GlowObjectivesOutlineColor";
        (*property_map)[0x7D9507A4] = "m_bNeverCloseGate";
        (*property_map)[0x9F1AF092] = "m_bIgnoreCovers";
        (*property_map)[0x7A6B0B75] = "m_bDebugDialog";
        (*property_map)[0x40979AB] = "m_bFixedRotation";
        (*property_map)[0xD0A64706] = "m_rGateEntity";
        (*property_map)[0x5BF0A331] = "m_rDoorExploded";
        (*property_map)[0x597972ED] = "m_aObjectives";
        (*property_map)[0xEF390EE7] = "m_fVRUserInputWeight";
        (*property_map)[0x5757C8] = "m_bEnableCulling";
        (*property_map)[0x9C7840B6] = "m_rDoorBroken";
        (*property_map)[0xDA248894] = "m_rActivateBoxVolume";
        (*property_map)[0x1EB6A713] = "m_ClothColliders";
        (*property_map)[0xA8416386] = "_c";
        (*property_map)[0x3F7FACC2] = "ACTOR_DYN_BODY";
        (*property_map)[0x93B36E40] = "m_bWaitingForDistanceStanddown";
        (*property_map)[0x59A9146] = "m_eHealthValue";
        (*property_map)[0x8F55711B] = "m_rDoorOpenFromInside";
        (*property_map)[0xB63832A4] = "m_fImpactAnimationCooldown";
        (*property_map)[0x2973BEDA] = "m_rDoorUnobstructed";
        (*property_map)[0xD1BF963] = "m_bRigidSkinning";
        (*property_map)[0x290A84AD] = "m_dialogLines";
        (*property_map)[0x8E83E794] = "m_fAmbientHitmanDistance";
        (*property_map)[0x37DB8E14] = "eFontFlags";
        (*property_map)[0xC798A0A1] = "m_rDoorSoundEmitter";
        (*property_map)[0x9267356A] = "m_sPageName";
        (*property_map)[0xC18B315] = "m_nEffector";
        (*property_map)[0x12BD91B0] = "m_pCollisionImpactEventDynamic";
        (*property_map)[0x65D13446] = "m_nBlend";
        (*property_map)[0x3A275871] = "m_aStringToSignalForwarders";
        (*property_map)[0xCEACAA74] = "m_bDumpBody";
        (*property_map)[0xBA313E16] = "m_aPreferredActors";
        (*property_map)[0xD06CD681] = "m_bPlayResponse";
        (*property_map)[0x8E6D598F] = "m_rMainItemKey";
        (*property_map)[0x1EF38F67] = "m_bClimbWindow";
        (*property_map)[0x8846DCE0] = "m_ePhysicsMode";
        (*property_map)[0xF174621C] = "m_eDisposalTypeOverwrite";
        (*property_map)[0xA28A3718] = "m_pHintTextResource";
        (*property_map)[0xA8B71A89] = "m_bIsCounterVisibleInTile";
        (*property_map)[0x3A10E436] = "m_bUsePlacementAttach";
        (*property_map)[0x54F9276] = "m_bSetAIPerceptible";
        (*property_map)[0x961A983C] = "m_rSetpieceUsed";
        (*property_map)[0x2C6936A3] = "m_PendingOrderData";
        (*property_map)[0x6FE85693] = "m_bActivateOnce";
        (*property_map)[0xD528F9C8] = "m_animResource";
        (*property_map)[0x559AD2BE] = "m_aSituations";
        (*property_map)[0x90282881] = "m_fAnimationTime";
        (*property_map)[0x2877BDCF] = "m_bIsIllegal";
        (*property_map)[0x56EEE881] = "m_fLFFreq";
        (*property_map)[0xA08808E5] = "m_eCapsuleGridSize";
        (*property_map)[0x775B2E01] = "m_eCoordinateSpace";
        (*property_map)[0xDA4EF708] = "m_bIsSevere";
        (*property_map)[0x60BF68DE] = "m_bIsLargeScale";
        (*property_map)[0x8DB7D569] = "m_Angle";
        (*property_map)[0x6649F60F] = "m_CastAsync";
        (*property_map)[0xEEE0E449] = "m_bHolsterNonWeaponItemsInHand";
        (*property_map)[0xF225F3ED] = "m_DrawOnlyNoCollision";
        (*property_map)[0xDB7D400C] = "m_DrawStickTime";
        (*property_map)[0x573CB32C] = "m_eDialogRangeEnd";
        (*property_map)[0xB75FA1D2] = "m_aOverrides";
        (*property_map)[0xC0BAFC06] = "m_fReleaseRagdollTimeout";
        (*property_map)[0xB599C53D] = "m_sTraceChannel";
        (*property_map)[0xA8ADABEC] = "object";
        (*property_map)[0x5F5674C6] = "m_fCivilianPropagationLOSMaxDistance";
        (*property_map)[0x25DA3F01] = "may_interrupt";
        (*property_map)[0x1C929A81] = "distance";
        (*property_map)[0x5BFA3943] = "m_bOneDirection";
        (*property_map)[0xBF26BFB9] = "m_bStanddownComplete";
        (*property_map)[0xB7B03208] = "m_eRotAxis";
        (*property_map)[0x731BC572] = "m_fMaxRotationDeg";
        (*property_map)[0xAF4CA9ED] = "m_bFleeForHelp";
        (*property_map)[0xE11EE94D] = "items";
        (*property_map)[0xA772E20E] = "m_bDebugAttachedInvisibleItems";
        (*property_map)[0x178F8AC7] = "m_fTransSpringDamping";
        (*property_map)[0x61C00B49] = "m_nDelayOnFalse";
        (*property_map)[0x57DD476C] = "m_bCancelIfInventoryInUse";
        (*property_map)[0x32CFD563] = "m_fSwing1SpringDamping";
        (*property_map)[0x9856D0CF] = "m_fLength";
        (*property_map)[0x76A6EE8F] = "m_rMayPickupCondition";
        (*property_map)[0x79A0F1F9] = "m_bMarkAsDeadEnd";
        (*property_map)[0x7792E83B] = "m_bCastCSMShadows";
        (*property_map)[0x3E8D9897] = "m_GlowLTMemoryColor";
        (*property_map)[0x6EB3D380] = "m_StunDurationVariance";
        (*property_map)[0xF5598A9C] = "subType";
        (*property_map)[0x4BC173D8] = "externalSceneTypeIndicesInResourceHeader";
        (*property_map)[0x60F076A8] = "m_rOcclusionRoom";
        (*property_map)[0x351BE901] = "inputPinForwardings";
        (*property_map)[0x7CF08925] = "outputPinForwardings";
        (*property_map)[0x20649CA5] = "overrideDeletes";
        (*property_map)[0xED4B2B43] = "m_rDefaultCrowdGroup";
        (*property_map)[0x95BFCAC8] = "pinConnectionOverrides";
        (*property_map)[0xFD446556] = "m_visibleOp";
        (*property_map)[0xCA1A755C] = "m_bShowBoneA";
        (*property_map)[0xB6AC0E77] = "pinConnectionOverrideDeletes";
        (*property_map)[0xD82B9709] = "_f";
        (*property_map)[0xFEE8167] = "m_bIsToggledOn";
        (*property_map)[0x375C3E71] = "m_aLongTermMemorySaveData";
        (*property_map)[0x7C450A51] = "m_sSweetStateTextIfOn";
        (*property_map)[0x56EA6482] = "m_nCrowdNearCullDist";
        (*property_map)[0x889EC90B] = "m_sRawStateTextIfOff";
        (*property_map)[0x291BCEF5] = "m_fRestLengthScale";
        (*property_map)[0xD27DED8B] = "m_fHDRBloomFalloff";
        (*property_map)[0xCB53CBB4] = "m_aSequences";
        (*property_map)[0xDFEE5B5F] = "m_fDeceleration";
        (*property_map)[0xE07DC84C] = "m_eMapType";
        (*property_map)[0x665BF167] = "m_bLoopUntilStopped";
        (*property_map)[0xD9EEA5F3] = "fConeWidth";
        (*property_map)[0xE7E488B4] = "m_bDepthPrepass";
        (*property_map)[0x691F5D5B] = "m_bDebugEmotionState";
        (*property_map)[0xFAF19E99] = "m_fGridSize";
        (*property_map)[0x1FAA61A8] = "m_aMultiSource";
        (*property_map)[0xD36DE64A] = "m_Doors";
        (*property_map)[0x9ADF38ED] = "m_aRenderables";
        (*property_map)[0x3F44701B] = "m_bAllowCC";
        (*property_map)[0x13CAAB20] = "m_rCloset";
        (*property_map)[0x9B01AB1B] = "m_bStopReaction";
        (*property_map)[0x5D32660] = "m_rEnterSequence";
        (*property_map)[0xAE9C065C] = "m_rPeekExitSequence";
        (*property_map)[0x12BED923] = "m_fVerticalAmplitude";
        (*property_map)[0x6779E48D] = "m_aHiddenBodyVolumes";
        (*property_map)[0xF8065883] = "m_rPeekSequence";
        (*property_map)[0x11E6A5CA] = "m_bBlockDeath";
        (*property_map)[0x6B954AFA] = "m_eClosetType";
        (*property_map)[0x265E2AD1] = "m_fPitchAngle0";
        (*property_map)[0x640CF611] = "m_libraries";
        (*property_map)[0x6C13FF94] = "m_bIncludeItemHiddenInInventory";
        (*property_map)[0x8854DC7E] = "m_bCheckInsideContainers";
        (*property_map)[0x54B94B6] = "m_nAmmoCountRPG";
        (*property_map)[0x86A9675D] = "m_aPatrolRoutes";
        (*property_map)[0xCC15BBC7] = "m_nPriorityModifier";
        (*property_map)[0x97F70BB] = "m_nGridCellSize";
        (*property_map)[0x7A862053] = "m_bShouldSubscribe";
        (*property_map)[0x6BE3DA87] = "m_eTargetState";
        (*property_map)[0x272E0C24] = "m_nRow";
        (*property_map)[0xB6771A30] = "m_bShouldSubscribeItemEvents";
        (*property_map)[0xB06A25D2] = "m_fRelease";
        (*property_map)[0x4CC8F980] = "m_pPhysics";
        (*property_map)[0xE3A2D9FB] = "m_CameraSpatial";
        (*property_map)[0xDC69604E] = "m_bGuardToHitman";
        (*property_map)[0x405094B7] = "m_eLayer";
        (*property_map)[0x1C4B29E5] = "m_nGameTimePrev";
        (*property_map)[0xB88A16F9] = "m_fDisplayTime";
        (*property_map)[0x4AFE56DF] = "m_bIsEnableCondValid";
        (*property_map)[0x414DA0B1] = "m_NearPlaneSize";
        (*property_map)[0xA3D5F029] = "m_iActorIndex";
        (*property_map)[0xB5931B30] = "m_FarPlaneSize";
        (*property_map)[0x679A115] = "m_TypeItem";
        (*property_map)[0x77958C09] = "m_Depth";
        (*property_map)[0xA1EA712B] = "m_bAIPerceptibleOnRelease";
        (*property_map)[0xAF381060] = "m_FarPlaneOffset";
        (*property_map)[0x3BE858CC] = "m_Slash1HEmitter";
        (*property_map)[0x9D4C6E28] = "m_bLooping";
        (*property_map)[0xF1BD08D4] = "m_bLookAtActiveCamera";
        (*property_map)[0x853156BA] = "m_bUseAlpha";
        (*property_map)[0xA0AB87C6] = "m_pParent";
        (*property_map)[0x8546A15A] = "m_bRunning";
        (*property_map)[0xEADFC230] = "m_bShouldBeStopped";
        (*property_map)[0x5286103C] = "m_StartTime";
        (*property_map)[0x3EB29D97] = "fTime";
        (*property_map)[0xE066D690] = "m_RunTime";
        (*property_map)[0x8E609070] = "m_rDirection";
        (*property_map)[0x455EFCE] = "m_bAutomaticScreenshotMode";
        (*property_map)[0xACC75978] = "m_rItemSpawner";
        (*property_map)[0x361058EC] = "m_sOnlineInstanceId";
        (*property_map)[0x5C9C97E9] = "m_nOcclusionMaxAngleDeg";
        (*property_map)[0x9118A07B] = "m_aItemModifierIds";
        (*property_map)[0x511906E3] = "m_aCrowdInclusionBoxes";
        (*property_map)[0x2CA95119] = "m_aDynamicParametersData";
        (*property_map)[0x9149A266] = "m_rBCLid";
        (*property_map)[0x7788ADD8] = "m_rContainer";
        (*property_map)[0x61418191] = "m_aDoors";
        (*property_map)[0xFE522CCA] = "m_rActorSpecification";
        (*property_map)[0xA8287555] = "m_GateType";
        (*property_map)[0x2A22442E] = "m_fCombatNPCDecisionRadius";
        (*property_map)[0x249023B3] = "m_BCCloseMode";
        (*property_map)[0x6E85F87D] = "m_rExcludeKeywords";
        (*property_map)[0xA4EF0F48] = "m_PoolDecalsSpawner";
        (*property_map)[0xEA2233FB] = "m_eBCAnimSet";
        (*property_map)[0x7E8401BB] = "m_fCollisionHeightFrom";
        (*property_map)[0xC1E92657] = "m_Ps3SplashNormalMapID";
        (*property_map)[0xCE13149E] = "m_RedOfRed";
        (*property_map)[0x92571278] = "m_bAllowUnconsciousTarget";
        (*property_map)[0x4EA60D34] = "m_nSupportCount";
        (*property_map)[0x41EFF568] = "eRole";
        (*property_map)[0xA4BDB2B0] = "m_nEmissiveHdrScale";
        (*property_map)[0x1B3174CB] = "eOrder";
        (*property_map)[0x6A85D4DC] = "m_aPathPoints";
        (*property_map)[0x12474AC4] = "m_rTrackEntity";
        (*property_map)[0x425CA720] = "m_bRunOnGameTime";
        (*property_map)[0x7AD68ED5] = "m_bHasBeenStarted";
        (*property_map)[0xF41FDE0C] = "m_bWasAimAssistActivated";
        (*property_map)[0x7F4309D8] = "m_rDropPoint";
        (*property_map)[0xCAD51F7] = "m_EnvironmentClothColliders";
        (*property_map)[0x29BA6D5B] = "m_nGameTime";
        (*property_map)[0xDDF7AD02] = "m_fSelectDistance";
        (*property_map)[0x6A18AA8D] = "m_ForceAlignmentWithCharacterDirection";
        (*property_map)[0xA87DBBF1] = "m_sScene";
        (*property_map)[0x55844EE6] = "m_sLocationId";
        (*property_map)[0xD39FEEA7] = "m_fPrecision";
        (*property_map)[0xB014234D] = "m_bMultiTrack";
        (*property_map)[0xC0A41EC2] = "m_pJSONResource";
        (*property_map)[0xB7228211] = "m_sContractId";
        (*property_map)[0x23D6FB7A] = "m_fFastSpeedBlendInTime";
        (*property_map)[0x50882CE2] = "m_EnabledEntranceId";
        (*property_map)[0xA1F0EF08] = "m_bStartAtClosestLocation";
        (*property_map)[0xC56A8777] = "m_pEnumType";
        (*property_map)[0x217A0F5B] = "m_bFlickerOn";
        (*property_map)[0x57E477EB] = "m_contractData";
        (*property_map)[0xA88875B5] = "m_trackedOpportunity";
        (*property_map)[0x53EE5477] = "m_SoundAmbienceData";
        (*property_map)[0x7B953446] = "m_ChallengesSaveData";
        (*property_map)[0x90128E0E] = "m_rSpatial2";
        (*property_map)[0xF1AC4D78] = "m_fColumnSpacing";
        (*property_map)[0xB96188F] = "m_nSaveGameLimit";
        (*property_map)[0xA749B53A] = "m_aValueZEntities";
        (*property_map)[0xD37D9450] = "m_fSimulationTimeAfterMoveProperty";
        (*property_map)[0xCB6D4652] = "m_bFireOnInit";
        (*property_map)[0x548D3368] = "m_Acts";
        (*property_map)[0xE681A00F] = "m_bGC_ConditionValue";
        (*property_map)[0xF2BD83DA] = "m_bSneak";
        (*property_map)[0xB61C01E1] = "m_bUseSeparateBlendFactors";
        (*property_map)[0xD4AA1F0A] = "m_fShootTime";
        (*property_map)[0xA982C36D] = "m_bForcedShooting";
        (*property_map)[0xF7A37870] = "m_aFontDefinitions";
        (*property_map)[0xAFECF544] = "act";
        (*property_map)[0xDF1B03A5] = "m_pChildrenContainer";
        (*property_map)[0x632CA0C7] = "m_nDebugTrack";
        (*property_map)[0xD097F79C] = "m_aActorMaterialOverrides";
        (*property_map)[0x4FA5DB57] = "m_fsm";
        (*property_map)[0x4ECBFAED] = "m_nMaxCrowdRenderDist";
        (*property_map)[0x1291C421] = "m_eAnimSetRequirement";
        (*property_map)[0xB4C85228] = "m_eColorUsage";
        (*property_map)[0xBA049DA3] = "m_aLayers";
        (*property_map)[0xF8DA7E26] = "m_GlowTraversalOutlineColor";
        (*property_map)[0x6DF09491] = "FlagPositionZ";
        (*property_map)[0x84FC61D8] = "FlagRotationX";
        (*property_map)[0xF3FB514E] = "FlagRotationY";
        (*property_map)[0xC91EB39C] = "m_fLightFadeLength";
        (*property_map)[0x6AF200F4] = "FlagRotationZ";
        (*property_map)[0x8DC26129] = "FlagViewMatrix";
        (*property_map)[0x4976528D] = "m_position";
        (*property_map)[0x52ADDAF2] = "m_degRotation";
        (*property_map)[0x6E19878] = "m_nMaxTagCount";
        (*property_map)[0xE47A6058] = "m_nHideTime";
        (*property_map)[0x3B253E30] = "m_bSituationEnded";
        (*property_map)[0xB0552437] = "m_bInRequiredDisguise";
        (*property_map)[0x7601CB3] = "m_pctScale";
        (*property_map)[0xA7871641] = "m_bBend";
        (*property_map)[0x8B95496E] = "m_View_degRotation";
        (*property_map)[0x555F7CD6] = "m_rSourceB";
        (*property_map)[0x7280C666] = "m_projOp";
        (*property_map)[0x88036555] = "m_fBodyPartModifierHand";
        (*property_map)[0x729FD1A7] = "m_Perspective_degFovY";
        (*property_map)[0x6E7A44DF] = "m_aVariations";
        (*property_map)[0xC8543BD9] = "aSupportedTypes";
        (*property_map)[0x484AF1B2] = "m_eRotationType";
        (*property_map)[0xF3E2D2EA] = "m_fAutoFadeMinTime";
        (*property_map)[0xAC9D571F] = "bIsContainer";
        (*property_map)[0xCAE51759] = "m_nLastLOD";
        (*property_map)[0x352E9D6D] = "m_rTestCondition";
        (*property_map)[0xD48A2F7C] = "config";
        (*property_map)[0x60A316AA] = "m_nExtraActorCount";
        (*property_map)[0x1C4771B3] = "fSliderPerc";
        (*property_map)[0xCC745B32] = "m_BlueOfBlue";
        (*property_map)[0xCEC7C153] = "m_rAlarm";
        (*property_map)[0x95D1661D] = "m_pAudioEventResource";
        (*property_map)[0xE8554EF2] = "m_bCoverScale";
        (*property_map)[0x5110AEF4] = "m_rSoundEvent_CloseCombat_BreakNeck";
        (*property_map)[0x2C229601] = "m_bRandomEjectForce";
        (*property_map)[0xC54CD224] = "m_fHitRadius";
        (*property_map)[0xAF03526F] = "m_anchorAdvanceDistance";
        (*property_map)[0x35757562] = "m_fFlyByRadius";
        (*property_map)[0x60D04178] = "m_aDestructiblePieces";
        (*property_map)[0x2957ECBB] = "m_bSituationRunning";
        (*property_map)[0x148F1DD7] = "m_nCurrentDrama";
        (*property_map)[0xE4975CAB] = "m_fBlendTime";
        (*property_map)[0x1CB55B04] = "m_fSettingDistanceMax";
        (*property_map)[0xA1EAFD01] = "m_bAssertOnInput";
        (*property_map)[0x2EE6063D] = "m_bForce";
        (*property_map)[0xFBE5929D] = "m_nAmmoCountSMG";
        (*property_map)[0x88255AF4] = "m_fShaderLODScale";
        (*property_map)[0x5502E1E] = "m_bFollow";
        (*property_map)[0x608CD823] = "m_GameTimersData";
        (*property_map)[0x75032158] = "m_rSecondValueInt";
        (*property_map)[0x493AC309] = "m_formationSlotWidth";
        (*property_map)[0xD2FBAEEA] = "m_fInvestigateCautiousPropagationLOSMinDistance";
        (*property_map)[0xD01E32A9] = "m_bStopAtSceneReset";
        (*property_map)[0x41FEF355] = "m_pLevelFlashMovie";
        (*property_map)[0xBACA88DD] = "fromEntity";
        (*property_map)[0x5A36B1A2] = "m_fShadowedRange";
        (*property_map)[0xA49C98C1] = "m_pValueProvider";
        (*property_map)[0x5A01A6F0] = "m_eCheckMode";
        (*property_map)[0xD692F5F2] = "m_rName";
        (*property_map)[0xE0C9FD21] = "m_precisionShotController";
        (*property_map)[0x8E6B86E0] = "m_rTracked";
        (*property_map)[0xD7775161] = "m_nPoolSize";
        (*property_map)[0x4A48746A] = "m_bDrawCones";
        (*property_map)[0x248CF8B4] = "m_fAngVelMin";
        (*property_map)[0xFF33D71E] = "m_AlphaCurveEntity";
        (*property_map)[0x1881C7ED] = "m_fAngVelMax";
        (*property_map)[0x29CE2F34] = "bDeepTrespassingNOTUSEDBUTNECESSARYBECAUSEOFREALLYSTRANGEPROPERTYPARSERBUG";
        (*property_map)[0xC3BA498C] = "m_fCLPTriggerTime";
        (*property_map)[0xEDD6530C] = "m_fVelocityContribution";
        (*property_map)[0x96206DB1] = "m_fLifeTime";
        (*property_map)[0xFB2E692F] = "m_pNPCAlertedReaction";
        (*property_map)[0x8A48BAE3] = "m_bUpdateApproachPositions";
        (*property_map)[0xD8A3F7A5] = "m_rInitialGuard";
        (*property_map)[0x1163ADDE] = "m_rAssistingActor";
        (*property_map)[0x5B704D7F] = "m_bBlockBulletImpacts";
        (*property_map)[0xF4855AF6] = "m_rTargetActor";
        (*property_map)[0xD06AAE81] = "m_bUseWireMaterialLength";
        (*property_map)[0x28E6ED2C] = "m_rSelectionCheck";
        (*property_map)[0xA600E80B] = "m_Title";
        (*property_map)[0x3840D530] = "m_fXAxisAngle";
        (*property_map)[0x139BF98E] = "m_bEnableSpring";
        (*property_map)[0x3CB5050D] = "m_fZAxisAngle";
        (*property_map)[0x9327B083] = "m_bUseDegrees";
        (*property_map)[0xAEFAA314] = "m_bSubscribeToKeywordChanges";
        (*property_map)[0xC7800A] = "m_bAIShootingReactionEnabled";
        (*property_map)[0x5257B38D] = "m_fGreetingCooldownMin";
        (*property_map)[0xFE0EDA97] = "m_rOriginalA";
        (*property_map)[0xA69AEBB4] = "_k";
        (*property_map)[0x7A342F4F] = "m_fHeightPitch";
        (*property_map)[0x8AEEF034] = "m_CombatSituations";
        (*property_map)[0xD275FBF7] = "m_ItemHandsIdle";
        (*property_map)[0x93875A49] = "range";
        (*property_map)[0xA6A072D5] = "maxMoves";
        (*property_map)[0xC5BD1BE] = "m_eBCState";
        (*property_map)[0x93D61455] = "m_bCheckItemOnBack";
        (*property_map)[0x437FAA67] = "m_aWaitingObservers";
        (*property_map)[0xCFE5BC15] = "m_DebugVariableName";
        (*property_map)[0xC7C56C4D] = "m_observerDistanceMax";
        (*property_map)[0x31A8C141] = "m_fMouseSensitivityY";
        (*property_map)[0x9C2A0827] = "m_pStashPoint";
        (*property_map)[0xC84B01A1] = "m_aSentryZones";
        (*property_map)[0xADA4A7D0] = "m_pTransition";
        (*property_map)[0xD6CE1EAB] = "m_bStartBySearchingLastKnownLocation";
        (*property_map)[0x37C500ED] = "m_chars";
        (*property_map)[0x554EED1F] = "m_eArrestReason";
        (*property_map)[0xA3BFA050] = "m_tLastIdleStanddown";
        (*property_map)[0xA44768B1] = "m_rLocation";
        (*property_map)[0x5F8DC896] = "m_tAnnounceHuntCoolDown";
        (*property_map)[0xA2AD3157] = "m_bPickupItem";
        (*property_map)[0xB74438BE] = "m_bUseRotation";
        (*property_map)[0x2C54183F] = "m_bUseStopDistance";
        (*property_map)[0xB6C5F657] = "m_vGlowContractVignetteRadii";
        (*property_map)[0xA3D82C8F] = "m_fRegularGridSize";
        (*property_map)[0x686EFF7D] = "m_BoxCenter";
        (*property_map)[0x7C98E2C8] = "m_sHintTextFallback";
        (*property_map)[0x9034D24F] = "m_eHintIcon";
        (*property_map)[0xE40801DA] = "m_fProfileBlendTime";
        (*property_map)[0xFE8C21B4] = "m_fYawOffset";
        (*property_map)[0xE1A61844] = "maxActors";
        (*property_map)[0x78FD5064] = "m_nZBias";
        (*property_map)[0x71B132D2] = "m_aCameraPositions";
        (*property_map)[0xC4B1C2F9] = "m_nPriorityGroup";
        (*property_map)[0x712AA961] = "m_eCoverPosition";
        (*property_map)[0x9850E594] = "m_rMaterialEffectMatchPrototype";
        (*property_map)[0x117AC74D] = "m_fLineOfSightInfluenceAggressive";
        (*property_map)[0x1B542D87] = "m_pOverrideMaterial";
        (*property_map)[0x816614FE] = "m_rDoorFrame2";
        (*property_map)[0x88AF6E48] = "m_fLeftRPM";
        (*property_map)[0x60746740] = "m_tExpiryHeroOutside";
        (*property_map)[0x44B55DDB] = "m_sMessageKey";
        (*property_map)[0x285A4BF3] = "m_rZone";
        (*property_map)[0x17090DA2] = "m_rMainCamera";
        (*property_map)[0x299599D8] = "m_fMinPitch";
        (*property_map)[0x35BFA1] = "m_aDeadBodies";
        (*property_map)[0x4392721A] = "m_RecordedHitmanOutfit";
        (*property_map)[0xC24F4C22] = "m_aPerceptiblePool";
        (*property_map)[0x92C5F8DD] = "m_eButton";
        (*property_map)[0x59A225CB] = "m_mRainDirection";
        (*property_map)[0x7235BD59] = "m_TextureMapID";
        (*property_map)[0x5321B64E] = "m_nNodeCount";
        (*property_map)[0x5292D17F] = "m_ReflectionMapID";
        (*property_map)[0x5505344A] = "m_fCausticsDistortionStrength";
        (*property_map)[0xEC83C341] = "m_fReflectionIntensity";
        (*property_map)[0xB426B1D5] = "m_sRagdollImpactsSwitch";
        (*property_map)[0x36FA2765] = "m_nNumParticles";
        (*property_map)[0xC10E09D3] = "m_fPuddleHeight";
        (*property_map)[0x1EC213C3] = "m_bCrowdAffectNPCs";
        (*property_map)[0xCC17AE3B] = "m_vLastPosition";
        (*property_map)[0xC17305ED] = "m_fPuddleScale";
        (*property_map)[0x5DEA5F9A] = "m_aDefaultSpreadingActors";
        (*property_map)[0x8A07B205] = "m_fRunningWaterHeight";
        (*property_map)[0xDC3C119] = "m_fSettingInSightAgeMin";
        (*property_map)[0xECCE6049] = "m_fSwingDriveDamping";
        (*property_map)[0xB6194F95] = "m_fRunningWaterDelta";
        (*property_map)[0x2ECD86B5] = "m_pPlayLegacyRumble";
        (*property_map)[0xCC1C94C8] = "m_fRunningWaterScale";
        (*property_map)[0x6F431AD0] = "m_eViewportAnchorB";
        (*property_map)[0xA4A49923] = "m_fRunningWaterDamping";
        (*property_map)[0xE183BD9C] = "m_fImpactMaxPFVerticalMinDistance";
        (*property_map)[0xE5BA64C6] = "m_vFadeDistanceNegative";
        (*property_map)[0x6CE64709] = "m_eLocation";
        (*property_map)[0xABC887A2] = "m_bEnableRainFade";
        (*property_map)[0x52B76246] = "m_fRainFadeDistance";
        (*property_map)[0xCE1AC986] = "m_fRainFadeLength";
        (*property_map)[0x39315007] = "m_nCrowdLODDist2";
        (*property_map)[0xC7DC3B] = "m_TextureBlockSize";
        (*property_map)[0x667C14B9] = "m_fBoxDistance";
        (*property_map)[0x73BCB802] = "m_fBackupCooldown";
        (*property_map)[0x646C77CA] = "m_ContactScaleStart";
        (*property_map)[0xF4B3A7C1] = "m_StreakScale";
        (*property_map)[0xFC1C78F] = "m_ContactScaleEnd";
        (*property_map)[0xC359CF9C] = "m_fVignetteSharpenStrength";
        (*property_map)[0x15832D0A] = "m_fFacingOffsetX";
        (*property_map)[0x5C73DF8F] = "m_fDieTime";
        (*property_map)[0x545B8135] = "entityIndex";
        (*property_map)[0xDC2EAC6A] = "m_nDisguiseCoverValue";
        (*property_map)[0x66FD1DAC] = "m_fRandomRotationStart";
        (*property_map)[0xF6A2B6D5] = "m_fRandomRotationSpeed";
        (*property_map)[0xEB749E68] = "bCannotBeHolstered";
        (*property_map)[0x7D62C210] = "m_fAlphaScaleEnd";
        (*property_map)[0xDFE3E0D1] = "m_fRainRadius";
        (*property_map)[0x8B0984DA] = "m_fReverseActivationAngle";
        (*property_map)[0xB0473750] = "m_HazeColor";
        (*property_map)[0x3952D0CB] = "platform";
        (*property_map)[0xD97BDF53] = "m_bCanHolsterItems";
        (*property_map)[0x5A68A2D2] = "m_fParticleNearClip";
        (*property_map)[0x9DA1B297] = "m_bTopEnabled";
        (*property_map)[0xB702A848] = "m_aEmitterRTPCs";
        (*property_map)[0xC7BF57C8] = "m_fParticleAlphaClipOffset";
        (*property_map)[0x5AD9D64B] = "m_bLadder";
        (*property_map)[0xB8E5CAE1] = "m_fPs3SplashGravityScale";
        (*property_map)[0x61B47B92] = "SubtitleMarkupsPerLanguage";
        (*property_map)[0xCFC837AC] = "m_fPs3SplashAngleStart";
        (*property_map)[0x22184519] = "m_fSSAOAngleOffset";
        (*property_map)[0x185D744D] = "m_fPs3SplashAngleEnd";
        (*property_map)[0xE03589F6] = "m_bCombat";
        (*property_map)[0xE20A9785] = "m_fPs3SplashSizeMin";
        (*property_map)[0xDE07A8DC] = "m_fPs3SplashSizeMax";
        (*property_map)[0x7C9A1E26] = "m_fPs3SplashSizeAspect";
        (*property_map)[0x55757846] = "m_sShellImpactsSwitchGroup";
        (*property_map)[0xBA2C8688] = "m_nPs3SplashMinSpawn";
        (*property_map)[0xEA0522A5] = "m_Ps3SplashMapID";
        (*property_map)[0x71A1DE95] = "m_bAlwaysZoomed";
        (*property_map)[0xCB5CF04F] = "m_bOpenPauseMenu";
        (*property_map)[0x1D66F8A7] = "m_fMaxYawAngle";
        (*property_map)[0xEA13936D] = "m_bCanBeOperatedFromAnyAngle";
        (*property_map)[0xC52F71C2] = "m_fCameraPullbackDistance";
        (*property_map)[0xC87AB839] = "m_fMoveCameraTargetVelocityH";
        (*property_map)[0xE04ABF3C] = "m_bNotIgnoreAnnoyingHitman";
        (*property_map)[0x7B61AF08] = "m_fDoubleVisionSpeed";
        (*property_map)[0x3D04F0F6] = "m_fMoveCameraSeekStrength";
        (*property_map)[0x5BA623D7] = "m_eDismountDownBehavior";
        (*property_map)[0x3B1B6935] = "m_fPeekFOV";
        (*property_map)[0x765C714E] = "m_fRangeMediumBonus";
        (*property_map)[0xFCE4272A] = "m_nAIEventBlockTime";
        (*property_map)[0xD5B59471] = "m_rGroupPath";
        (*property_map)[0x8648862B] = "m_bHolsterDuringAct";
        (*property_map)[0xD2A79EC4] = "m_fPeekZoomVelocity";
        (*property_map)[0xFDFD0EE7] = "m_bIsHDR";
        (*property_map)[0x7B7564B3] = "Override";
        (*property_map)[0xA60E5FCA] = "m_sIconId";
        (*property_map)[0x9475E086] = "m_fInitialNormalizedYaw";
        (*property_map)[0x784CC99A] = "m_bAlertedHigh";
        (*property_map)[0x46AFF1D7] = "m_fMouseSensitivityX";
        (*property_map)[0x9E4DF6F] = "m_aDefinitions";
        (*property_map)[0x5078ED66] = "m_bAlternativeControlScheme";
        (*property_map)[0xFB5F87C9] = "attributeOverrides";
        (*property_map)[0xF25C5BB1] = "m_ForwardOnVariant";
        (*property_map)[0xEEF32C6A] = "m_bDropCarriedItems";
        (*property_map)[0xC226D057] = "m_mOffset";
        (*property_map)[0x76F30B76] = "bShowMissingTool";
        (*property_map)[0x6D802BE0] = "m_fSphereRadius";
        (*property_map)[0xBF9D5234] = "m_bSendValueOnInit";
        (*property_map)[0x1C915DA6] = "m_fInitInput";
        (*property_map)[0x27588EEB] = "m_tAnnouncedHunt";
        (*property_map)[0x67C7318F] = "m_aVRMovementConfigs";
        (*property_map)[0xE3E3ECC4] = "m_fLeftHandRotationAngles";
        (*property_map)[0x285B419E] = "m_fFPSFovYDeg";
        (*property_map)[0xE796E392] = "m_rBlunt2HDecal";
        (*property_map)[0xD37FDF93] = "m_pRootNode";
        (*property_map)[0x1DF42C4F] = "m_bTargetBlamed";
        (*property_map)[0x439CBAC8] = "m_vClearColor";
        (*property_map)[0x678392E5] = "m_GlowEnemiesUnconsciousColor";
        (*property_map)[0x600F34A6] = "m_fRightFadeLength";
        (*property_map)[0x2BBF97DA] = "m_bAllowAutoCameraCuts";
        (*property_map)[0x93B40AC7] = "m_nPIPPriority";
        (*property_map)[0x9E8A35F] = "m_fCameraCutTriggerAngleDeg";
        (*property_map)[0x403363F3] = "m_fHeadBoneAttacherFwdOffsetPSVR";
        (*property_map)[0xB398E0F] = "m_bBlockCurious";
        (*property_map)[0x61913988] = "m_fDepthExportNearZ";
        (*property_map)[0x1C43F57B] = "m_nNumAnchors";
        (*property_map)[0xB5F912E] = "m_sMatch";
        (*property_map)[0xFABE1857] = "m_nAmmoCount";
        (*property_map)[0xE7A46D9D] = "m_aMaterials";
        (*property_map)[0xF826C48E] = "m_ChallengeId";
        (*property_map)[0xDA6F2C67] = "m_TargetRoom";
        (*property_map)[0x83F2B5D1] = "m32";
        (*property_map)[0xE592FC24] = "m_Object";
        (*property_map)[0x8F1D8C11] = "m_rLookAtEntity";
        (*property_map)[0x9932E965] = "m_aUIHighlightGeom";
        (*property_map)[0xA4980B2C] = "m_eEmotionRemoveLimit";
        (*property_map)[0x569D8717] = "m_aIgnoreCollisionGeom";
        (*property_map)[0x23FDFC7D] = "m_Minute";
        (*property_map)[0xDA2D1F7F] = "m_aContent";
        (*property_map)[0x4DD33009] = "m_sActionName";
        (*property_map)[0x7E2B1E18] = "m_enabledEntranceId";
        (*property_map)[0x7E75DF4D] = "m_bAlwaysDirty";
        (*property_map)[0xEE27419F] = "m_fLongScale";
        (*property_map)[0xEAD2C891] = "locked";
        (*property_map)[0x14F05297] = "m_bIgnoreCollision";
        (*property_map)[0xF4E04AEF] = "m_fOnABreakEvaluationFrequency";
        (*property_map)[0x69A88D28] = "m_fAspect";
        (*property_map)[0xDC7F4239] = "m_bRequiresHitmanFacing";
        (*property_map)[0x5274D351] = "m_nAudioHeadType";
        (*property_map)[0x2DD63A79] = "m_bWorldAlignPrompt";
        (*property_map)[0x2C8DA8FC] = "m_targetDistanceField";
        (*property_map)[0xF7FE3846] = "m_eBlendMode";
        (*property_map)[0xE05B644A] = "m_sExitTextID";
        (*property_map)[0xC1C6EE8B] = "m_aPromptPositions";
        (*property_map)[0x94ACF543] = "m_fStaticFriction";
        (*property_map)[0x7880B828] = "m_rIllegalActionEntity";
        (*property_map)[0x48FB1207] = "m_aNearHeroEnableConditionListeners";
        (*property_map)[0x40BA4931] = "m_pBlockDisguiseAttentionCondition";
        (*property_map)[0xF288B9E9] = "m_nMaxRaycasts";
        (*property_map)[0xEA5716C] = "m_GreenOfRed";
        (*property_map)[0x5EF19B72] = "aPerks";
        (*property_map)[0xAAEBF009] = "m_fInvestigateCautiousCivilianPropagationProximityMinDistance";
        (*property_map)[0x809D6710] = "m_bDisabledStage";
        (*property_map)[0x1D775834] = "value";
        (*property_map)[0x28CD86E9] = "m11";
        (*property_map)[0xDB6A47E4] = "m_bNotIgnoreLowNoise";
        (*property_map)[0x7ACB8788] = "m_bNotIgnoreSillyHitman";
        (*property_map)[0x6202C8C3] = "m_bNotIgnoreDistractions";
        (*property_map)[0xB1840C1C] = "m_vFaceTarget";
        (*property_map)[0xAAB3D24C] = "m_bNotIgnoreWeapons";
        (*property_map)[0x6BB531D5] = "m_vGlowTaggedFadeNearFar";
        (*property_map)[0xC6642CC8] = "m_faceTarget";
        (*property_map)[0x4407CE37] = "m_fSpatialBlurStart";
        (*property_map)[0x1DFCA98] = "m_bMainEventEndedOnStop";
        (*property_map)[0xDF2415E8] = "m_pSafeZoneOrigin";
        (*property_map)[0x6A3E4022] = "m_bNotIgnoreLockdown";
        (*property_map)[0xCED1D473] = "m_bNotWantsPrivacy";
        (*property_map)[0x634211A1] = "m_bLerpGaussian";
        (*property_map)[0xCD7FAA97] = "m_fHFFreq";
        (*property_map)[0xAA666F5C] = "m_pEnableBlindfire";
        (*property_map)[0xBF62B85E] = "m_bLerpSpatial";
        (*property_map)[0x702137AF] = "m_bLerpDepthOfField";
        (*property_map)[0x12DF9066] = "m_fRadiusLeft";
        (*property_map)[0xB740B3BB] = "m_aPromptDescriptionText";
        (*property_map)[0xFE57BC86] = "m_bLerpDoubleVision";
        (*property_map)[0x626EEB30] = "m_bLerpFastFog";
        (*property_map)[0x8EAD2EE1] = "m_bLerpFog";
        (*property_map)[0x1EEC16E6] = "m_fBodyPartModifierLeg";
        (*property_map)[0x76F554E5] = "m_bLerpSharpening";
        (*property_map)[0x465B4675] = "m_aKeywordsData";
        (*property_map)[0xB100F2A6] = "m_bLerpSSR";
        (*property_map)[0xA0089792] = "m_aChildrenProbes";
        (*property_map)[0xF739992C] = "m_bLerpVignette";
        (*property_map)[0xB996C006] = "m_bRoomLerpEnabled";
        (*property_map)[0x77C2F8DF] = "m_fLuminance";
        (*property_map)[0x5F80D5A0] = "m_fBlendThreshold";
        (*property_map)[0x57AB0E0E] = "m_tLastSeriousSituation";
        (*property_map)[0x2920F05E] = "m_tLastThreat";
        (*property_map)[0xD198F8F6] = "m_curveBlue";
        (*property_map)[0x33720D31] = "m_tresspassGroup";
        (*property_map)[0x1BE75ED6] = "m_HeightThresholdSneaking";
        (*property_map)[0x82E08B91] = "m_evacuateGroups";
        (*property_map)[0x97127126] = "m_activeSafeRooms";
        (*property_map)[0x7E080BCD] = "m_aGenerators";
        (*property_map)[0xE7231907] = "m_vFacingDirection";
        (*property_map)[0x3DA47B8] = "m_eAmmoType";
        (*property_map)[0x22FB0750] = "m_fDoubleVisionStrength";
        (*property_map)[0x7CFD2085] = "m_States";
        (*property_map)[0x69261271] = "m_nSeed";
        (*property_map)[0x693B2578] = "m_bShuffleOnEachLoop";
        (*property_map)[0x12331DCC] = "m_fInterval";
        (*property_map)[0x56B70203] = "m_bSignalOnClient";
        (*property_map)[0xD768B67B] = "m_bInternalEnabled";
        (*property_map)[0xBA437832] = "m_bReplicateTransform";
        (*property_map)[0x773FD5F6] = "m_bSignalOnHost";
        (*property_map)[0x3F76D6BE] = "m_sSweetNavigationLabel";
        (*property_map)[0xA9F4CE0D] = "m_bPassthroughOnHost";
        (*property_map)[0x102604F0] = "m_eAnimSetFrontSide";
        (*property_map)[0x893C4B14] = "m_bAllowActorGroupC";
        (*property_map)[0xA6FE4399] = "m_eWrapMode";
        (*property_map)[0x752B658B] = "m_fEquippedWeaponTime";
        (*property_map)[0x1758DEB7] = "m_bAllowActorGroupD";
        (*property_map)[0xBF585F60] = "m_bMandatory";
        (*property_map)[0x6487C467] = "m_nParamId";
        (*property_map)[0xFF03FC9D] = "m_fMoveOrderSphereSize";
        (*property_map)[0x348603D0] = "m_bCover";
        (*property_map)[0xD2D8B446] = "m_bTakeClothes";
        (*property_map)[0x65AB8DC] = "m_bOperateCPDoor";
        (*property_map)[0x4E5D7140] = "m_fOcclusionFactorOpen";
        (*property_map)[0x6CE8FFCD] = "m_bFinalizePendingSpawnGrenade";
        (*property_map)[0xE7983A4F] = "m_bHideInCloset";
        (*property_map)[0x1105B7F9] = "m_bGrabVictim";
        (*property_map)[0x3D8E89D0] = "m_aWaterVolumes";
        (*property_map)[0xEBE0AC4F] = "m_bPushVictimThroughWindowAndRail";
        (*property_map)[0xA03801BD] = "m_nCrowdLODDist1";
        (*property_map)[0x9D16C33B] = "m_bKickVictimOverLedge";
        (*property_map)[0x82701B5D] = "m_bDropItem";
        (*property_map)[0x8806E83C] = "m_bThrowItem";
        (*property_map)[0xCDBB2560] = "m_bUseDeathAnimation";
        (*property_map)[0xB9BEE187] = "m_bRun";
        (*property_map)[0x4050A00E] = "m_bNoSnapSneak";
        (*property_map)[0x96A78423] = "m_NumShatter1HEmitters";
        (*property_map)[0x857DA118] = "m_bFastWalk";
        (*property_map)[0xB6407C75] = "m_bMovementAllowed";
        (*property_map)[0x4D8B35A8] = "eState";
        (*property_map)[0xD7F2420A] = "m_bCheckArrest";
        (*property_map)[0xDCC4B85B] = "m_bIdleAnimationsAllowed";
        (*property_map)[0x5507AAA6] = "m_bCoverToCover";
        (*property_map)[0x788EFCCA] = "m_bMayEscort";
        (*property_map)[0xDA91AD44] = "m_bCloseCombatChoke";
        (*property_map)[0x4C00E6BE] = "npcName";
        (*property_map)[0x27854074] = "m_formationSlotHeight";
        (*property_map)[0x92B7A2A7] = "m_bCloseCombatPistolFinish";
        (*property_map)[0xC22E8F5D] = "m_nAmmo";
        (*property_map)[0x9C23CC74] = "m_bCloseCombatStandard";
        (*property_map)[0xC674D2AD] = "m_bCloseCombatFakeSwing";
        (*property_map)[0x1E7079E0] = "m_rSpeakingActor";
        (*property_map)[0x7CB34BA9] = "m_bRegisterAsCondition";
        (*property_map)[0x971AD913] = "m_bGameCameraAutoAlign";
        (*property_map)[0xBB2D1AC8] = "m_bInstinct";
        (*property_map)[0x7EB2528C] = "m_bPending";
        (*property_map)[0xD8A0F049] = "m_bAim";
        (*property_map)[0x4AE492F] = "m_bHairTrigger";
        (*property_map)[0xD1725AC3] = "m_bLimitedAmmo";
        (*property_map)[0xF6B254B8] = "m_parametersEntity";
        (*property_map)[0x70693B38] = "m_pHandles";
        (*property_map)[0xED8188B0] = "groupId";
        (*property_map)[0xF8AF6955] = "intelHeadline";
        (*property_map)[0xDE44A810] = "repoID";
        (*property_map)[0x403FE653] = "m_fRotationOffsetPitch";
        (*property_map)[0xC2DC464E] = "intelType";
        (*property_map)[0xA812A04A] = "m_Remap";
        (*property_map)[0xF95E4974] = "m_fEventWindowDelay";
        (*property_map)[0xB2363B49] = "m_nTargetDeadTime";
        (*property_map)[0x3625F625] = "_d";
        (*property_map)[0xF6734E9B] = "m_rawSecondary";
        (*property_map)[0x4122C6B3] = "_e";
        (*property_map)[0x57AB784A] = "m_vEnvCollisionsBoxSizeFinger";
        (*property_map)[0xEE2F400C] = "m_fSlowDownNearFactor";
        (*property_map)[0xD19DDB22] = "_j";
        (*property_map)[0x57E2D7F7] = "m_bCanHang";
        (*property_map)[0x61DD20BF] = "m_fGlowInteractionDeselectedOutlineWidth";
        (*property_map)[0xD002EB8E] = "m_WorldBounds";
        (*property_map)[0x6361AB50] = "m_CanCiviliansInvestigate";
        (*property_map)[0x12520175] = "m_bNoiseEnabled";
        (*property_map)[0xF5A81F64] = "m_json";
        (*property_map)[0x7D37A07E] = "m_bTriggerOnValue";
        (*property_map)[0x4255FB82] = "m_vipTargetNode";
        (*property_map)[0xC5E57A34] = "m_sRequestID";
        (*property_map)[0xAFBA16F8] = "aStringArray";
        (*property_map)[0x36701C0F] = "m_bIsFadeDown";
        (*property_map)[0x2E30E58D] = "fTimeBetweenBullets";
        (*property_map)[0xAEA46125] = "m_bGuardBodyBystander";
        (*property_map)[0x15DE07EC] = "m_bFadeAmbience";
        (*property_map)[0x15E5EAFF] = "m_bFadeNonAmbience";
        (*property_map)[0x3C51E5F1] = "m_dtLateralShiftDelay";
        (*property_map)[0x750D872] = "m_bMuteBGMusicXBox";
        (*property_map)[0x68A0618A] = "m_fAdditionalAlignRadius";
        (*property_map)[0xA5B2F577] = "m_bReverseItemThrowRotationDirection";
        (*property_map)[0x59AF9469] = "m_magazineBulletsByAmmoType";
        (*property_map)[0xEAC6AB70] = "m_nMostRecentItemIndex";
        (*property_map)[0xF7F303F2] = "m_eCollideType";
        (*property_map)[0x39AEB1DE] = "m_eDeathContext";
        (*property_map)[0x3E12A070] = "m_fOcclusionMaxDistance";
        (*property_map)[0x1C8E3975] = "m_aCheckActorList";
        (*property_map)[0x4FC1D89A] = "m_fRightHandWeight";
        (*property_map)[0xF79228F9] = "m_fPrecisionRecoveryDelay";
        (*property_map)[0x8A42BB57] = "m_pCollisionSlidingEvent";
        (*property_map)[0xA49FBCF5] = "m_uicData";
        (*property_map)[0xEF9D9DB1] = "m_Drama2SituationData";
        (*property_map)[0x59FEBD04] = "m_bTargetsHidden";
        (*property_map)[0xB38CACC7] = "m_rLedgeGuide";
        (*property_map)[0xD0B2DD7] = "m_ePrevAgilityState";
        (*property_map)[0xBB5DF4BE] = "m_nActiveAnimNode";
        (*property_map)[0xC33AFA04] = "m_eCameraOffset";
        (*property_map)[0x6ADE828A] = "m_spatialArm";
        (*property_map)[0x33655E96] = "m_bCollisionGroup1";
        (*property_map)[0xFD78930C] = "m_spatialHUD";
        (*property_map)[0x4DE430A9] = "LocalOffset";
        (*property_map)[0x721FBDE4] = "m_alignment";
        (*property_map)[0x1FEB61DF] = "m_bIsMainDestination";
        (*property_map)[0xBB721E5F] = "m_fMarginRight";
        (*property_map)[0x6B163A1A] = "m_normal";
        (*property_map)[0x151A5DA6] = "m_bValidDisturbanceField";
        (*property_map)[0xE7BAE0BF] = "m_bAlwaysKnowHitmanPosition";
        (*property_map)[0xE1FDE248] = "m_rCombatZone";
        (*property_map)[0x9F2CB454] = "m_rSharedKnowledge";
        (*property_map)[0x1BA2CC94] = "m_bTriggered";
        (*property_map)[0xD583F239] = "m_sClassTypeName";
        (*property_map)[0xECAB2FE6] = "m_nTarget";
        (*property_map)[0x41F7A758] = "m_bHasPosition";
        (*property_map)[0x820EBE6A] = "m_eTest";
        (*property_map)[0x56D4C8EB] = "m_bSignalChangeToDefault";
        (*property_map)[0x239BFD44] = "m_fCooldownForThis";
        (*property_map)[0x5D79D762] = "m_FooThing";
        (*property_map)[0x499CAC59] = "m_nSomeInts";
        (*property_map)[0x26B2F3E7] = "m_vScreenPos";
        (*property_map)[0x76BA0C0F] = "exposedEntity";
        (*property_map)[0xF37B75E1] = "m_aPotentialOperators";
        (*property_map)[0xFC91CC90] = "m_nDoorState";
        (*property_map)[0xF800A843] = "m_bHideNonLeftHandItems";
        (*property_map)[0x53A8BBBB] = "m_fAnimPercent";
        (*property_map)[0x51F72B27] = "m_bIncludeInBaseMap";
        (*property_map)[0xBACD4B18] = "m_sLoadoutContractId";
        (*property_map)[0xEF2F14E4] = "m_fTargetAnimPercent";
        (*property_map)[0x999B9A07] = "m_tOpenTime";
        (*property_map)[0xCABF0F4E] = "m_nActorTemplateIndex";
        (*property_map)[0x2BF0F2A7] = "m_bAllowRandomStateChanges";
        (*property_map)[0xFF4DAC8F] = "m_fRecoilHorizontalRange";
        (*property_map)[0xD8E9F8CE] = "m_bCheckEquippedItem";
        (*property_map)[0xE9B08F65] = "m_fTimeWaiting";
        (*property_map)[0xA020EA3F] = "m_rLead";
        (*property_map)[0x185159C0] = "m_aItemIds";
        (*property_map)[0x510FF6C] = "m_eEventType";
        (*property_map)[0x7004B901] = "m_fEventTimeMin";
        (*property_map)[0x25AAF872] = "m_ShaderColor4";
        (*property_map)[0x4C098658] = "m_fEventTimeMax";
        (*property_map)[0x86D89484] = "m_vConeAxis";
        (*property_map)[0x33750727] = "m_fExitEventTimeMin";
        (*property_map)[0x24BD0D24] = "m_bNeedsToPlayAlone";
        (*property_map)[0x214C5700] = "m_fCooldownForThisRand";
        (*property_map)[0x1CF05DDB] = "m_fCooldownForAny";
        (*property_map)[0x7F310F68] = "m_fPropagationProximityMinDistance";
        (*property_map)[0xAFD8CEA2] = "m_eInputMoveRequirement";
        (*property_map)[0x7DA8B953] = "m_eRuningRequirements";
        (*property_map)[0x701FC8F7] = "m_fVecticalSmoothTime";
        (*property_map)[0x186889BF] = "m_bCanUseRightHand";
        (*property_map)[0x679EAAFD] = "m_nCurrentHitman";
        (*property_map)[0x43330C16] = "m_bNeedsRightHand";
        (*property_map)[0x48A3C3A9] = "m_bDisableLeftHandIK";
        (*property_map)[0x23B75399] = "m_nGuards";
        (*property_map)[0x46A4DE86] = "m_bDisableRighttHandIK";
        (*property_map)[0x839643B0] = "m_fKillThreshold";
        (*property_map)[0xDD4C13DF] = "m_fStrength";
        (*property_map)[0xEFE8BC20] = "m_rPicker";
        (*property_map)[0x6481D044] = "m_eItemRequirementLeft";
        (*property_map)[0x6797E304] = "m_ActorDamageControlData";
        (*property_map)[0x860CFFC0] = "m_eItemRequirementRight";
        (*property_map)[0x6DF1CF14] = "m_WindowFrame";
        (*property_map)[0xE4BDC3BA] = "m_aCurrentBehaviorEntities";
        (*property_map)[0x8DC8AF1C] = "m_eItemTypeRight";
        (*property_map)[0xE3E4981C] = "m_aTagsRequired";
        (*property_map)[0xC9EECF69] = "m_bPrecaching";
        (*property_map)[0x48F89031] = "m_aTagsBlockingMe";
        (*property_map)[0x62480C9B] = "m_bAccessibleFromRight";
        (*property_map)[0x8BAB31C5] = "m_aTargets";
        (*property_map)[0xEBC4AEA7] = "m_fArmDamageMultiplier";
        (*property_map)[0x120169B5] = "m_fLArmDamageScalar";
        (*property_map)[0xA76124A1] = "m_sConfigVariableID";
        (*property_map)[0xD4A4A180] = "m_fRArmDamageScalar";
        (*property_map)[0xB2A446A3] = "m_fHandDamageMultiplier";
        (*property_map)[0x1FB442DE] = "m_fLHandDamageScalar";
        (*property_map)[0x635BDA12] = "m_LOSSMGFirePatterns";
        (*property_map)[0x6C35B796] = "m_bSendInterruptWhilePreparing";
        (*property_map)[0x49C12335] = "m_fRHandDamageScalar";
        (*property_map)[0xA1E9DFEB] = "m_nMaxVIPCount";
        (*property_map)[0xFB25522A] = "m_fTorsoDamageMultiplier";
        (*property_map)[0x58C03B97] = "m_bApplyLeftRightScalars";
        (*property_map)[0x7BE2A532] = "m_bApplyDebugEffectors";
        (*property_map)[0x7D0324D4] = "m_AIAreaWorld";
        (*property_map)[0x5A9D59D] = "m_fMarkerHeight";
        (*property_map)[0xFA1ACA92] = "m_nValuesPrSec";
        (*property_map)[0x6480E68] = "m_bIgnoreAnnoyingHitman";
        (*property_map)[0xE4A8541D] = "m_bIgnoreDistractions";
        (*property_map)[0xC99D0AB3] = "m_fNextEventTime";
        (*property_map)[0xF1C1D8D] = "m_eMotionTransX";
        (*property_map)[0x3E1CCB78] = "m_fScaleBeginDistance";
        (*property_map)[0x89B1F330] = "m_bIgnoreAccidents";
        (*property_map)[0x7E81118E] = "m_bWantsPrivacy";
        (*property_map)[0xA51852FE] = "m_pRole";
        (*property_map)[0x7108ECE0] = "m_id";
        (*property_map)[0xE916B8EA] = "nNeighbor0";
        (*property_map)[0x9E11887C] = "nNeighbor1";
        (*property_map)[0x90FD841E] = "m_bAllEscortsAreDead";
        (*property_map)[0x997C4C65] = "nNeighbor5";
        (*property_map)[0xEB35010] = "m_rKnight";
        (*property_map)[0xDC5101A3] = "m_pGrid";
        (*property_map)[0x751DDF] = "nNeighbor6";
        (*property_map)[0x74A2342B] = "m_GridMap";
        (*property_map)[0x25502105] = "nVisionDataOffset";
        (*property_map)[0x469EFC11] = "m_aKnownBy";
        (*property_map)[0x8E492887] = "m_vRotation";
        (*property_map)[0x9EB5042B] = "m_fShadowBlurWidth";
        (*property_map)[0x15B077D] = "m_bSmartStairSurface";
        (*property_map)[0x2B60AE17] = "m_rSpawner";
        (*property_map)[0x753861F2] = "m_rOwner";
        (*property_map)[0x50F7DA77] = "m_rHoldingContainer";
        (*property_map)[0xD94B7B23] = "deathContextType";
        (*property_map)[0xECA698AA] = "m_bEnabledImpulseListener";
        (*property_map)[0xF58FCCB0] = "m_pPreviousOwner";
        (*property_map)[0x11E1F6A8] = "m_FixedColor";
        (*property_map)[0xE0A0821A] = "m_pMuteMusicEvent";
        (*property_map)[0x2FDE4629] = "m_nQuantity";
        (*property_map)[0x863FA195] = "m_fMinAttentionToForceLookAtIfIncreasing";
        (*property_map)[0x7E9A38C3] = "m_eGlowType";
        (*property_map)[0xAC8E0E53] = "m_bDisplayFootsteps";
        (*property_map)[0x28E15E86] = "m_bDeathReactionEnabled";
        (*property_map)[0x680DD0F4] = "fAngle";
        (*property_map)[0xB0CC13B9] = "m_fNormalStrength";
        (*property_map)[0x37396984] = "m_sTypeId";
        (*property_map)[0xB4EDD052] = "m_fDampTime";
        (*property_map)[0x2817E17F] = "m_vGlowBackgroundFadeNearFar";
        (*property_map)[0x1F3BDDC2] = "m_NumFacePunchEmitters";
        (*property_map)[0xF6E0916E] = "m_fImpactRadius";
        (*property_map)[0x58CC2C93] = "m_aReactVariationIndecies";
        (*property_map)[0x4756C5D7] = "m_ColorA";
        (*property_map)[0xDE5F946D] = "m_ColorB";
        (*property_map)[0xBEC3E81] = "m_aEntityData";
        (*property_map)[0xDC6F55D3] = "m_bIsActive";
        (*property_map)[0x60E045AE] = "m_boneID";
        (*property_map)[0xD596DEDE] = "m_fCooldown";
        (*property_map)[0xC09CE233] = "m_Stage2Duration";
        (*property_map)[0x7FE5AEB2] = "m_aeFromStates";
        (*property_map)[0xB669BB3E] = "m_Stage3SpreadEvaluator";
        (*property_map)[0x1BFDFF2A] = "m_GlowCivilianColor";
        (*property_map)[0xA9CB50AF] = "m_pDoors";
        (*property_map)[0x26F0342D] = "m_eMonitoredSharedEvent";
        (*property_map)[0xDBEB9E23] = "m_nWarningPriority";
        (*property_map)[0xF4291BAD] = "m_TargetHasWeaponEquipped";
        (*property_map)[0xCE276D1B] = "m_bTargetLost";
        (*property_map)[0x801C0932] = "m_nFootstepsMovementType";
        (*property_map)[0xAA8D89E2] = "m_eInputValueCheck";
        (*property_map)[0xB5002643] = "m_rIncludeKeywords";
        (*property_map)[0x841EB83C] = "m_FadeTime";
        (*property_map)[0xF06B7364] = "m_Tag";
        (*property_map)[0x7EBE70F6] = "m_bHandlerHasLOS";
        (*property_map)[0x917AB932] = "m_fDefaultRadius";
        (*property_map)[0xF593F7D9] = "m_vLastKnownPosition";
        (*property_map)[0x36334BBB] = "m_tLastTimeTargetLOS";
        (*property_map)[0xA0506B1C] = "m_tNextCivBark";
        (*property_map)[0xEC5930AF] = "m_eHDRGlareType";
        (*property_map)[0x8693D362] = "m_fCivilianPropagationLOSSpreadPerSecond";
        (*property_map)[0xE762D1DA] = "m_tStartSearchCountdown";
        (*property_map)[0x3FD71321] = "m_eDialogState";
        (*property_map)[0xA4747209] = "m_vRunningDropMinMax";
        (*property_map)[0xAD91F96E] = "m_Agent_FreeSlots";
        (*property_map)[0xBA8DEC90] = "m_nSkinSelection";
        (*property_map)[0x3A9A4EF9] = "m_bTriggeredBySecurityCameras";
        (*property_map)[0xDE63048] = "m_DebugValues";
        (*property_map)[0xDF156ACF] = "m_rBodyPart";
        (*property_map)[0xB550FF57] = "m_aProfessions";
        (*property_map)[0x9C1FD05A] = "m_bWorldRotation";
        (*property_map)[0x27A521CA] = "m_pCharacter";
        (*property_map)[0x8127739B] = "m_footsteps";
        (*property_map)[0x69228AAE] = "m_nCurrentFrame";
        (*property_map)[0xD7649870] = "m_nCountdownFrom";
        (*property_map)[0xC63E9EBE] = "m_GlowSetPieceOutlineColor";
        (*property_map)[0x9565D79F] = "m_vBoxSize";
        (*property_map)[0x18F57A67] = "m_ManHuntServiceData";
        (*property_map)[0x6A9328F5] = "m_bPreferredActorIgnoreDanger";
        (*property_map)[0xED08ED31] = "aMenuEntries";
        (*property_map)[0xA91382FF] = "m_aConditionalTextLines";
        (*property_map)[0x7F3D5504] = "m_TextLineNoPrimaryUser";
        (*property_map)[0x719E634B] = "m_nDoneTime";
        (*property_map)[0x29A626ED] = "m_bAllowInterrupt";
        (*property_map)[0xD04FA1AD] = "fDynamicScaleNearDistance";
        (*property_map)[0xA1AF04D] = "m_eSoundDef";
        (*property_map)[0xF3F82B0C] = "m_aFontLibraries";
        (*property_map)[0xA85F3651] = "m_rSequence";
        (*property_map)[0xC17241CA] = "m_rBehavior";
        (*property_map)[0x91C6E607] = "m_bBreakBefore";
        (*property_map)[0x9A8AC428] = "m_tActorSpeakEnd";
        (*property_map)[0xA57B9A5] = "m_pHUDRootList";
        (*property_map)[0x91897A56] = "m_pGameCameraHudRootlist";
        (*property_map)[0x310359FD] = "m_fMarginTop";
        (*property_map)[0x6FB0FCB3] = "m_fSettingCooldownEvalVariance";
        (*property_map)[0x30A12903] = "m_bFixedOrientation";
        (*property_map)[0x445379A1] = "m_pViewFactory";
        (*property_map)[0x2877E2F8] = "m_mChallengeStates";
        (*property_map)[0x49B6CB3E] = "fDistance";
        (*property_map)[0x75FBCDC9] = "fValue";
        (*property_map)[0xE612B620] = "m_bNeverInvestigateDistractions";
        (*property_map)[0xCD395B3D] = "bIsPlaceholder";
        (*property_map)[0xC341CE98] = "m_pPreliminaryLeader";
        (*property_map)[0xBCA3A9C8] = "m_ShaderColor7";
        (*property_map)[0xA878DA2] = "m_pRootControlEntity";
        (*property_map)[0x21836C8B] = "m_aEnterAct";
        (*property_map)[0x12CD9F7] = "m_fMinSpeed";
        (*property_map)[0xE5215049] = "m_pHMShootingReaction";
        (*property_map)[0x73FDEDA0] = "m_fHeadAnchorOffsetUp";
        (*property_map)[0xFD02C50D] = "m_pAttachMovie";
        (*property_map)[0xAD6CE38E] = "m_VignetteColor";
        (*property_map)[0x532108F4] = "m_vHighBoundPos";
        (*property_map)[0xFEFA5BFC] = "m_fShootFromCoverTimerMax";
        (*property_map)[0x400AAB52] = "m_bIncluder";
        (*property_map)[0xADF3F363] = "data";
        (*property_map)[0x9902A890] = "m_eModeToTest";
        (*property_map)[0xF480157C] = "m_idScreenDecalsPairBTex";
        (*property_map)[0x642D44F6] = "m_basePriority";
        (*property_map)[0xECB054E3] = "m_ridColorCurveEntity";
        (*property_map)[0x1B911827] = "pReactionEntity";
        (*property_map)[0xF7F8DBA9] = "m_InitialAmbience";
        (*property_map)[0x2E053872] = "m_fRecoilEaseTime";
        (*property_map)[0xAF6C535D] = "m_fTimeout";
        (*property_map)[0x5BE59611] = "m_rGroupAnchor";
        (*property_map)[0x18DC0157] = "aPrompts";
        (*property_map)[0x7C09B069] = "bIllegal";
        (*property_map)[0xB8B822D6] = "m_fThrottleFactor";
        (*property_map)[0xD7ECC10A] = "m_fThrottleDownDist";
        (*property_map)[0x4675B31D] = "m_fThrottleUpDist";
        (*property_map)[0xD39F584C] = "m_fThrottleUpRange";
        (*property_map)[0x2C3424CC] = "m_rDialogSwitchGeoLocation";
        (*property_map)[0x49079CE4] = "m_fMaxVelocity";
        (*property_map)[0x295A3EED] = "m_bHuntTargetKnownKiller";
        (*property_map)[0xCFD26968] = "m_targetLocation";
        (*property_map)[0x468C762C] = "npc";
        (*property_map)[0xAE992C63] = "m_bSharedHuntTarget";
        (*property_map)[0xFE70D778] = "m_eFirstWarning";
        (*property_map)[0xA49348E8] = "m_bRuleActive";
        (*property_map)[0x90403FAB] = "m_pHitmanFootstepEvent";
        (*property_map)[0xB6991FA0] = "m_bLegalizeCloseCombat";
        (*property_map)[0x61400556] = "m_bColliding";
        (*property_map)[0x58E59716] = "m_Vegetation";
        (*property_map)[0xE73843F6] = "m_bEnableProjection";
        (*property_map)[0xD713FAAA] = "m_bSaveState";
        (*property_map)[0x217E868E] = "m_nConditionOffset";
        (*property_map)[0x6F6C4183] = "m_bPreloadAssets";
        (*property_map)[0xD75526E2] = "m_fASSAORadius";
        (*property_map)[0x67A0E724] = "m_bLoadSaveGame";
        (*property_map)[0x4CC8D75D] = "m_bHide";
        (*property_map)[0xA008FE7C] = "m_bDoneReporting";
        (*property_map)[0xA78833B6] = "m_bActivated";
        (*property_map)[0x5B7E9430] = "m_aProxySpatials";
        (*property_map)[0xFB101367] = "m_bDeactivating";
        (*property_map)[0x53647EB] = "m_DangerousAreaServiceData";
        (*property_map)[0x3F8EFB46] = "m_aSpaceProviders";
        (*property_map)[0xAC733669] = "m_aExclusiveSpaceProviders";
        (*property_map)[0xF74A3968] = "m_bItem";
        (*property_map)[0xCA9A39DE] = "m_fTriggerDelay";
        (*property_map)[0x8D3BB2F3] = "m_bAny";
        (*property_map)[0x24BC28ED] = "m_fLODScale";
        (*property_map)[0x4EF5E763] = "m_fLinearVelocity";
        (*property_map)[0xA2D2B63B] = "m_fNPCMaxDistance";
        (*property_map)[0x7D993827] = "m_bStandAndShoot";
        (*property_map)[0xCCFC213A] = "m_nExplosionLiftSharpness";
        (*property_map)[0x6DACE921] = "m_fLODOffset";
        (*property_map)[0x9B025367] = "m_GlowPlayerSeenInLVAColor";
        (*property_map)[0xA80B229] = "m_eTessellationMode";
        (*property_map)[0x8CEA1A6B] = "m_eTessellationFactorMode";
        (*property_map)[0x5F89E515] = "m_nHeight";
        (*property_map)[0x6681AF08] = "m_vsyncPolicy";
        (*property_map)[0x9F858608] = "m_nTessellationMaxTessellation";
        (*property_map)[0x196409DE] = "m_fOpenOcclusionBlendDistance";
        (*property_map)[0x694A6C39] = "m_bWasSpawned";
        (*property_map)[0x99C20388] = "m_aPromptTextPassive";
        (*property_map)[0xDF70BA57] = "m_vNormal";
        (*property_map)[0x878B300A] = "m_vDirection";
        (*property_map)[0xF2FFFE69] = "m_fAlpha";
        (*property_map)[0x7E7E2F2] = "m_eInputNavigateRight";
        (*property_map)[0x229E820A] = "m_fAngle";
        (*property_map)[0xCF57D5E1] = "m_aShapes";
        (*property_map)[0xCA62F6B2] = "m_aViewportRatios";
        (*property_map)[0x89C64223] = "m_vUVOffset";
        (*property_map)[0xB1556E41] = "m_GlowAlliesUnconsciousColor";
        (*property_map)[0xBCFF82D4] = "nCurrentAmmoType";
        (*property_map)[0x911798D9] = "m_NoLOSMachineGunFirePatterns";
        (*property_map)[0x8C9C0B6C] = "m_bReInit";
        (*property_map)[0xE9409E68] = "m_eTraversalDir";
        (*property_map)[0x9E33316E] = "bHolstered";
        (*property_map)[0xB79FECB6] = "m_nMaxNumVisibleCharacters";
        (*property_map)[0x6A3A2F33] = "m_nCrowdLODCount0";
        (*property_map)[0x9A3112E7] = "m_fMieAnisotropy";
        (*property_map)[0x1D3D1FA5] = "m_nCrowdLODCount1";
        (*property_map)[0x84344E1F] = "m_nCrowdLODCount2";
        (*property_map)[0xE20D01B0] = "m_bUseOcclusionForCulling";
        (*property_map)[0xADF8432E] = "m_nCrowdFarCullDist";
        (*property_map)[0xB556E12E] = "m_fReverbBlendDistance";
        (*property_map)[0x497AAB47] = "m_fMinTimeDiff";
        (*property_map)[0xF87CFCDD] = "m_bDone";
        (*property_map)[0x31EB3E37] = "m_bIsMainCharacter";
        (*property_map)[0xB3C9A403] = "m_eFootwear";
        (*property_map)[0x810D0E6C] = "m_SlitThroatEmitter";
        (*property_map)[0x97BBBBFF] = "m_Blunt1HEmitter";
        (*property_map)[0xAE36873A] = "m_Blunt2HEmitter";
        (*property_map)[0xD44B1970] = "m_ShatterEmitter";
        (*property_map)[0x77ABF5B5] = "m_rHitmanPosition";
        (*property_map)[0xC1886EE1] = "m_Storage";
        (*property_map)[0xF8837E3D] = "m_AxePullEmitter";
        (*property_map)[0x9BAE0462] = "m_AxePullSlitThroatEmitter";
        (*property_map)[0x838D2B16] = "m_rSlitThroatDecal";
        (*property_map)[0x2C1644FA] = "shadingModel";
        (*property_map)[0xC122743] = "m_rStabDecal";
        (*property_map)[0xD67EF90F] = "m_rBlunt1HDecal";
        (*property_map)[0x94CA2B0C] = "m_rShatterDecal";
        (*property_map)[0x46D01CC6] = "m_rAxeCleaveDecal";
        (*property_map)[0xFB505239] = "m_pMaterialHeroFlesh";
        (*property_map)[0xF842461E] = "m_rAxePullsDecal";
        (*property_map)[0xCA015831] = "m_bSetLocalPosition";
        (*property_map)[0x2D59DAEA] = "m_rAxePullsSlitThroat";
        (*property_map)[0xBFAD8115] = "m_CanGuardsInvestigate";
        (*property_map)[0x71C33DDD] = "m_NumStabEmitters";
        (*property_map)[0x21429640] = "m_NumAxeCleaveEmitters";
        (*property_map)[0x3BD988AF] = "m_NumBloodEmitters";
        (*property_map)[0xE0A57FFB] = "m_FistEffects";
        (*property_map)[0x2B02ADBC] = "m_bInvestigateDeadBody";
        (*property_map)[0xF4F118A9] = "m_BluntEffects";
        (*property_map)[0x6C0941AB] = "m_sNamespace";
        (*property_map)[0x8BFDDE14] = "m_sKey";
        (*property_map)[0x90AB82F5] = "m_pGuiGroup";
        (*property_map)[0x2590C0B8] = "m_outfitId";
        (*property_map)[0x3750014B] = "m_rCover";
        (*property_map)[0x700ED65] = "m_bClampToCoverCorners";
        (*property_map)[0xB9ECC46] = "m_bDrawTorque";
        (*property_map)[0x3108C71F] = "m_fPacifyThreshold";
        (*property_map)[0xEC0A0959] = "m_aEventsEnabledAfterInit";
        (*property_map)[0xFD14F1B4] = "m_fBloodyKillThreshold";
        (*property_map)[0x1FF143AA] = "m_bShowOnMainMap";
        (*property_map)[0xC23DA113] = "m_nMinShadowLOD";
        (*property_map)[0x77DDD8C3] = "m_fIntensity";
        (*property_map)[0x64E79C50] = "m_fVelocityScale";
        (*property_map)[0xBAD8C739] = "m_TypeHitman";
        (*property_map)[0x7721C1ED] = "m_bDebugDrawGrid";
        (*property_map)[0x10EE7659] = "m_TypeActorAlive";
        (*property_map)[0xC127C1AA] = "m_fSettingCooldown";
        (*property_map)[0x38094324] = "m_TypeActorDead";
        (*property_map)[0xEDFC28CB] = "m_eDismountDownDirection";
        (*property_map)[0x37F0CB90] = "m_TypeActorPacified";
        (*property_map)[0x121AD400] = "m_fRadiusX";
        (*property_map)[0x84868386] = "m_pVideoResource";
        (*property_map)[0x38F0C0F9] = "m_pOwner";
        (*property_map)[0xCE76286D] = "m_fCausticsIntensity";
        (*property_map)[0x929E0F54] = "m_fBlendOrientation";
        (*property_map)[0xF8E339C4] = "m_SharedEvent";
        (*property_map)[0xB564E916] = "m_fMinAge";
        (*property_map)[0x55DB9504] = "m_vNearAttenuation";
        (*property_map)[0xF537FAB5] = "m_bIsStill";
        (*property_map)[0x40AB556] = "m_rKeywordProfession";
        (*property_map)[0x2E8B4231] = "m_fDirectionFrequency";
        (*property_map)[0xEC9F70D1] = "m_fGridOffsetSec";
        (*property_map)[0xF83A07CD] = "m_rKeywordProfessionRank";
        (*property_map)[0x3CFBBD73] = "m_fLoiteringInitialDelay";
        (*property_map)[0x8278E445] = "m_nEndIndex";
        (*property_map)[0x6CF9D5DA] = "aCategories";
        (*property_map)[0xEF589E37] = "m_bBounce";
        (*property_map)[0xF2EB4552] = "m_aBoxes";
        (*property_map)[0x2E9310BE] = "m_fLFSpeed";
        (*property_map)[0x3E404D75] = "m_fHFHorizontalInt";
        (*property_map)[0x8A9DD6DE] = "m_tFriskCooldown";
        (*property_map)[0x5A670238] = "m_fDisplaceNormalInt";
        (*property_map)[0xB9678AC] = "m_fDepthSizeY";
        (*property_map)[0xAA58E922] = "m_fDepthPosY";
        (*property_map)[0x5F2C189A] = "m_fWaterTessellationFactor";
        (*property_map)[0xA1DB9C0C] = "m_bCLPDisabled";
        (*property_map)[0x3550F6FD] = "m_fWaterTessellationNear";
        (*property_map)[0x6C05F951] = "m_eMeleeDamageBehavior";
        (*property_map)[0x659429DB] = "icon";
        (*property_map)[0xE2DBE4EA] = "m_fCausticsGlobalUVScale";
        (*property_map)[0xF1EEB6F2] = "m_fCausticsRotationMagnitude";
        (*property_map)[0x3459E211] = "m_bActiveOnStart";
        (*property_map)[0xEE854C35] = "m_fDensity";
        (*property_map)[0x498A65F8] = "m_fCausticsDrift";
        (*property_map)[0x5BEB2455] = "aRepositoryAssetIds";
        (*property_map)[0xEB60C939] = "m_bValueOnNoOperators";
        (*property_map)[0x9C034A82] = "m_MainFrustums";
        (*property_map)[0x447ADD95] = "m_MainBackOccluders";
        (*property_map)[0x90A211E9] = "m_bCheckHasItemOnBack";
        (*property_map)[0xA95FAA57] = "m_fLightBleedingReduction";
        (*property_map)[0xD886239E] = "m_bCheckEquippedItemLegal";
        (*property_map)[0x7CEC9209] = "m_eCheckEquippedMaxLegalGameTensionLevel";
        (*property_map)[0x76433992] = "m_Radius";
        (*property_map)[0x9183C6CE] = "m_eHolsterAbilityToCheck";
        (*property_map)[0x792BC7B7] = "m_ItemKeyToFind";
        (*property_map)[0xEC64146B] = "m_DoorData";
        (*property_map)[0xF051EF1F] = "m_movementState";
        (*property_map)[0x5EEA3BC1] = "m_BulletsToFire";
        (*property_map)[0xF2085BB2] = "m_fMuzzleVelocity";
        (*property_map)[0xD96D17AE] = "m_eActionType";
        (*property_map)[0x9FAE6F37] = "m_ChestAvoidanceGhostDeactivationAngle";
        (*property_map)[0xC97BA0C] = "m_fShortScale";
        (*property_map)[0xDF0BE4FF] = "m_eFilterResult";
        (*property_map)[0xF418E734] = "m_eInputIsApplied";
        (*property_map)[0xBC5CF252] = "m_situationStartTime";
        (*property_map)[0x18F443BD] = "m_aConditionIdentifiers";
        (*property_map)[0xAF06FE2D] = "m_Back1Attacher";
        (*property_map)[0x17BB29A9] = "m_bDynamicAnimSet";
        (*property_map)[0x30D68E14] = "m_bAllowActorHitActor";
        (*property_map)[0x7E4BDBFD] = "m_bUnnoticed";
        (*property_map)[0x6829FCC6] = "m_bIsOpen";
        (*property_map)[0x4461FBBE] = "m_fThicknessSource";
        (*property_map)[0x41F9C777] = "m_fThicknessDestination";
        (*property_map)[0x94468262] = "m_fLateralMargin";
        (*property_map)[0x21133D1F] = "m_fDefaultActivationAngle";
        (*property_map)[0x5D457BEB] = "m_fFollowDistance";
        (*property_map)[0xB093BEE1] = "m_fLateralMarginRight";
        (*property_map)[0x363315BE] = "m_ViewPort";
        (*property_map)[0x2C7805D3] = "m_fSSRIntensity";
        (*property_map)[0x591896B1] = "m_fVerticalMargin";
        (*property_map)[0x736618F0] = "m_bIncludeRepeated";
        (*property_map)[0xD67F789E] = "m_fVerticalMarginTop";
        (*property_map)[0x43BB72FB] = "m_bJumpOverCover";
        (*property_map)[0xF0E79173] = "m_sDebugActiveBreadcrumbConfig";
        (*property_map)[0x9BBA7143] = "m_aGeomSetups";
        (*property_map)[0xF21FDB7F] = "m_fOcclusionMinDistance";
        (*property_map)[0xE226395C] = "m_StartBehavior";
        (*property_map)[0xF5124380] = "m_fDestinationDiffractionScaling";
        (*property_map)[0xBC43EB0A] = "m_HDRLensFlareColorC";
        (*property_map)[0xEB4DD0CC] = "m_fMaxDistance";
        (*property_map)[0xE53521AD] = "m_AmbienceSide1";
        (*property_map)[0x220F2064] = "m_FadeInCurve";
        (*property_map)[0x8E206219] = "m_vPositionOffset";
        (*property_map)[0x4057CF9B] = "m_OcclusionTransitionCurve";
        (*property_map)[0xABC60F59] = "m_rItemEntity";
        (*property_map)[0x718FBCFB] = "m_camera";
        (*property_map)[0x9A1FB2CC] = "m_bNoWarningDialog";
        (*property_map)[0xFC14B52C] = "m_fRadiusZ";
        (*property_map)[0x6B4E9B44] = "m_aUnconsciousGuards";
        (*property_map)[0xA0C71E73] = "m_fMinAttentionToForceLookAt";
        (*property_map)[0x451F7FC7] = "m_fRadiusZBottom";
        (*property_map)[0xDE1A149E] = "m_fHuntedAttractionRunPenalty";
        (*property_map)[0x1DDB54AE] = "m_nAllowRunRadius";
        (*property_map)[0x7873CDDA] = "m_fTop";
        (*property_map)[0x223C063B] = "m_rMockClothbundle";
        (*property_map)[0x9433F82C] = "m_bCameraObstacle";
        (*property_map)[0x63594B60] = "m_rMaterialOverrideFactory";
        (*property_map)[0x1F3B1D44] = "m_bSpotted";
        (*property_map)[0x7C4F4558] = "m_vHitmanEnterPosition";
        (*property_map)[0x739BB64E] = "m_GlowEnforcerOutlineColor";
        (*property_map)[0xD25AD01E] = "m_pEmitterProxy";
        (*property_map)[0xE00833E0] = "m_cPrimingCurve";
        (*property_map)[0x44626E00] = "m_bCollisionGroup0";
        (*property_map)[0x430FAA19] = "m_bCollisionGroup4";
        (*property_map)[0x884ABCD4] = "m_fPrecisionShotBulletSpeedMultiplier";
        (*property_map)[0xAD01CB35] = "m_bCollisionGroup6";
        (*property_map)[0x4D21E771] = "m_spatialEntity";
        (*property_map)[0xDA06FBA3] = "m_bCollisionGroup7";
        (*property_map)[0x8E81B488] = "m_rSoundEvent_CloseCombat_FiberWire";
        (*property_map)[0x9A96F48A] = "m_bWaitIfPending";
        (*property_map)[0x2988BEF3] = "m_nRouteID";
        (*property_map)[0x3CED0087] = "m_fYawSpeedFast";
        (*property_map)[0xA9FEA97C] = "m_bCantFleeNoPath";
        (*property_map)[0x31F34041] = "m_rIntelGroupData";
        (*property_map)[0x87D7B6D9] = "m_fPitchSpeed";
        (*property_map)[0xA27ED710] = "m_fPitchSpeedFast";
        (*property_map)[0xCB9867C0] = "m_ridImage";
        (*property_map)[0x9E114DAA] = "m_bDebugTriggerState";
        (*property_map)[0x75DAA7DE] = "m_bDrawShadows";
        (*property_map)[0xDB0C9F7D] = "m_bFightBack";
        (*property_map)[0x5F39B841] = "m_fActTimeout";
        (*property_map)[0x8CCEC8E7] = "m_bDrawCrowds";
        (*property_map)[0x8E8E371] = "m_fMonitorDistance";
        (*property_map)[0xD9AF59BF] = "m_bDrawDecals";
        (*property_map)[0x6C507A2F] = "m_bDrawTransparent";
        (*property_map)[0x7F60529C] = "m_bDrawAtmosphericScattering";
        (*property_map)[0xDE31ED8] = "m_fLostSightReactionDelay";
        (*property_map)[0x4D65686B] = "m_sourceLocation";
        (*property_map)[0x6C66AB72] = "m_fHeight";
        (*property_map)[0xA324B211] = "m_fOffsetFromSide";
        (*property_map)[0x35F2A407] = "m_eSystemPhysicsType";
        (*property_map)[0xF2E59A7] = "m_BaseEventConsumers";
        (*property_map)[0x2EBD80C8] = "m_bAccessibleFromLeft";
        (*property_map)[0xA2E14197] = "m_fGlobalLightOverrideBackIntensity";
        (*property_map)[0xB44AE60B] = "m_fOverrideInteractionRangeValue";
        (*property_map)[0x69A3278A] = "m_bClosed";
        (*property_map)[0xE7EE8EAE] = "m_bFoundDeadTarget";
        (*property_map)[0x22149FEF] = "m_eMenuIcon";
        (*property_map)[0x2AF829B9] = "m_bPushVictim";
        (*property_map)[0xDA84C079] = "m_sEnterTextID";
        (*property_map)[0x2F123FDC] = "aSubSubStructs";
        (*property_map)[0xA9484C59] = "m_nTargetValueString";
        (*property_map)[0xF3D18EFC] = "m_rMessageResourcePS4";
        (*property_map)[0x27FBD9A4] = "m_fGlowGunsOutlineWidth";
        (*property_map)[0x133DE48D] = "m_fAnimation";
        (*property_map)[0x7A22B22E] = "m_nBehaviorTreeIndex";
        (*property_map)[0xB81A12BC] = "m_bRotateIndicator";
        (*property_map)[0x35827A6D] = "m_bByPathfinder";
        (*property_map)[0x4F70E3F2] = "m_AreaBoxes";
        (*property_map)[0x3A6410F8] = "m_bNeedDataUpdate";
        (*property_map)[0xB16D2E23] = "typeName";
        (*property_map)[0x40EEDBAD] = "typeFlags";
        (*property_map)[0xB0C1D517] = "m_nMaxDispatchesPerCamera";
        (*property_map)[0x7E185F31] = "m_fCombatRadiusSecondaryRange";
        (*property_map)[0x81897A22] = "m_HitmanVisibleEscalationRules";
        (*property_map)[0x1B7F224A] = "m_aKeys";
        (*property_map)[0xCEF77C91] = "m_bIs2DPlane";
        (*property_map)[0x5CDBF528] = "m_bContractsCreationMode";
        (*property_map)[0x471FF4D4] = "m_nTagCount";
        (*property_map)[0xAD0D9EA4] = "m_bTaggingEnabled";
        (*property_map)[0x31512802] = "m_fTransitionTime";
        (*property_map)[0x28418723] = "m_aActorTagData";
        (*property_map)[0x16EBEA4A] = "m_CanVIPsInvestigate";
        (*property_map)[0xB00F926C] = "m_fStrengthFrequency";
        (*property_map)[0xB6D6101D] = "m_pLocationVolume";
        (*property_map)[0xB4D14BB9] = "m_bRepositoryId";
        (*property_map)[0xBAD3894A] = "m_vTargetRadii";
        (*property_map)[0x8CFFD114] = "m_sSignalName";
        (*property_map)[0x16372BE0] = "m_bGrabNPCEnabled";
        (*property_map)[0x64AF7114] = "m_sHudIconDisabled";
        (*property_map)[0xC579E852] = "m_fCornerDistanceModify";
        (*property_map)[0x6027B65] = "m_eMoveEndState";
        (*property_map)[0xAC0176D7] = "m_aActorProvidersData";
        (*property_map)[0xED129976] = "m_bCrouch";
        (*property_map)[0xB2F27E53] = "m_bEndInCrouch";
        (*property_map)[0x98646C91] = "m_bAllowStrafing";
        (*property_map)[0x35DD311B] = "m_pWaypoints";
        (*property_map)[0x31355ADA] = "m_bOnlyRepositioning";
        (*property_map)[0xCDC859C1] = "m_eRepositioningStrategy";
        (*property_map)[0xA3D8FD17] = "m_MinimumAnchorDistance";
        (*property_map)[0x55C4542A] = "m_bIgnoreOrientation";
        (*property_map)[0xB984F8AF] = "m_bReactOnlyIfNeeded";
        (*property_map)[0xC4C4C03F] = "m_eWaitCondition";
        (*property_map)[0x2DCBA4FE] = "m_pAimAt";
        (*property_map)[0x65F52B25] = "m_aPreferredEntourage";
        (*property_map)[0xAE8180F9] = "m_sAct";
        (*property_map)[0x2D8996AE] = "m_optionKey";
        (*property_map)[0x995B7B63] = "m_fActDuration";
        (*property_map)[0xD17E6162] = "m_pLookAt";
        (*property_map)[0x29776D6] = "m_pAttentionHUDControl";
        (*property_map)[0x64A9A4D0] = "m_pFaceTowards";
        (*property_map)[0x6B21D8EF] = "m_bStopAtEntityDeletion";
        (*property_map)[0x63E8CA39] = "m_bSavable";
        (*property_map)[0xCBE7B4A7] = "m_eCullingBehavior";
        (*property_map)[0x96B7F8C6] = "m_eTransitionCurve";
        (*property_map)[0x79A76743] = "Attenuation";
        (*property_map)[0x350583F2] = "m_fSkipAllowedDelay";
        (*property_map)[0x61656E39] = "LowPassFilter";
        (*property_map)[0xA591AEC2] = "m_Agent_SlotVersions";
        (*property_map)[0x5F91AEDE] = "m_StimuliSaveData";
        (*property_map)[0xE75D7AF4] = "m_rEventLocation";
        (*property_map)[0x1D25596E] = "m_GlowContractTargetNonCriticalOutlineColor";
        (*property_map)[0x3DA6147A] = "m_eRuntimeMemoryAllocationState";
        (*property_map)[0xBDDE8F4F] = "m_sPinName";
        (*property_map)[0x7544B575] = "m_nSelectedEffect";
        (*property_map)[0x3FE5E9BD] = "m_eIcon";
        (*property_map)[0xC8775CD1] = "m_eAnimMode";
        (*property_map)[0xF67FE001] = "m_eSlot";
        (*property_map)[0x3321E6B6] = "m_bAmbienceActivated";
        (*property_map)[0x954E23F7] = "m_bTriggerByAmbience";
        (*property_map)[0x80A8EE25] = "m_vip";
        (*property_map)[0x41E041B3] = "m_fAngularDeviationY";
        (*property_map)[0x7823A9EF] = "m_animationName";
        (*property_map)[0xCDD15E6C] = "m_CollisionType";
        (*property_map)[0x2A709A15] = "m_aWaypointActs";
        (*property_map)[0x75B49AAC] = "m_CollisionEntities";
        (*property_map)[0x6B247493] = "m_bInverted";
        (*property_map)[0x89170B85] = "bDisguiseBroken";
        (*property_map)[0x36DA46A7] = "bTrespassing";
        (*property_map)[0xF8E72AA9] = "bNearDeadBody";
        (*property_map)[0x9AE24EF8] = "m_aIncidents";
        (*property_map)[0x95023FC2] = "bDisguiseSuspicious";
        (*property_map)[0xA453860D] = "bDeepTrespassing";
        (*property_map)[0xB0732F1F] = "m_aTracksAndGroups";
        (*property_map)[0xD077929F] = "m_startTime";
        (*property_map)[0x8E58EB01] = "m_mMatVal";
        (*property_map)[0x49734613] = "m_bUpdateOnGameTime";
        (*property_map)[0xFC599983] = "m_eStealthSituation";
        (*property_map)[0x32F5CA2F] = "m_pVisibilityData";
        (*property_map)[0x2CCB591A] = "m_nLoopCount";
        (*property_map)[0xBC49B207] = "m_bLetterbox";
        (*property_map)[0x6DF4208F] = "m_CooldownExpirationTimestamp";
        (*property_map)[0xC7EF8783] = "m_letterboxFadeIn";
        (*property_map)[0x5CA7D09D] = "m_letterboxFadeOut";
        (*property_map)[0x40314EDE] = "m_bFadeOutBeforeEnd";
        (*property_map)[0x97D84153] = "m_pVideoPlayer";
        (*property_map)[0xBE3D8F41] = "m_bResetOnStop";
        (*property_map)[0x3A2E3CDD] = "m_ObjectviveId";
        (*property_map)[0xF25D6BC6] = "newDistraction";
        (*property_map)[0xEE6AB802] = "m_VariationList";
        (*property_map)[0xB3256A06] = "m_replicationFrequency";
        (*property_map)[0xCA3621C1] = "m_vColliderAngularVelocity";
        (*property_map)[0xA3DF935D] = "m_fLODScaleLinked";
        (*property_map)[0x7F3DC1C9] = "m_Outfit";
        (*property_map)[0xD54D763C] = "m_bPreviewInStopMode";
        (*property_map)[0x8182418] = "m_bSharedResource";
        (*property_map)[0xB76DE509] = "m_fBlendPosition";
        (*property_map)[0x18653AB9] = "m_fCurveMultiplier";
        (*property_map)[0x5DA88FB7] = "m_eUpdateStrategy";
        (*property_map)[0x64D13BE0] = "m_pClothBundle";
        (*property_map)[0x22EA0B36] = "m_aSituationTypes";
        (*property_map)[0x37B33386] = "m_fMinimum";
        (*property_map)[0x17521CC2] = "m_sColor";
        (*property_map)[0x8DAA72C7] = "m_fDuration";
        (*property_map)[0x32CA8C34] = "m_nPlayerId";
        (*property_map)[0xF2DA9F85] = "m_aEntityRefs";
        (*property_map)[0x7C34CC19] = "m_fPropagationGracePeriod";
        (*property_map)[0x193674BC] = "m_cells";
        (*property_map)[0x162A4319] = "mappings";
        (*property_map)[0x40A47BA9] = "m_TextLineXBS";
        (*property_map)[0x8561E1ED] = "m_bGlobal";
        (*property_map)[0x65DBA11B] = "m_uRecenterRequestCounter";
        (*property_map)[0x6E1B832C] = "m_fGlowPlayerInLVAOutlineWidth";
        (*property_map)[0xD8E91009] = "m_fAngularDeviationZ";
        (*property_map)[0xB69D15F7] = "m_bUpdateKnownPosition";
        (*property_map)[0xE1603B8D] = "m_fSpeedTreeStrength";
        (*property_map)[0x384EF60D] = "m_SubTextLine";
        (*property_map)[0x58AA8A9F] = "m_eRoomLightFlow";
        (*property_map)[0x49A403B9] = "m_rValueInt";
        (*property_map)[0x4E356D7E] = "m_DynamicDisguiseAttentionDuration";
        (*property_map)[0xCE75C021] = "m_nCompareTo";
        (*property_map)[0x1E8322E2] = "m_bPlayDuringStoryDrama";
        (*property_map)[0xBE212D55] = "m_fDisguiseAttractionTimeInCrowdFactor";
        (*property_map)[0xE1440571] = "m_fRightHandRotationAngles";
        (*property_map)[0x8106E8AC] = "m_fSpringDamper";
        (*property_map)[0xDCF175DF] = "m_CombatActCooldownTime";
        (*property_map)[0x5CA9D162] = "m_nCurrentIndex";
        (*property_map)[0xE4F8F9C0] = "m_nHighDensityMinZones";
        (*property_map)[0x93B0BE50] = "m_bTriggerWhenIndexChanged";
        (*property_map)[0x6F257773] = "m_interactionData";
        (*property_map)[0x9221CDAC] = "m_bFogUseQuarterRes";
        (*property_map)[0x4BDEF4BF] = "m_aDamageData";
        (*property_map)[0x7DB71C28] = "m_pGeneric02Sound";
        (*property_map)[0x7CBF0550] = "m_bHasSystemBeenDetached";
        (*property_map)[0x23A6AABC] = "m_eFaction";
        (*property_map)[0xA3ADE68F] = "m_bHasSystemBeenFractured";
        (*property_map)[0x6605D98C] = "IsElevationDifferenceEnabled";
        (*property_map)[0x13AD1A62] = "m_fGameTimeMultiplier";
        (*property_map)[0x74B72FE9] = "m_rIKConfig";
        (*property_map)[0x41FDE3D4] = "bIsHidden";
        (*property_map)[0x319B9E70] = "attributes";
        (*property_map)[0xE5876260] = "m_vRightFadeAngles";
        (*property_map)[0x68EF4BFC] = "m_isEnabled";
        (*property_map)[0x21E0F3A4] = "Aperture";
        (*property_map)[0x4B1EFC02] = "active";
        (*property_map)[0xE74C7616] = "m_fMaxSpeed";
        (*property_map)[0xA9F53C59] = "m_fDistanceFieldValue";
        (*property_map)[0x4CED470C] = "m_bDialogPreventShooting";
        (*property_map)[0x987E012E] = "m_bLegalizeAiming";
        (*property_map)[0x122C8BD0] = "m_bReportedToGuard";
        (*property_map)[0x823EF313] = "m_HelpTargetKnowledgeIndex";
        (*property_map)[0xF89DA72A] = "supportedTypes";
        (*property_map)[0xF6235863] = "m_fAngularVelocity";
        (*property_map)[0xF2309412] = "m_bUnlockedByDefault";
        (*property_map)[0x8C1FF554] = "m_rRepositoryIdHolder";
        (*property_map)[0x86EB6719] = "m_pVersusRaceGhostFactory";
        (*property_map)[0xBF5B820C] = "m_eVerticalAlignment";
        (*property_map)[0xA57EA206] = "m_bScreenAlign";
        (*property_map)[0x99C9A496] = "m_stateData";
        (*property_map)[0xBDBF1FFF] = "m_vFogTurbulenceNearScale";
        (*property_map)[0xFEA57770] = "m_occupancyNode";
        (*property_map)[0x37D6064] = "m_fInitialMagazines";
        (*property_map)[0x8C271BC6] = "m_vEnvCollisionsBoxSizeThreatening";
        (*property_map)[0x3C8EFE76] = "m_eInputNavigateUp";
        (*property_map)[0x571258C3] = "m_eInputNavigateDown";
        (*property_map)[0x21A0DE39] = "m_aActions";
        (*property_map)[0x98BFF96B] = "m_nLifeSpan";
        (*property_map)[0x52BB4FAF] = "m_aMenuEntries";
        (*property_map)[0x6529335C] = "m_bSetGlowTypeWhenChanged";
        (*property_map)[0xC3EEED0C] = "m_bMountableFromTop";
        (*property_map)[0x8368C455] = "m_bAllowDropDown";
        (*property_map)[0xE744061A] = "m_bAllowJumpToLedgeRight";
        (*property_map)[0x67D8F2AD] = "m_eDismountUpBehavior";
        (*property_map)[0xEB49CD75] = "m_bPacifyKickedNPC";
        (*property_map)[0x4311CCDA] = "m_bBlockVisionAmbient";
        (*property_map)[0x4BC3E3D2] = "m_vDollyRotation";
        (*property_map)[0x8FC5AD52] = "m_VolumeEntity";
        (*property_map)[0x3E05A562] = "m_vGlowTagFocusTaggedVignetteRadii";
        (*property_map)[0x249412A4] = "m_eVoiceVariation";
        (*property_map)[0x9771C687] = "m_vKnockdownRadii";
        (*property_map)[0x42560041] = "m_nWidth";
        (*property_map)[0xC6C4E619] = "m_fLayer";
        (*property_map)[0x92804063] = "m_fOutputScale";
        (*property_map)[0x89438F43] = "m_ridTemplate";
        (*property_map)[0x3F9F54F0] = "m_bNeedsGamma";
        (*property_map)[0xD6398BB8] = "m_bDrawUI";
        (*property_map)[0x5106FDCD] = "m_nColumns";
        (*property_map)[0xAA4A84A4] = "m_fMargin";
        (*property_map)[0xE349E70F] = "m_aCellsPositions";
        (*property_map)[0x3D4D6A2C] = "m_bLocalize";
        (*property_map)[0x4D44C346] = "m_ePreCustomAnimSet";
        (*property_map)[0x31775FE9] = "npcToRequest";
        (*property_map)[0x3EFEEC41] = "m_CurrentBehaviorState";
        (*property_map)[0x3A06AC3D] = "button";
        (*property_map)[0x6825DE79] = "m_vAlphaAttenuationAngles";
        (*property_map)[0xDA65C954] = "m_nMaxLOD";
        (*property_map)[0x5E13DB29] = "m_bOnlyAmbientMood";
        (*property_map)[0xDBB24B67] = "m_bAlphaAttenuation";
        (*property_map)[0x9426ADCB] = "m_bCulling";
        (*property_map)[0xBDD68843] = "condition";
        (*property_map)[0x44FA7277] = "m_Left";
        (*property_map)[0xB1950E87] = "m_contractId";
        (*property_map)[0x80736701] = "index";
        (*property_map)[0x1B3E5B74] = "m_bLeaderMovingBack";
        (*property_map)[0xE0E861BD] = "headline";
        (*property_map)[0xC503AB51] = "m_fSwing2LimitOffset";
        (*property_map)[0x99901C93] = "m_sFaceExpression";
        (*property_map)[0x6A84ABC3] = "m_vPrimitiveScale";
        (*property_map)[0x1B0A3AF5] = "m_fMinReprojectionValue";
        (*property_map)[0x72836808] = "m_Reference";
        (*property_map)[0x16775CDB] = "m_rInteractionPoint";
        (*property_map)[0x8C9EFDAF] = "m_fGridDurationSec";
        (*property_map)[0x6C7324A9] = "m_fVignetteMin";
        (*property_map)[0x94D794E5] = "containedIcon";
        (*property_map)[0xDF9B3B62] = "nPropertyID";
        (*property_map)[0x578475DF] = "m_eOrderType";
        (*property_map)[0xBFF20468] = "m_bIsSparse";
        (*property_map)[0x17A33A9A] = "m_bStandingDown";
        (*property_map)[0x77B7FF8C] = "m_entries";
        (*property_map)[0x66D1BF59] = "m_aNonObserverMembers";
        (*property_map)[0xDDA95E1E] = "m_aEnabledEntranceEquip";
        (*property_map)[0x406D4DB0] = "opportunities";
        (*property_map)[0xBDBB2739] = "m_fFarRadius";
        (*property_map)[0x38213F07] = "m_fSustain";
        (*property_map)[0x3CBD163C] = "m_fRangeH";
        (*property_map)[0xC6B22B5F] = "m_fRangeV";
        (*property_map)[0x8A1B8D02] = "m_fRangeRoll";
        (*property_map)[0x1A32DB33] = "m_bDisableInExternalBuilds";
        (*property_map)[0xDC6AE126] = "m_sTargetPageName";
        (*property_map)[0xEDA28C82] = "m_eDialog";
        (*property_map)[0x32678FD7] = "m_bRemoveDisabledActors";
        (*property_map)[0x49F80CFB] = "m_pAnimLocation";
        (*property_map)[0xA676A4AB] = "m_fNPCMaxAngle";
        (*property_map)[0x161ACC64] = "m_eRequirementsEvaluationType";
        (*property_map)[0x6E79BD77] = "m_bUseTargetDialog";
        (*property_map)[0xE4961D7A] = "m_bDesignOnlyBool";
        (*property_map)[0x6E1FCEF4] = "m_rDesignOnlyRef";
        (*property_map)[0xA6D5CE0] = "m_pNonStreamedResPtr";
        (*property_map)[0xDDEDDBCC] = "m_bDistortionBarrelUseRealTime";
        (*property_map)[0xB600E62B] = "m_fPoweredJointFallStrengthScale";
        (*property_map)[0xCC18AB42] = "m_vVec2Val";
        (*property_map)[0xDC01D808] = "m_bFriendlyFire";
        (*property_map)[0x61F9F160] = "m_aEntityRefVal";
        (*property_map)[0xD4184839] = "nColumnIndex";
        (*property_map)[0xA5ED2CE3] = "m_bEndAfterPutDownLast";
        (*property_map)[0xE30CCF6F] = "m_ItemKeywordProxiesData";
        (*property_map)[0x89E2044E] = "nRowIndex";
        (*property_map)[0xE0411DEC] = "m_bHMShootingReactionEnabled";
        (*property_map)[0x6FABBF71] = "nColumnSpan";
        (*property_map)[0x97FF18B7] = "m_rPinForward";
        (*property_map)[0x366C5CF8] = "m_fHeadBoneAttacherOffset";
        (*property_map)[0x7C8298CA] = "m_aEnterEffects";
        (*property_map)[0xF57868EF] = "m_mAnchorRotOffset";
        (*property_map)[0x598512A3] = "m_rCategoryResource";
        (*property_map)[0xDE7108DB] = "m_rMessageResourceXB1";
        (*property_map)[0xE4BE04CD] = "m_NextLevel";
        (*property_map)[0x670CA539] = "m_tLastHunt";
        (*property_map)[0x711F3689] = "m_pIntel";
        (*property_map)[0xDC9124DD] = "m_fImpactForce";
        (*property_map)[0xD3E5A07] = "bHasItemToShow";
        (*property_map)[0xCF2F919] = "m_rAnimConfig";
        (*property_map)[0x134DA28A] = "m_eDeathType";
        (*property_map)[0xB5E42EC1] = "m_fRagdollImpactForceGlobal";
        (*property_map)[0x2A17A932] = "bSuspicious";
        (*property_map)[0xC52EC54E] = "m_fTrailsOverlap";
        (*property_map)[0x524EB1F9] = "m_nMaxShadowLOD";
        (*property_map)[0x1F5A198A] = "m_sPauseRequestID";
        (*property_map)[0x16FF7F3A] = "m_fTrespassingPeripheralAngle";
        (*property_map)[0xA6A7F366] = "m_bRepulsive";
        (*property_map)[0x57ACB221] = "m_rMorphemeEntity";
        (*property_map)[0x817C5977] = "m_CameraConfig";
        (*property_map)[0xCC87575D] = "m_rVRBoneAnimator";
        (*property_map)[0xD186694C] = "m_fPostPoneGreetingCooldown";
        (*property_map)[0x559ADD10] = "m_fConeAngleY";
        (*property_map)[0x880239F1] = "m_fConeSpringForce";
        (*property_map)[0x42D97AA4] = "m_fConeSpringDamper";
        (*property_map)[0xCED77756] = "m_bDrawDebug";
        (*property_map)[0x19D40F2E] = "m_fAtTargetEvaluationDistance";
        (*property_map)[0x77586DBF] = "m_rPrioritizedItemKeyword";
        (*property_map)[0x6CDDB955] = "m_nNumberOfPickups";
        (*property_map)[0xF76EC26B] = "m_aSlots";
        (*property_map)[0x392F9DDA] = "m_aPendingPickupItems";
        (*property_map)[0x8AECCD6C] = "m_bExplosionBlocker";
        (*property_map)[0xC32D87F4] = "m_nAmmoCountRevolver";
        (*property_map)[0xC6871CE3] = "m_aIsCurrent";
        (*property_map)[0x44186357] = "m_nAmmoCountRifle";
        (*property_map)[0xAD61665E] = "m_nAmmoCountSniper";
        (*property_map)[0x2642243D] = "m_nAmmoCountLightPistol";
        (*property_map)[0x300C3197] = "m_nNumSamples";
        (*property_map)[0x147CC672] = "m_nAmmoCountAmmoShotgunBeanbag";
        (*property_map)[0xEF850035] = "m_sResourceName";
        (*property_map)[0x7D034182] = "m_aAmmoCounts";
        (*property_map)[0x41FD2805] = "m_pMovieResource";
        (*property_map)[0x685E52D7] = "m_fMarginBottom";
        (*property_map)[0xC2F59821] = "m_fMarginLeft";
        (*property_map)[0x1645CBF5] = "m_fPlayerDistanceInterpolation";
        (*property_map)[0x6DA7BAAA] = "m_PenaltyInvestigateCautious";
        (*property_map)[0x8CAD3BCC] = "m_fFogTurbulenceOpacity";
        (*property_map)[0x7DE75553] = "m_sRTPCName";
        (*property_map)[0x7CCBD6C4] = "m_rawPrimary";
        (*property_map)[0xD0FFC991] = "m_fRTPCModification";
        (*property_map)[0x5903D6FC] = "m_bIsWalking";
        (*property_map)[0xB5843724] = "m_fDragCorrectionTime";
        (*property_map)[0x50AEB18B] = "m_aGlobalStates";
        (*property_map)[0xFBA8DDD4] = "m_bIsStrafing";
        (*property_map)[0x169F1311] = "m_pTexture";
        (*property_map)[0x984C72F3] = "m_pUiControl";
        (*property_map)[0xD6461973] = "m_fTimeOfDayRangeStart";
        (*property_map)[0x83C120AC] = "m_bAffectsFriskExemptDisguises";
        (*property_map)[0x3E3B852C] = "m_aAIZones";
        (*property_map)[0x1B154DC6] = "m_Containers";
        (*property_map)[0x7E4D01A1] = "m_aEmitterEvents";
        (*property_map)[0xCCC8311F] = "m_aEmitterSwitches";
        (*property_map)[0x7882DB69] = "m_aGlobalRTPCs";
        (*property_map)[0x25A28976] = "m_bLocalSpace";
        (*property_map)[0x20E52CF5] = "m_bOnlyLivingActors";
        (*property_map)[0x4023787] = "m_bDontEscalateOnLineCrossing";
        (*property_map)[0xAA75B4AB] = "m_aExcludedActors";
        (*property_map)[0xB00030DB] = "m_Position";
        (*property_map)[0x493B85AC] = "m_bCheckActorInventoryForKeywords";
        (*property_map)[0x5982F14E] = "m_fDebug_StateModifierLoudness";
        (*property_map)[0xE0BE2935] = "m_aLeaveEffects";
        (*property_map)[0xC5125745] = "m_aRandomEffects";
        (*property_map)[0x52A6701C] = "m_bMakeActorIntrovert";
        (*property_map)[0x8C8B56F6] = "m_bAlwaysSignalOnSet";
        (*property_map)[0x517BF25B] = "m_bTagged";
        (*property_map)[0x88700D26] = "m_fPitchAngle";
        (*property_map)[0xE14C40ED] = "m_fYawAngle";
        (*property_map)[0x918E71D6] = "m_fRotationOffsetY";
        (*property_map)[0x3EBEC183] = "m_bAttached";
        (*property_map)[0x709417DD] = "m_fDrag";
        (*property_map)[0x68781EA] = "m_aTargetBoxes";
        (*property_map)[0x329022A] = "m_fYawBase";
        (*property_map)[0xD997E9A6] = "m_fCharacterRainIntensity";
        (*property_map)[0xE78B2DCC] = "m_bApplyLimitsInstantly";
        (*property_map)[0xEB1C6144] = "m_nGlowTaggedFadeSpeed";
        (*property_map)[0x9B8BDDD7] = "m_fRotationOffsetYYaw";
        (*property_map)[0xB7D1FEAE] = "m_fStickLengthYaw";
        (*property_map)[0xC285324E] = "m_fYawAnimationRange";
        (*property_map)[0xC5571805] = "m_targetControl";
        (*property_map)[0x6F325941] = "m_bPitchAnimation";
        (*property_map)[0x7AA6EED2] = "m_fCollisionHeight";
        (*property_map)[0xD6E68616] = "m_nEventId";
        (*property_map)[0x8C963A70] = "m_bAimPushOut";
        (*property_map)[0x50EB61CB] = "m_bEnableSoftCollision";
        (*property_map)[0x260A499C] = "m_CameraBones";
        (*property_map)[0x327DCCAB] = "m_aSharedKnowledgeSaveData";
        (*property_map)[0x26293B32] = "m_aTrackerData";
        (*property_map)[0x1E5890BE] = "m_bHasRequiredItem";
        (*property_map)[0x9AA6401] = "m_aActorApproachProviders";
        (*property_map)[0xD797B403] = "m_aActorApproachProvidersData";
        (*property_map)[0x43F04974] = "m_bEnableReposition";
        (*property_map)[0x3A3FD2CD] = "m_aActorIKControllersData";
        (*property_map)[0xF7028936] = "m_aOutfitVariation";
        (*property_map)[0xF48A9AA2] = "m_CuriousEventServiceData";
        (*property_map)[0x6E04AECD] = "m_GetHelpServiceData";
        (*property_map)[0x6E06B7C4] = "m_AIModifierServiceData";
        (*property_map)[0xCE782C26] = "m_TargetTrackingServiceData";
        (*property_map)[0xEE74453A] = "m_sEventNameFilter";
        (*property_map)[0x89AC6BB6] = "m_sNotificationCategory";
        (*property_map)[0x59934B91] = "m_sNotificationMessageKey";
        (*property_map)[0xAA28F9FE] = "AddAgentData_HumanoidNPC_Pool_SaveData";
        (*property_map)[0xB3F12A4D] = "RemoveAgentData_BehaviorsDisabled_Pool_SaveData";
        (*property_map)[0x4BAA281B] = "m_GlowGunsColor";
        (*property_map)[0x46746121] = "m_Source2";
        (*property_map)[0xDCEB2CC3] = "RemoveAgentData_GuardHeadDomain_Pool_SaveData";
        (*property_map)[0x3B8AE4A5] = "RemoveAgentData_HumanoidNPC_Pool_SaveData";
        (*property_map)[0x91584385] = "RemoveAgentData_HumanoidWorldState_Pool_SaveData";
        (*property_map)[0x58080237] = "RemoveAgentData_NPC_Pool_SaveData";
        (*property_map)[0x5195246F] = "m_pSvgViewFactory";
        (*property_map)[0x3A119072] = "m_safeRoomNode";
        (*property_map)[0xB72B8220] = "m_eDamageEvent";
        (*property_map)[0xBF70B829] = "m_leader";
        (*property_map)[0xA06E8F8E] = "m_bTargetIsMoving";
        (*property_map)[0x32C906C9] = "m_eTargetActState";
        (*property_map)[0x1DE0FBB8] = "m_ItemsData";
        (*property_map)[0x31231599] = "m_fTargetNotMovingTime";
        (*property_map)[0x6D4E07BE] = "m_aStates";
        (*property_map)[0x2BF00558] = "m_fGridMapHeightOffsetsBias";
        (*property_map)[0xAAE7E417] = "m_aEscortActs";
        (*property_map)[0xBCC2B869] = "bSilenced";
        (*property_map)[0x2997EA4] = "bCanReload";
        (*property_map)[0xA97B35E5] = "fReloadDuration";
        (*property_map)[0x97E58069] = "bContainsItem";
        (*property_map)[0xBF519F41] = "m_tPenalty";
        (*property_map)[0x571CAACF] = "m_bJustShowExit";
        (*property_map)[0x95E0F3BE] = "fLastBulletFiredTime";
        (*property_map)[0x85CE16EC] = "m_aDestinations";
        (*property_map)[0x5D5B47E6] = "m_bUseRightHandItemGrip";
        (*property_map)[0x5E7EE74A] = "m_pCorpseBodybagEntity";
        (*property_map)[0xD006C7D6] = "m_rPerceptibleEntity";
        (*property_map)[0xFF258279] = "m_sNameFilter";
        (*property_map)[0x6B398CEB] = "m_SmudgeDecalsSpawner";
        (*property_map)[0x502BA3D9] = "m_SeasonOneHead4";
        (*property_map)[0xAC2513FF] = "m_TrailDecalsSpawner";
        (*property_map)[0x591ADDDC] = "m_fSampleFrequency";
        (*property_map)[0x75AF0C5A] = "m_bForceAcknowledge";
        (*property_map)[0xAD25C480] = "m_eScreenplayTriggerType";
        (*property_map)[0xB595192B] = "m_bDirectReference";
        (*property_map)[0xAB7FF87A] = "m_SpatialAttractors";
        (*property_map)[0x53A8CBC1] = "fAttention";
        (*property_map)[0x1AC95FFF] = "m_bEnableOnActivate";
        (*property_map)[0x20B8645D] = "m_fSettingDistanceMin";
        (*property_map)[0xF7D0C5E7] = "m_fSettingRemainingPathDistanceMax";
        (*property_map)[0x24829BEE] = "m_fSettingCooldownVariance";
        (*property_map)[0x6C7B86F2] = "m_bInterrupt";
        (*property_map)[0x638C9E33] = "m_eStorageType";
        (*property_map)[0x395A297D] = "propertyOverrides";
        (*property_map)[0x75869AB4] = "m_fCurve";
        (*property_map)[0xE58BCC6E] = "m_bDebugDrawTangents";
        (*property_map)[0x961695AE] = "m_bComsumeInput";
        (*property_map)[0x7FCD5E0F] = "m_aVideDatabases";
        (*property_map)[0x8B4694A7] = "m_fBendContraint";
        (*property_map)[0x83E65581] = "m_fCutoffDistance";
        (*property_map)[0x8179781B] = "m_MaterialPrimitives";
        (*property_map)[0x2B596ABE] = "targetMustBeVisible";
        (*property_map)[0x967A27E] = "vMin";
        (*property_map)[0x235A963A] = "m_vCSMSplitFades";
        (*property_map)[0x3982D501] = "nVisibilityRange";
        (*property_map)[0x4EDEC594] = "AudioLanguages";
        (*property_map)[0x89A3C00C] = "fFloatProp";
        (*property_map)[0xD2D0D43E] = "VideoRidsPerAudioLanguage";
        (*property_map)[0xCDBB87CD] = "m_eMode";
        (*property_map)[0xCC562D6C] = "m_rSourceA";
        (*property_map)[0xF64A4B6A] = "m_eViewportAnchorA";
        (*property_map)[0x5E8D505F] = "m_vDestViewportB";
        (*property_map)[0xB1E81665] = "m_bPerception";
        (*property_map)[0x1F5BC30B] = "m_rChair";
        (*property_map)[0xC4C5AAD6] = "m_vGlowInteractionSelectedVignetteRadii";
        (*property_map)[0xA92B1E87] = "m_fSharpeningStrength";
        (*property_map)[0x8AB8D3AE] = "m_testSuites";
        (*property_map)[0xF6B54A24] = "m_aSources";
        (*property_map)[0x91345D9F] = "m_fVignetteSharpenPower";
        (*property_map)[0x5B1E900C] = "m_fSpringForce";
        (*property_map)[0x8A5B4664] = "m_PinConDesc";
        (*property_map)[0xDB498E3A] = "m_eMaxLegalGameTension";
        (*property_map)[0x7A2AF251] = "m_bLegalizeShooting";
        (*property_map)[0x2411AAFF] = "m_lastStaticArrestReason";
        (*property_map)[0x96B0263C] = "m_bLegalizeChangeOutfit";
        (*property_map)[0x3F0AF9BA] = "m_pCollisionResource";
        (*property_map)[0x75B853E4] = "m_Sound";
        (*property_map)[0xE7EF23EC] = "m_SwitchHMOutfit";
        (*property_map)[0xDA55BE09] = "m_SwitchGeoTag";
        (*property_map)[0x7237954F] = "m_SwitchPQ";
        (*property_map)[0x99D8C4CB] = "m_eFormerEquippedItems";
        (*property_map)[0xD48D5F94] = "m_SwitchNamedNPCKeyword";
        (*property_map)[0x879BB192] = "m_fTrespassingAttentionDurationModifier";
        (*property_map)[0xF35C3BE7] = "m_SwitchSetItemInviteRequirest";
        (*property_map)[0x44D54175] = "m_aEventNames";
        (*property_map)[0x38FD994D] = "m_fWaitForApproachingTargetDistance";
        (*property_map)[0x6D0D6B2D] = "m_Inventory";
        (*property_map)[0x7B90289] = "m_ThrowHandler";
        (*property_map)[0xCD14A022] = "m_nCurrentBehaviorType";
        (*property_map)[0xE491396D] = "m_fHeadAnchorOffsetForward";
        (*property_map)[0x76D48CD9] = "m_aBehaviorEntities";
        (*property_map)[0xCC91B2FA] = "m_vGlowStashedGunsFadeNearFar";
        (*property_map)[0x61E1D665] = "m_KnowledgeData";
        (*property_map)[0xE75DF446] = "m_fMaxTriggerToRayDistance";
        (*property_map)[0xA6EABD1D] = "m_OutfitRepositoryId";
        (*property_map)[0xD7DD4C7] = "m_tSequenceEndTime";
        (*property_map)[0x65F392AE] = "m_rBodyContainerEntity";
        (*property_map)[0xC9FE770F] = "m_nBodyContainerSlot";
        (*property_map)[0x91571781] = "m_eEmotionState";
        (*property_map)[0x94B091C6] = "m_rCorpseBodybagEntity";
        (*property_map)[0xF582E590] = "m_eMarkerExportMode";
        (*property_map)[0xB08DCCDF] = "m_AccidentContext";
        (*property_map)[0x99013790] = "m_fSSRSharpenSides";
        (*property_map)[0x5FE72FB8] = "m_AnimatedActorOrderData";
        (*property_map)[0x9D5DE124] = "m_fStepsFraction";
        (*property_map)[0x130640A1] = "m_sFootstepsSwitch";
        (*property_map)[0x8ACC4719] = "m_fZBeforeEnteringStairs";
        (*property_map)[0xD86EA511] = "m_fZError";
        (*property_map)[0x38E6F942] = "m_sEndOrientation";
        (*property_map)[0x45DD1534] = "m_fNoiseIntensity";
        (*property_map)[0xEDA6F06C] = "m_eDeathBehavior";
        (*property_map)[0x95562564] = "m_RagdollPose";
        (*property_map)[0xC8F48018] = "m_SituationAvailabilityValue";
        (*property_map)[0x55C5EF76] = "m_bCheckSuspects";
        (*property_map)[0x787D1A9A] = "m_HadValidAgent";
        (*property_map)[0x7C9FD295] = "m_bNude";
        (*property_map)[0xB8DE9DB8] = "m_GlowInteractionDeselectedColor";
        (*property_map)[0x8F07838A] = "m_fSpatialBlurFade";
        (*property_map)[0xD85BB8EE] = "m_bActiveEnforcer";
        (*property_map)[0xB380BA4] = "m_bDynamicWeaponUnholstered";
        (*property_map)[0xE9004BB6] = "m_bIsPotentialEnforcer";
        (*property_map)[0x76B463F3] = "m_fAIEventInjectDuration";
        (*property_map)[0xE222E99B] = "m_bIsCrowdCharacter";
        (*property_map)[0xF267A165] = "m_bIsWoozy";
        (*property_map)[0x362853F3] = "m_fCharacterWetnessSpeed";
        (*property_map)[0xCB63E446] = "m_bBlendingOrientation";
        (*property_map)[0x9C6B900A] = "m_bMakeMainWeaponUndroppable";
        (*property_map)[0x80169F4E] = "m_fDoubleVisionTrailStrength";
        (*property_map)[0x50549F48] = "m_bRegisteredForLT";
        (*property_map)[0x5249100] = "m_bKeepOverrideSensorStateAfterBeingUnconscious";
        (*property_map)[0x66B38C8] = "m_bAlignToBaseAngles";
        (*property_map)[0xE17A9A12] = "m_bWeaponReady";
        (*property_map)[0xAEAC129A] = "m_bForceInteractionGlow";
        (*property_map)[0xF2A061] = "m_bIsOutfitRuined";
        (*property_map)[0xB9BFCD6C] = "m_TextLinePS5";
        (*property_map)[0x30966C4D] = "m_BehaviorSelectDisabled";
        (*property_map)[0x99147044] = "m_aPickerFilters";
        (*property_map)[0x2E197CC7] = "m_aProviderFilters";
        (*property_map)[0xD21E349C] = "m_nSeconds";
        (*property_map)[0x9466AA5C] = "m_vImpactPosition";
        (*property_map)[0x52A3CEF7] = "m_rStashPoints";
        (*property_map)[0x919280B1] = "m_vImpactDirection";
        (*property_map)[0x1DFB1F2A] = "m_rLeftHandVolumeRestriction";
        (*property_map)[0x72F6EC17] = "m_nImpactBone";
        (*property_map)[0xF07B07C7] = "m_pHealthControl";
        (*property_map)[0x997EC319] = "m_fYAxisWeight";
        (*property_map)[0x6273D10F] = "m_goal";
        (*property_map)[0x286CD606] = "m_fGlowCameraOutlineWidth";
        (*property_map)[0xE4DCB29E] = "m_bFullVolumeUpdate";
        (*property_map)[0x4D102F52] = "m_fStartFraction";
        (*property_map)[0xD3FB5FAB] = "m_fRowSpacing";
        (*property_map)[0x28F754FB] = "m_customData";
        (*property_map)[0xBA91CC21] = "m_rObjectEmittingProxy";
        (*property_map)[0x2FE769EF] = "otherNPC";
        (*property_map)[0x86CC52E] = "m_bAmbientOnly";
        (*property_map)[0x177E24D3] = "m_bDisablePAC";
        (*property_map)[0x2CB6BAFA] = "m_aSpreadingActors";
        (*property_map)[0x25200AFA] = "m_nModelScopeSkinSelection";
        (*property_map)[0x5B527E3A] = "m_aDeadSpreadingActors";
        (*property_map)[0xA9F45EE2] = "m_aSpreadingActorsAddedTime";
        (*property_map)[0x3D047C0] = "m_IllegalItem";
        (*property_map)[0xF1509FA2] = "m_aNewCandidates";
        (*property_map)[0x595F3567] = "m_bCollector";
        (*property_map)[0x3A7E295F] = "m_rCurrentSpreadingActor";
        (*property_map)[0x5C0B1E7C] = "Mirror";
        (*property_map)[0x67C72CC3] = "m_nCurrentActorIndex";
        (*property_map)[0x69201628] = "BlobsDatastoreRid";
        (*property_map)[0x649E4C2B] = "m_fGuardPointInfluence";
        (*property_map)[0x6BFE4D05] = "m_bPlayerSpreading";
        (*property_map)[0x6C8051E7] = "m_fMuzzleEnergy";
        (*property_map)[0x3229284F] = "m_bPlayerIsValidCandidate";
        (*property_map)[0xAFD85058] = "m_bOwnsCurrentBehavior";
        (*property_map)[0xC55E8854] = "m_bPlayerIsCandidate";
        (*property_map)[0xBBA19347] = "m_bGreetedLoitering";
        (*property_map)[0xC5FED6C5] = "m_PlayerCandidateTime";
        (*property_map)[0x9134A42A] = "m_bEnableAmbientLookat";
        (*property_map)[0x355092A0] = "m_bBlendDirectFromWalk";
        (*property_map)[0x31EC7904] = "m_bAnnouncedFrisk";
        (*property_map)[0x98C04C6C] = "m_bBlendUpperBodyAtEnd";
        (*property_map)[0xBA70BCB9] = "m_bAllowRagdollDamageReactions";
        (*property_map)[0xB3E7D960] = "m_eAnimationPose";
        (*property_map)[0x963E5E98] = "m_aCrowdPanicOnlyBoxes";
        (*property_map)[0x1386B780] = "m_bSavableNetwork";
        (*property_map)[0x4EB34CC1] = "m_fPointAimDurationMin";
        (*property_map)[0xCF4DF058] = "m_pCustomReactionsNetwork";
        (*property_map)[0x464749BF] = "m_bPreciseOrientation";
        (*property_map)[0x776F0876] = "m_bIgnoreEndCollision";
        (*property_map)[0xD455556C] = "m_formationSlots";
        (*property_map)[0x200230BC] = "m_fLensBlur";
        (*property_map)[0x319A8B1A] = "m_bForcedEnpointSet";
        (*property_map)[0xBA3756CD] = "VideoId";
        (*property_map)[0x14E9E8F7] = "m_vWaypoint";
        (*property_map)[0x35495507] = "m_vForcedEndpoint";
        (*property_map)[0xB3203B7E] = "m_rVolume";
        (*property_map)[0x586A6060] = "m_fShotPenetrationDepth";
        (*property_map)[0xB6D22D97] = "m_bShowExitWarningEnded";
        (*property_map)[0x391B5A66] = "m_fCombatThreatRadius";
        (*property_map)[0x974B4E0F] = "m_bDebugAIModifiers";
        (*property_map)[0x8103670A] = "m_bMayBeAddressedWhileSpeaking";
        (*property_map)[0x796A7971] = "m_fCombatPreferredDistanceFromThreat";
        (*property_map)[0x19B8B59F] = "m_fCombatPreferredDistanceFromThreatSecondaryRange";
        (*property_map)[0x3F4CA349] = "m_rSpline";
        (*property_map)[0xED43F12E] = "m_fCombatRadius";
        (*property_map)[0x59BA4E84] = "m_fDistanceToThreatInfluence";
        (*property_map)[0x542DD619] = "m_fDistanceToThreatInfluenceSecondaryRange";
        (*property_map)[0x780E6E61] = "m_Drama2SetupData";
        (*property_map)[0x11189886] = "m_fDistanceFromActorInfluence";
        (*property_map)[0xAE5B3F58] = "m_fCoverInfluence";
        (*property_map)[0xCE0AFB22] = "m_fCoverInfluenceSecondaryRange";
        (*property_map)[0x992AA5E6] = "m_fLineOfSightInfluence";
        (*property_map)[0xCE4F367A] = "m_SeasonOneHead3";
        (*property_map)[0xBC8C25B6] = "m_fOccupancyPenaltyInfluence";
        (*property_map)[0xCF172D8B] = "m_fHazardPenaltyInfluence";
        (*property_map)[0x9F96274C] = "m_fDistanceToThreatInfluenceAggressive";
        (*property_map)[0x6CFC05EE] = "m_GlobalLightBackColor";
        (*property_map)[0x8B689369] = "m_fHideInCoverTimerMin";
        (*property_map)[0xB10379F6] = "m_fReevaluateCoverPositionTimer";
        (*property_map)[0xECCBC5D0] = "m_fHideDueToAimedAtDuration";
        (*property_map)[0x13D077D0] = "m_fCloseCombatTriggerDistance";
        (*property_map)[0xA17D117] = "m_pHMAimReaction";
        (*property_map)[0xD6380FEA] = "m_fMaxInteractionRange";
        (*property_map)[0x1C5ADC73] = "m_fCloseCombatCooldown";
        (*property_map)[0x32DE15A0] = "m_bSkipCLP";
        (*property_map)[0x4760EA71] = "m_fAgilityAttractionClimbPenalty";
        (*property_map)[0x184A01E3] = "m_fCLPAttentionResetThreshold";
        (*property_map)[0x7D4D99FC] = "m_DialogueLines";
        (*property_map)[0xF58FEB21] = "m_aFlotationPoints";
        (*property_map)[0xC0146C94] = "fCloseupDistance";
        (*property_map)[0xBE053997] = "m_vCenter";
        (*property_map)[0x84CAB079] = "fScale";
        (*property_map)[0x63E2547F] = "bDynamicScale";
        (*property_map)[0xF2F62042] = "m_tLastWarning";
        (*property_map)[0xD41A80B8] = "fDynamicScaleBlendRange";
        (*property_map)[0x3E074B6E] = "m_rSafeZone";
        (*property_map)[0xA2F9290E] = "m_pReporter";
        (*property_map)[0x13FC0BD2] = "m_nSafeZoneAnimNode";
        (*property_map)[0x26ADCCFA] = "m_nID";
        (*property_map)[0xC97D07F0] = "m_bAllocatedForSituation";
        (*property_map)[0xD4554B7C] = "m_rCurrentScreenplay";
        (*property_map)[0x8BCA24AB] = "ridProp";
        (*property_map)[0x4043BA79] = "m_rPreferredIntermediateScreenplay";
        (*property_map)[0x1702FA3C] = "m_field";
        (*property_map)[0x124FC5EF] = "m_Start";
        (*property_map)[0x8D4492E4] = "m_plane";
        (*property_map)[0xAA9E88DD] = "m_sMasterSwitch";
        (*property_map)[0x8DF5B130] = "m_mThreshold";
        (*property_map)[0xB1B46FCF] = "m_bDifficulty_Level3";
        (*property_map)[0xA498B50A] = "m_eCinemaModeType";
        (*property_map)[0x61EA0D94] = "sFontId";
        (*property_map)[0x8AA471EC] = "m_rSoundEvent_BulletImpactHead";
        (*property_map)[0x5CD23AD9] = "sFontName";
        (*property_map)[0x81A7148B] = "m_aSpaces";
        (*property_map)[0x77E0B60C] = "m_mOffsetOrientation";
        (*property_map)[0x34F116C8] = "m_behavior";
        (*property_map)[0xC5CC1EA3] = "_dummy";
        (*property_map)[0xE7B5DD18] = "m_fPuddleNumDrops";
        (*property_map)[0xEF775D8E] = "m_bDisableEventFlagsOnStopEvents";
        (*property_map)[0x24A3D765] = "m_aOccurences";
        (*property_map)[0xD6047B0B] = "m_vTargetPosition";
        (*property_map)[0x958549C1] = "m_MinColor";
        (*property_map)[0x73E5E620] = "m_MaxColor";
        (*property_map)[0x9DB91A96] = "m_nAudioExoticType";
        (*property_map)[0xB7CEB3F4] = "m_bRandomizeRed";
        (*property_map)[0xB5EDADE0] = "m_bRandomizeGreen";
        (*property_map)[0x59ACCDEC] = "m_bRandomizeBlue";
        (*property_map)[0x8BA14F2C] = "m_curveRed";
        (*property_map)[0xCCBF6248] = "m_ReactionEntity";
        (*property_map)[0xE473FBF3] = "m_sideBoxDepthMarginFar";
        (*property_map)[0x60BAD2DD] = "m_bLinear";
        (*property_map)[0x764F3DC7] = "posCharacter";
        (*property_map)[0x53C01A] = "udirTowardsPlane";
        (*property_map)[0x97093978] = "udirAnchor";
        (*property_map)[0xBE1520FC] = "m_bSetAsActive";
        (*property_map)[0x35EAADD5] = "m_AgencyPickupId";
        (*property_map)[0xF3F2D059] = "m_bOdometerStairs";
        (*property_map)[0x1CE85462] = "m_bOdometerBodyDragging";
        (*property_map)[0xC06DBC13] = "m_sChannelId";
        (*property_map)[0x9F18FE4C] = "m_bTriggerOnSet";
        (*property_map)[0xE70EAB77] = "m_sJsonData";
        (*property_map)[0xD8D8EE67] = "m_CurveY";
        (*property_map)[0x41D1BFDD] = "m_CurveZ";
        (*property_map)[0x9449F20F] = "m_bVignetteEnabled";
        (*property_map)[0x4045EEC3] = "m_CurveRotX";
        (*property_map)[0x2A9C4511] = "m_ePlaceable";
        (*property_map)[0x3742DE55] = "m_CurveRotY";
        (*property_map)[0xAE4B8FEF] = "m_CurveRotZ";
        (*property_map)[0x78261BCD] = "m_rEnableCondtion";
        (*property_map)[0xCF39772C] = "m_bNotebookIntelEnabled";
        (*property_map)[0x214CDD3D] = "m_nMinDistance";
        (*property_map)[0xCE00CF41] = "m_BoxLengthByDistance";
        (*property_map)[0xE46E5800] = "m_bEquipSameTypeIfInInventory";
        (*property_map)[0xED41A632] = "m_nMaxDistance";
        (*property_map)[0xE5FF9F2B] = "m_bGiveItem";
        (*property_map)[0x9A06A422] = "m_bAimAtTarget";
        (*property_map)[0x8F64D131] = "m_CureActs";
        (*property_map)[0x7F3F0DC6] = "m_fSlidingEventCooldownTime";
        (*property_map)[0x8194DC10] = "m_bCloseCombatStandart";
        (*property_map)[0x46E430B7] = "bLeftThroughWarningZone";
        (*property_map)[0xFE88C869] = "bEnteredThroughEntranceZone";
        (*property_map)[0x8969E439] = "m_bTrespassing";
        (*property_map)[0x94E338D4] = "bInEntranceZone";
        (*property_map)[0xEB5B7456] = "bInGreetingZone";
        (*property_map)[0x83EEAA89] = "m_nEscalatedActorCount";
        (*property_map)[0x7F496498] = "m_nCurrentState";
        (*property_map)[0x219C890C] = "m_bInFriskExemptDisguise";
        (*property_map)[0xC057DC66] = "m_bCanShowActionPrompt";
        (*property_map)[0xC51AB2E4] = "m_TriggerOnOverlap";
        (*property_map)[0xBEF63639] = "m_bTargetInAnyZone";
        (*property_map)[0xF060B90E] = "m_bItemChecked";
        (*property_map)[0x81F93BD0] = "m_nNumberOfSwitches";
        (*property_map)[0x63772CC9] = "m_bGreeted";
        (*property_map)[0x30E4A4AD] = "m_rCollider";
        (*property_map)[0x25813B26] = "m_aeToStates";
        (*property_map)[0xE1F082CC] = "m_bGreetedUnexpected";
        (*property_map)[0xDFB45501] = "m_eCarryAnimSetType";
        (*property_map)[0xB1A1DD6A] = "m_bInFriskWarningZone";
        (*property_map)[0xBF8450C2] = "m_bDebugSituation";
        (*property_map)[0xAC77462A] = "m_aDisguisesFriskExemptDisabled";
        (*property_map)[0xC9810A6A] = "m_aDisguisesDontEscalateOnLineCrossingDisabled";
        (*property_map)[0x6B5324AC] = "m_eThirdWarning";
        (*property_map)[0xDF87861] = "m_ItemsDroppedInZone";
        (*property_map)[0x45D53F3] = "m_rBoneAttacher";
        (*property_map)[0x72A177DC] = "eCoreProvider";
        (*property_map)[0x4540D78B] = "nUserData";
        (*property_map)[0xD0453B00] = "m_bUseLimits";
        (*property_map)[0x2B8EEB2B] = "m_fRotationMultiplier";
        (*property_map)[0xDA773800] = "m_fParticleCameraFadeSizeMultiplier";
        (*property_map)[0xC943ABB8] = "m_GlowContractTargetOutlineColor";
        (*property_map)[0x4DA77ECB] = "m_fAngularMin";
        (*property_map)[0x71AA4192] = "m_fAngularMax";
        (*property_map)[0x88D893F0] = "m_fProjectionDistanceMax";
        (*property_map)[0xE3FD5CFB] = "m_fStopDistance";
        (*property_map)[0x3A061B4E] = "m_vBobbingAngleY";
        (*property_map)[0x7FCAB167] = "m_bForceStand";
        (*property_map)[0xFF7DB8A3] = "m_bLookAtTargetMustBeInSight";
        (*property_map)[0xEC3FA6AB] = "m_bUseKnownPosition";
        (*property_map)[0x947FF047] = "m_bEndStanding";
        (*property_map)[0x284C5F98] = "m_bFastTransition";
        (*property_map)[0x7D0EB84D] = "m_tLastGuardSearch";
        (*property_map)[0x1AB5FDC4] = "m_rMoveToTransform";
        (*property_map)[0x90412A17] = "m_rOrientToTransform";
        (*property_map)[0xE5415823] = "m_MovementType";
        (*property_map)[0x4D4C0708] = "m_rShootTarget";
        (*property_map)[0x1AB201F5] = "m_bUpperBodyAct";
        (*property_map)[0x412AF116] = "m_StartingConditions";
        (*property_map)[0x91DC95EA] = "knownEntity";
        (*property_map)[0x59FB266C] = "m_eApproachAlignment";
        (*property_map)[0x53EBABB0] = "m_rConvertionHelperTarget";
        (*property_map)[0xD0FA2116] = "m_bFixXAxis";
        (*property_map)[0x7CBF2A5E] = "m_bUseConversationHelper";
        (*property_map)[0xBAF08CE9] = "m_rConversationHelperCondition";
        (*property_map)[0x5507CDE7] = "m_vGlowGunsFadeNearFar";
        (*property_map)[0xF3B9DA1A] = "m_bConversationHelperFastTrans";
        (*property_map)[0x41AE39CD] = "m_bRemoveWhenDone";
        (*property_map)[0x1BE98653] = "m_bRequestItemAndVarientReset";
        (*property_map)[0x3D233FE8] = "m_sRigidbodyImpactsSoliditySwitchGroup";
        (*property_map)[0x2C524352] = "m_bDisableLookAt";
        (*property_map)[0x26356953] = "m_bResetAnimSetAfterAct";
        (*property_map)[0xBEC748A2] = "m_bSendInterruptWhileStopping";
        (*property_map)[0x14B2C035] = "m_eActorBumpType";
        (*property_map)[0xB3895260] = "m_nBranchValue";
        (*property_map)[0x2E5DD3DE] = "m_pShellImpactEvent";
        (*property_map)[0x1FD10B85] = "newInfo";
        (*property_map)[0x8C281F1E] = "m_bClearHistory";
        (*property_map)[0x8ADB55F1] = "m_StoppingDistance";
        (*property_map)[0x97BA2F62] = "m_LookAroundAnimation";
        (*property_map)[0xBB9919B9] = "m_vBodyVelocity";
        (*property_map)[0xF3F7C10F] = "m_rApplyTransformToSpatial";
        (*property_map)[0x608A5828] = "m_pRepositoryId";
        (*property_map)[0xCBFE028D] = "m_fRunningNumDrops";
        (*property_map)[0xCDE76755] = "m_sUniqueIdentifier";
        (*property_map)[0x390FF56D] = "m_sType";
        (*property_map)[0xD0458B9F] = "m_sIcon";
        (*property_map)[0xAF4DA18C] = "m_nFollowerCount";
        (*property_map)[0x67B4AA60] = "m_aButtons";
        (*property_map)[0xFE7EB6AF] = "m_aHelpingActors";
        (*property_map)[0x4957CE89] = "m_eRightHandIK";
        (*property_map)[0x4612011C] = "m_rProxyFor";
        (*property_map)[0xA6DD2400] = "m_eLeftHandIK";
        (*property_map)[0xFB0C3BE3] = "m_ePelvisIK";
        (*property_map)[0x2A1EC9B6] = "m_eSpineIK";
        (*property_map)[0xF9BF03BC] = "m_pSplineGizmo";
        (*property_map)[0xAA24E194] = "m_eLeftHandCollisions";
        (*property_map)[0x542FE18C] = "m_eRightHandCollisions";
        (*property_map)[0x59121BBA] = "m_GlowCivilianUnconsciousOutlineColor";
        (*property_map)[0xAA73F1F8] = "m_eLeftHandPosesAllowed";
        (*property_map)[0x286897AA] = "m_eRightHandPosesAllowed";
        (*property_map)[0xDBC929A9] = "m_sBoneName";
        (*property_map)[0xA693508B] = "m_rRightHandVolumeRestriction";
        (*property_map)[0xF75A784E] = "m_bLeftHandRotationType";
        (*property_map)[0x64B58E32] = "m_bRightHandEnablePositionOffset";
        (*property_map)[0xAEC86362] = "m_vGlowEnemiesFadeNearFar";
        (*property_map)[0x6D53D192] = "m_sLeftHandPositionOffset";
        (*property_map)[0x8EB20AE1] = "m_eLeftHandIKAnimationDriven";
        (*property_map)[0xCBEA88CD] = "m_eRightHandIKAnimationDriven";
        (*property_map)[0xA6C086AA] = "m_globalTemplate";
        (*property_map)[0xB18260D1] = "m_velocity";
        (*property_map)[0x29C0D1A6] = "m_timeToLiveSec";
        (*property_map)[0x38C9C3E8] = "m_timeToDie";
        (*property_map)[0xB92218F5] = "m_aDisguisesDontEscalateOnLineCrossing";
        (*property_map)[0x44F3378E] = "m_vFixedXAxis";
        (*property_map)[0x122DF908] = "m_lastPosition";
        (*property_map)[0xD84E76CC] = "m_bForwardOnSet";
        (*property_map)[0x97A178BB] = "m_fShadowedInscatterIntensity";
        (*property_map)[0xD271669A] = "m_eForwardRule";
        (*property_map)[0xBEB25F65] = "m_aPickupScreenplays";
        (*property_map)[0xB38575E3] = "m_rDangerousZone";
        (*property_map)[0xA989777B] = "m_aFilterProcedures";
        (*property_map)[0x885DE9BD] = "hidden";
        (*property_map)[0xFD8C5053] = "m_bBlockIfHUDChannelUnavailable";
        (*property_map)[0x1F62E3FE] = "m_pLoadoutEntity";
        (*property_map)[0xE27FB585] = "m_bPlacedInMainMenu";
        (*property_map)[0x4C96353D] = "m_nWeaponIndex";
        (*property_map)[0xD0CD7B94] = "m_bDisableAIPerceptionOnPlace";
        (*property_map)[0x345B05D6] = "m_pSafeZoneNetwork";
        (*property_map)[0xD4F2507B] = "m_pActionFilter";
        (*property_map)[0x8584D4DE] = "m_pBlockTrespassAttentionCondition";
        (*property_map)[0x96F2E070] = "m_fAcceptance";
        (*property_map)[0x41004691] = "m_bSound";
        (*property_map)[0x56E813BF] = "m_pHide";
        (*property_map)[0x98AF8BE2] = "m_bHideItems";
        (*property_map)[0xB49A1970] = "m_bDropAndPickupLeftHandItem";
        (*property_map)[0x266F1762] = "m_fMinSize";
        (*property_map)[0xDCBF6350] = "m_bAlignYawAngle";
        (*property_map)[0x697F12BE] = "m_AlertableActorOnScreenCooldown";
        (*property_map)[0x7B7C9152] = "m_bAlignPitchAngle";
        (*property_map)[0xD969EA15] = "m_bSetPositionOnExit";
        (*property_map)[0x14D99A21] = "m_fTwistDriveDamping";
        (*property_map)[0x943173B] = "m_bIsOnline";
        (*property_map)[0xEF5FB61A] = "m_fClothBlendTime";
        (*property_map)[0xF0A634DA] = "m_aInputListeners";
        (*property_map)[0x6CCDDFB5] = "m_jumpPoints";
        (*property_map)[0x584A77B9] = "m_eCurrentClosetType";
        (*property_map)[0x3947288C] = "m_pInvestigateDisguiseGroup";
        (*property_map)[0xDFFDA193] = "m_pRecoverUnconsciousGroup";
        (*property_map)[0x96E256AB] = "m_pObserversGroup";
        (*property_map)[0xC75C057C] = "m_vBounds";
        (*property_map)[0xF67D6ABF] = "m_MathLerpsData_SVector3";
        (*property_map)[0xAA42665C] = "m_bVictimPacified";
        (*property_map)[0x2F1D671C] = "m_rVRMovementConfigDefault";
        (*property_map)[0x121091BE] = "m_nMinCrowdLOSRangeMod";
        (*property_map)[0x2AB285BC] = "itemStashes";
        (*property_map)[0x7FBA18A8] = "m_HighVisibilityBits";
        (*property_map)[0xFC1C6336] = "m_aContainerItems";
        (*property_map)[0x8D82BB95] = "nAndUnhandled";
        (*property_map)[0xA3BF5386] = "m_sSceneResource";
        (*property_map)[0xC0329722] = "m_aAdditionalBrickResources";
        (*property_map)[0x2EAC5BE7] = "m_HUDTmpl";
        (*property_map)[0xB2B6A61F] = "m_timerEndTime";
        (*property_map)[0xFB3EE4A6] = "m_bTimerHasJustStarted";
        (*property_map)[0x6BDCA776] = "m_bIsActiveTimerVisible";
        (*property_map)[0xE1309F61] = "m_fASSAOPower";
        (*property_map)[0x510C1A8A] = "m_nAudioTailType";
        (*property_map)[0x9C587221] = "m_nObjectivesCounterCount";
        (*property_map)[0xA97C501D] = "m_dObjectivesCounterExtraData";
        (*property_map)[0x892EAE7A] = "m_bNoStanddown";
        (*property_map)[0xB1723321] = "m_tLastStandDownOrder";
        (*property_map)[0x61DE65BF] = "m_nObjectivesCounterType";
        (*property_map)[0xA26B8BCD] = "m_CrowdAttentionRangeCurve";
        (*property_map)[0xF8EE65BC] = "m_rEscortee";
        (*property_map)[0x773B2160] = "m_rPromptDescriptionTextPassiveResource";
        (*property_map)[0x54574E64] = "m_sRawLabel";
        (*property_map)[0x711794C1] = "m_sSweetLabel";
        (*property_map)[0xA99B247E] = "bHolster";
        (*property_map)[0xA7CC65CF] = "m_bSetHighPriorityTextures";
        (*property_map)[0x3970DD49] = "m_nMuzzleBrakeSelection";
        (*property_map)[0x2C92DED9] = "m_nMuzzleBrakeSkinSelection";
        (*property_map)[0xC0986D91] = "m_nScopeSkinSelection";
        (*property_map)[0xBF33123E] = "m_aSkinSelections";
        (*property_map)[0x6D7F5D3A] = "m_fDeadZone";
        (*property_map)[0x7D333AEF] = "m_rTextLineResource";
        (*property_map)[0x6C09FF9D] = "r";
        (*property_map)[0x1D41B76] = "g";
        (*property_map)[0x3E1D98DF] = "m_spawnPoints";
        (*property_map)[0x9EC4A25C] = "m_bEnable";
        (*property_map)[0xF4E50D62] = "m_rFromItem";
        (*property_map)[0x1B95EE65] = "m_fGlowSetPieceOutlineWidth";
        (*property_map)[0x33AB4C1D] = "m_rOverrideKey";
        (*property_map)[0xEF984BF8] = "m_pCompassView";
        (*property_map)[0xB10FAF82] = "m_iAmountOfItemsPerPosition";
        (*property_map)[0xCAAAAD9F] = "m_FarIconControl";
        (*property_map)[0x892E7B75] = "m_aPickupSpawners";
        (*property_map)[0xB6F47BC0] = "m_rLeftHandColliders";
        (*property_map)[0x566AC4A9] = "m_UsePlaneMovement";
        (*property_map)[0x404F102A] = "SlotIndex";
        (*property_map)[0x43C7C693] = "OperationId";
        (*property_map)[0x9FAB1AE2] = "StimulusData";
        (*property_map)[0x3E834562] = "m_pWaypointUnlocked";
        (*property_map)[0x576721EF] = "m_pSuitcase";
        (*property_map)[0x3B70FADD] = "m_pWaypointUnlockedData";
        (*property_map)[0x6FEFB9F4] = "m_pWaypointLocked";
        (*property_map)[0xCAA18BD8] = "m_pWaypointLockedData";
        (*property_map)[0x84E3D642] = "m_pWaypointLockedArrow";
        (*property_map)[0x1251AF83] = "m_fUnlockedBottomAlpha";
        (*property_map)[0x3D0EF569] = "m_fRotateAnimationDuration";
        (*property_map)[0xFEC5B1C7] = "m_fScale3D";
        (*property_map)[0xA7B51273] = "m_nShowAmount";
        (*property_map)[0x57A0B429] = "m_tCooldown";
        (*property_map)[0xBBEC57DD] = "m_bSpecificMenu";
        (*property_map)[0xDE5735E3] = "m_pBankResource";
        (*property_map)[0xC80D378F] = "m_pMapContainer";
        (*property_map)[0xCD67FB72] = "range_from_origin";
        (*property_map)[0x4B103D9F] = "m_eItemWeapon";
        (*property_map)[0xD5C51BD2] = "m_fMasterVolume";
        (*property_map)[0xEC687CB5] = "m_bCheckRoomsCombined";
        (*property_map)[0xF45A9321] = "m_tFriskGroupEnd";
        (*property_map)[0xAF0A4C31] = "m_nAudioWeaponFamily";
        (*property_map)[0x1327D9F3] = "m_bCheckBlockedCells";
        (*property_map)[0x1F4CEDC6] = "m_bUseCameraRoomsAsReference";
        (*property_map)[0x7655925C] = "m_bCheckNonBlockedNeightbors";
        (*property_map)[0x72E47D93] = "m_pTimerDataInterface";
        (*property_map)[0x4E838451] = "m_Hour";
        (*property_map)[0x4D072826] = "m_sendHeroPosition";
        (*property_map)[0xFB2046A1] = "m_Second";
        (*property_map)[0x235D358C] = "m_Milisecond";
        (*property_map)[0xDE1601B5] = "m_aGeneratorData";
        (*property_map)[0x74D521C1] = "m_bSortParticles";
        (*property_map)[0xCBFFE352] = "m_bIgnoreRotation";
        (*property_map)[0x742EC1C3] = "m_fAtlasUpsampleFactor";
        (*property_map)[0x9601A28B] = "m_fScaleFactor";
        (*property_map)[0xB58620E5] = "m_fAOBending";
        (*property_map)[0xB9B84326] = "sKillMethod";
        (*property_map)[0x4D8E58C1] = "nVersion";
        (*property_map)[0x2B74C4CE] = "nCrc32";
        (*property_map)[0x96966A79] = "nSize";
        (*property_map)[0x2C4B0382] = "nTimeStamp";
        (*property_map)[0x301B3A2B] = "m_rCurrentStage";
        (*property_map)[0xF2D0B7CB] = "m_bTracked";
        (*property_map)[0xD208A723] = "m_pCrosshairControl";
        (*property_map)[0x5712BD1C] = "m_bUnlocked";
        (*property_map)[0x8FF4D657] = "m_bIsLastTriggered";
        (*property_map)[0x5EC716BA] = "m_nAIPoolSizeFemale";
        (*property_map)[0xB37E3076] = "m_bIsHandlerAudioPlaying";
        (*property_map)[0x4BBE4C32] = "m_fCapsuleRadius";
        (*property_map)[0x170D80B6] = "m_vCapsuleLength";
        (*property_map)[0x50677B3B] = "m_ThrowType";
        (*property_map)[0xCAFB7FD1] = "m_bPausesPrecisionModeDrain";
        (*property_map)[0x20BC609E] = "m_fCivilianPropagationGracePeriod";
        (*property_map)[0x26824D39] = "m_fIncreaseDrainPercentage";
        (*property_map)[0x398D1522] = "m_pAnimation";
        (*property_map)[0xFBEC230B] = "m_eMainAnimSetType";
        (*property_map)[0x2E066104] = "Videos";
        (*property_map)[0xD12B4F9F] = "m_bIsRayIntersecting";
        (*property_map)[0xEC9087F5] = "m_bIsCurrentlyEnabled";
        (*property_map)[0xB99D654A] = "m_eVsGenericEvent";
        (*property_map)[0x4C4BAAEA] = "m_pDestructibleMaterialResource";
        (*property_map)[0xCFB258A3] = "m_nCellSizeY";
        (*property_map)[0x4DC11B3] = "m_pDestructibleMaterial";
        (*property_map)[0xEB613697] = "m_nInitialFractureLevel";
        (*property_map)[0x7FC3EE32] = "m_fDebug_StateModifierRange";
        (*property_map)[0x4983BBFB] = "m_fMinimumWakeTime";
        (*property_map)[0xE2BBF0D1] = "m_GlowGunsOutlineColor";
        (*property_map)[0x3467D631] = "m_eRestitutionCombineMode";
        (*property_map)[0xA7007DF6] = "m_fMaxAngularVelocity";
        (*property_map)[0xF4B3E3F9] = "m_sRagdollImpactsSwitchGroup";
        (*property_map)[0xF9730659] = "m_pStopEvent";
        (*property_map)[0xDD077CD7] = "m_bRespondOnHostOnly";
        (*property_map)[0x9508A385] = "m_bAffectedByExplosionEffectRadius";
        (*property_map)[0x7B3369BF] = "m_eCollisionLayer";
        (*property_map)[0x61D37233] = "m_fMaximumDisplayTime";
        (*property_map)[0x4A0620E5] = "m_bIgnoreCharacterCollisions";
        (*property_map)[0xAEDF786] = "m_pSpatialEntity";
        (*property_map)[0x3F1C1704] = "m_eCCDUsage";
        (*property_map)[0xE5EF5704] = "m_bAllowSelfCollisions";
        (*property_map)[0x3D127C28] = "m_aDestroyedEffect";
        (*property_map)[0xBD8EC09F] = "m_aCollidedEffect";
        (*property_map)[0xBD796776] = "m_fCollidedEffect_ForceThreshold";
        (*property_map)[0x28586032] = "m_bAffectVisibility";
        (*property_map)[0xD031F1D0] = "m_pBackgroundDataProvider";
        (*property_map)[0x56AB99DC] = "m_ExtendedEventConsumers";
        (*property_map)[0x1A706862] = "m_bTutorialNotebookPrompt";
        (*property_map)[0x98954FA8] = "m_TargetEntity";
        (*property_map)[0x73DE978E] = "m_degAnchorLookResetAperture";
        (*property_map)[0x375D8F5E] = "m_ChestAvoidanceGhostActivationAngle";
        (*property_map)[0x31A13BFC] = "m_bDiscardInHighTraffic";
        (*property_map)[0xEEE02081] = "m_degpdegAnchorMoveResetSpeedAngular";
        (*property_map)[0xEB4187C] = "m_degLateralShift";
        (*property_map)[0x8AACA565] = "m_bSynchroniseSpeech";
        (*property_map)[0xA61E2749] = "m_degPlaneAlignmentClamp";
        (*property_map)[0x2D086D8D] = "m_recoverUnconsciousGroup";
        (*property_map)[0x8D93B869] = "tension";
        (*property_map)[0x143B1107] = "m_UsageRepresentation";
        (*property_map)[0x181C287B] = "m_FreeIndices";
        (*property_map)[0xC4FA888C] = "m_rDangerousArea";
        (*property_map)[0x541A2725] = "m_vDestinationPoint";
        (*property_map)[0xB3B4F99D] = "m_LOSShotgunFirePatterns";
        (*property_map)[0xBBD93782] = "m_nBoneId";
        (*property_map)[0x6195BA6F] = "m_nSituation";
        (*property_map)[0xB82C5BBE] = "m_objectEntity";
        (*property_map)[0xA42C6007] = "m_bSetOnValueChange";
        (*property_map)[0x2B279502] = "m_nGroupID";
        (*property_map)[0xEFB2E4A0] = "m_eSpawnAxis";
        (*property_map)[0x757D8BCA] = "m_bUseEmitterMoveSpeed";
        (*property_map)[0xF9F4ABF9] = "m_GlowVSOpponentColor";
        (*property_map)[0x3416148D] = "m_fRandomPitch";
        (*property_map)[0x3CEEDF24] = "m_bRandomAdjustEjectDir";
        (*property_map)[0x6D2A0A2D] = "m_bRandomTorque";
        (*property_map)[0xA3AE4F78] = "Primitive";
        (*property_map)[0xF339F671] = "m_bIsReacting";
        (*property_map)[0x259CF72D] = "m_fMaxSize";
        (*property_map)[0xB4797CE0] = "m_sound";
        (*property_map)[0xA6D58A0C] = "m_aExclusionBoxes";
        (*property_map)[0x7268DF6E] = "m_StoredActor";
        (*property_map)[0xA0EDA0D] = "m_rUseStashPoints";
        (*property_map)[0xEF19CC69] = "m_aGeomEntities";
        (*property_map)[0xCF6A4C2E] = "m_widget";
        (*property_map)[0x8C4B1415] = "m_PopUp_dtOpaque";
        (*property_map)[0xC18C5C15] = "m_PopUp_dtFadeOut";
        (*property_map)[0x6C539746] = "m_CollisionControllerAspectsData";
        (*property_map)[0x677E5CE7] = "m_funcnameInvokedWhenTakeClothesMovementBegins";
        (*property_map)[0xB1C4D753] = "m12";
        (*property_map)[0xCEC2EF42] = "m_blendNode";
        (*property_map)[0x58A77266] = "m14";
        (*property_map)[0x884789CB] = "m_bDoubleVisionEnabled";
        (*property_map)[0x9AE98490] = "m22";
        (*property_map)[0xEDEEB406] = "m23";
        (*property_map)[0xD297BD9F] = "m_rAttachmentTarget";
        (*property_map)[0x738A21A5] = "m24";
        (*property_map)[0xEA53A605] = "COLLISION_VOLUME_HITMAN_OFF";
        (*property_map)[0xE11FE8B] = "RuntimeLocalizedVideoRid";
        (*property_map)[0x1E771C43] = "m_ItemHandsUse";
        (*property_map)[0x1AFBE46B] = "m31";
        (*property_map)[0xF4F58547] = "m33";
        (*property_map)[0x6A9110E4] = "m34";
        (*property_map)[0x13FAEECD] = "m_fSampleDistance";
        (*property_map)[0xCCB32316] = "m42";
        (*property_map)[0x46152736] = "m_fHealthPercentToFall";
        (*property_map)[0x8CC104F5] = "m_rYAxis";
        (*property_map)[0xBBB41380] = "m43";
        (*property_map)[0x25D08623] = "m44";
        (*property_map)[0x229157D2] = "m_middleBoxDepthMarginNear";
        (*property_map)[0xE9AFE99F] = "m_nTimeStamp";
        (*property_map)[0x8097A509] = "m_nDbgConcept";
        (*property_map)[0x6B475D32] = "m_bDepthCull";
        (*property_map)[0x5F38A006] = "m_vGlowVSOpponentFadeNearFar";
        (*property_map)[0xD05FFFC] = "m_nDbgParentConcept";
        (*property_map)[0x55D757EC] = "aLoadout";
        (*property_map)[0x6B193C4B] = "m_loadoutData";
        (*property_map)[0x94AA960] = "m_vGlowObjectivesFadeNearFar";
        (*property_map)[0x9910EA83] = "m_loadoutChanged";
        (*property_map)[0x25103EE9] = "m_bIsAvailableSavesCacheInitialized";
        (*property_map)[0xB0664D21] = "m_cachedAvailableSavesChanged";
        (*property_map)[0xBD7E05F9] = "selectedAgencyPickup";
        (*property_map)[0xD8C9DEC3] = "m_enabledEntranceUnlockable";
        (*property_map)[0x9C07A155] = "m_ActorProxiesData";
        (*property_map)[0x5436E30A] = "m_persistentContext";
        (*property_map)[0xB87E4DBD] = "m_loadingScreenData";
        (*property_map)[0x8177A1E0] = "m_fRangeClose";
        (*property_map)[0xE2BD258F] = "m_bNoFarewellDialog";
        (*property_map)[0xBB1ABD9B] = "m_rBodyPartSelector";
        (*property_map)[0xC9979CEE] = "m_rMuzzleExtensionCollection";
        (*property_map)[0xB4A00F18] = "m_rScopeCollection";
        (*property_map)[0x37E7AE60] = "m_fTwistDriveStrength";
        (*property_map)[0x58AC722A] = "m_rScopeFPSCollection";
        (*property_map)[0x2A7094D5] = "m_rFallbackMuzzleEffect";
        (*property_map)[0x91B86E40] = "m_nRadius";
        (*property_map)[0xC0A1BE8E] = "iInteraction3";
        (*property_map)[0xA7FF1863] = "m_vOffset";
        (*property_map)[0xDC82C1DA] = "m_fRandomAttenuation";
        (*property_map)[0x8493029A] = "m_eHUDElement1";
        (*property_map)[0x534871D] = "m_eChannelGroup";
        (*property_map)[0x8D152EEA] = "m_PenaltyRangeDefault";
        (*property_map)[0xFA4018A4] = "HERO_DYN_BODY";
        (*property_map)[0x4FD4BFCA] = "m_pWavebank";
        (*property_map)[0xD7CAC6E5] = "m_rControlledSpatial";
        (*property_map)[0xB877F681] = "m_fRayleighDepthScale";
        (*property_map)[0x79931E3E] = "m_vFixedYAxis";
        (*property_map)[0x8F76139A] = "m_bSpawnKinematic";
        (*property_map)[0xAA3A7276] = "m_bFixZAxis";
        (*property_map)[0x3E3364EE] = "m_vFixedZAxis";
        (*property_map)[0x9A1B9AC3] = "m_sClassName";
        (*property_map)[0x8BDC00D] = "m_pParentContainerOverride";
        (*property_map)[0x8515CD2B] = "m_aReFriskZones";
        (*property_map)[0x4F232947] = "m_sDataPath";
        (*property_map)[0xE158F9B4] = "m_rSpeakController";
        (*property_map)[0x3EA7EB24] = "m_vLayoutSize";
        (*property_map)[0x132EF581] = "m_bIsContainer";
        (*property_map)[0x1DBDE323] = "m_bIsVisibleInExternalBuild";
        (*property_map)[0x3F0C1747] = "m_tEscortStateSilenceTime";
        (*property_map)[0x585EB2E3] = "m_bMouseEnabled";
        (*property_map)[0x14043A1C] = "m_bAutoAttachChildren";
        (*property_map)[0xDBEF671F] = "m_hintManager";
        (*property_map)[0x897CD31A] = "m_bCancelHintsWithMatchingContext";
        (*property_map)[0x8D1ECC04] = "m_fTimeOffset";
        (*property_map)[0xD32C1DF] = "m_fTransDriveDamping";
        (*property_map)[0x4ABA4310] = "m_titleDev";
        (*property_map)[0x64DB43A5] = "m_titleResource";
        (*property_map)[0x1FF0E722] = "m_dtAnimationLength";
        (*property_map)[0x2A9E83EE] = "m_bReceiverActivated";
        (*property_map)[0x16F43B43] = "m_nGridCellNavMeshShrink";
        (*property_map)[0x6F0EEEC9] = "m_aCrowdExclusionBoxes";
        (*property_map)[0x2C43D25C] = "m_aCrowdExclusionVolumeShapes";
        (*property_map)[0x7FADE961] = "m_aCrowdInclusionVolumeShapes";
        (*property_map)[0xE964C281] = "m_aCrowdPanicOnlyVolumeShapes";
        (*property_map)[0x69806A26] = "m_fMaximumSpeedMultiplier";
        (*property_map)[0x934C49D9] = "m_aFlowBoxes";
        (*property_map)[0x7CE30D70] = "m_aFlowBoxesTwoWay";
        (*property_map)[0xDA7DBCA2] = "m_aSplineFlows";
        (*property_map)[0x4B461E1F] = "m_aCrowdTeleports";
        (*property_map)[0x5C6B6336] = "m_nGridGeneratorOffset";
        (*property_map)[0xEC230B14] = "m_pCrowdMapResource";
        (*property_map)[0xFEF9A4D8] = "m_fBoneSphereSize";
        (*property_map)[0xCB4CD87E] = "m_sCrowdMapIdentifier";
        (*property_map)[0x21E38648] = "m_vFogTurbulenceVelocity";
        (*property_map)[0x8D067249] = "m_nRandSeed";
        (*property_map)[0x9B907B03] = "m_nUpdateFixedFramerate";
        (*property_map)[0x748FCD3E] = "m_nMaxLod";
        (*property_map)[0x199D6315] = "m_bAutoParentToCurrentCamera";
        (*property_map)[0x93FEE08] = "m_bUseCrowdCameraProfile";
        (*property_map)[0x9692C4EC] = "m_DiscoveredVIPOrContractTargetBodies";
        (*property_map)[0xE9EB1666] = "m_bAllowAICrowdShooting";
        (*property_map)[0x66AA767C] = "m_bAllowRun";
        (*property_map)[0x3DD97F7E] = "m_nSeparationMaxLOD";
        (*property_map)[0xF0FE32E4] = "m_nShadowMapMaxLOD";
        (*property_map)[0x32C0FF40] = "m_nPanicedActorRemoveFactor";
        (*property_map)[0x144C76D] = "m_nPanicedActorExitFactor";
        (*property_map)[0x54307F25] = "m_nGoProneIfStuckTimeout";
        (*property_map)[0x7ED211BB] = "m_nBodyObstacleTimeout";
        (*property_map)[0x67C071B8] = "m_bApplyHeadAnchorOffset";
        (*property_map)[0x3DD76C4A] = "m_nReusableCores";
        (*property_map)[0x9186F8B] = "m_pAIEntityFactory";
        (*property_map)[0xB8B1CF22] = "m_pAIEntityFactoryFemale";
        (*property_map)[0xC09A84FA] = "m_nRagdollPoolSize";
        (*property_map)[0x7D3262B5] = "m_fCastShadowZoomFactor";
        (*property_map)[0x5F2018F5] = "m_nCastShadowCSMMask";
        (*property_map)[0x87DC2A3D] = "m_vPuddleDropMinMax";
        (*property_map)[0x449061F9] = "m_aManuallyPlacedActors";
        (*property_map)[0xD35C7D3A] = "m_nHighlightSegmentIndex";
        (*property_map)[0x90A5CF7C] = "m_eModifierType";
        (*property_map)[0xC89C6DC2] = "m_pActControlledFormationMoveTemplate";
        (*property_map)[0xB4695672] = "m_rAnimationNetwork";
        (*property_map)[0xB00C2768] = "m_bTrigger";
        (*property_map)[0xF3389DCB] = "m_fCornerAngleThreshold";
        (*property_map)[0x27E90462] = "m_bHMInGuardDisguise";
        (*property_map)[0x7DD463BC] = "m_fCornerElevatingDistanceModify";
        (*property_map)[0x98BA58D0] = "m_vGlowInteractionDeselectedFadeNearFar";
        (*property_map)[0x57DD7DA9] = "m_fElevationThreshold";
        (*property_map)[0x3E14968C] = "m_bFilterOutInsignificantNodes";
        (*property_map)[0x35FA9B6] = "m_bPostMergeSegments";
        (*property_map)[0xE29DCADD] = "m_nRandomizeOnNthFrame";
        (*property_map)[0xC4DCFC78] = "m_bApplyG2PostProcess";
        (*property_map)[0x8AC60E80] = "m_bDrawRailPath";
        (*property_map)[0x7C13B877] = "m_fInvestigateCautiousPropagationProximityMaxDistance";
        (*property_map)[0x32C81A20] = "m_bDrawModifiedPath";
        (*property_map)[0x63EDB138] = "m_bAllowEmptyContractId";
        (*property_map)[0xFAC282AD] = "m_bDrawAreas";
        (*property_map)[0xF3E2C05C] = "m_fRecoilRecoveryFraction";
        (*property_map)[0xD892E77C] = "m_bTraceLines";
        (*property_map)[0xEAA7D761] = "m_bTestCirlceFitOnStraight";
        (*property_map)[0x96DC56DE] = "m_fAgilityAttentionDistanceModifier";
        (*property_map)[0xC874AF6F] = "m_selection";
        (*property_map)[0xE3187BDD] = "m_TextLineXB1";
        (*property_map)[0xA988B9C5] = "m_bContinuousValueUpdate";
        (*property_map)[0x7117D267] = "m_sTextID";
        (*property_map)[0xFDA53E80] = "m_bVisibility";
        (*property_map)[0xA08A8F48] = "m_bAutoClientRooms";
        (*property_map)[0x8C9C8713] = "m_bDisableCulling";
        (*property_map)[0xB9777644] = "m_rPowerConditon";
        (*property_map)[0x6394389F] = "m_RoomLeft";
        (*property_map)[0x9D47C9C2] = "m_fLeftFadeLength";
        (*property_map)[0xAC288C4B] = "m_fLeftFadeStart";
        (*property_map)[0x862C9251] = "m_fRightFadeStart";
        (*property_map)[0xF6CEE4CE] = "m_fClipDistance";
        (*property_map)[0xDF496D14] = "m_pHelperClosed";
        (*property_map)[0xBFA8EE9C] = "m_PierceOccluder";
        (*property_map)[0x4BDE5A51] = "m_aFinishedStages";
        (*property_map)[0x6CE494F4] = "m_vConnectorOffset";
        (*property_map)[0xB1E4D87F] = "m_bSignaling";
        (*property_map)[0x61F3D2A5] = "m_fRaycastDistance";
        (*property_map)[0x817A5A29] = "m_MathLerpsData_SVector2";
        (*property_map)[0xEBD22B80] = "m_fRaycastRadius";
        (*property_map)[0x2F895A1D] = "m_fHorizontalSmoothTime";
        (*property_map)[0x7FEA6749] = "m_fRotationSmoothTime";
        (*property_map)[0x90C41D63] = "scaleContextType";
        (*property_map)[0xE3278307] = "m_fEntityMass";
        (*property_map)[0xF45CC3BB] = "m_fAngularVelocityDamping";
        (*property_map)[0xEDFF1EE3] = "m_fTargetPenetration";
        (*property_map)[0xB976DB5B] = "m_pCustomDeathTemplate";
        (*property_map)[0xB3D04852] = "m_fMaxLinearVelocity";
        (*property_map)[0xD378FD87] = "m_fMaxYawDeg";
        (*property_map)[0x33D92F95] = "m_fMaxRollDeg";
        (*property_map)[0xFA894F67] = "m_fWaterDensity";
        (*property_map)[0x7A23B7D1] = "m_fSunIntensityScale";
        (*property_map)[0x2350F1F7] = "m_rCharacter";
        (*property_map)[0x9D849DD5] = "m_OutfitRepositoryID";
        (*property_map)[0xDBA267BD] = "m_aWeaponStatusControls";
        (*property_map)[0x8B05BE55] = "m_postfilterGraphNode";
        (*property_map)[0x9F39E01F] = "m_pCostumeFeatures";
        (*property_map)[0xC20CD71B] = "m_aEnforcedOutfits";
        (*property_map)[0x115FF8D0] = "m_sActorName";
        (*property_map)[0xD2BE5733] = "m_pCompiledBehaviorTree";
        (*property_map)[0x55008831] = "m_bWeaponUnholstered";
        (*property_map)[0x11A254E0] = "m_nGrenadeIndex";
        (*property_map)[0xD2B51B6B] = "m_tRecoverUnconsciousGroupEnd";
        (*property_map)[0xD48EC920] = "m_AgentVisionConfiguration";
        (*property_map)[0xDB2E7CD3] = "weaponStatus";
        (*property_map)[0xD2C0BF7F] = "itemLeftHandStatus";
        (*property_map)[0x4EB985AC] = "itemOnBackStatus";
        (*property_map)[0x2A82F3E8] = "m_eTestType";
        (*property_map)[0x59040240] = "bShowHolstered";
        (*property_map)[0xF96B1D77] = "nDisplayMode";
        (*property_map)[0xFEB7C261] = "m_rChannelToken";
        (*property_map)[0x450F68BB] = "m_pHeadlineTextListResource";
        (*property_map)[0x18AFF40C] = "m_bConversationHelperPaused";
        (*property_map)[0xB4897FC5] = "m_vExitPosition";
        (*property_map)[0x7E9E046F] = "m_tWarningTime";
        (*property_map)[0xCACA3EB4] = "m_bHardWarning";
        (*property_map)[0x495775F3] = "m_tWaitHitmanToComplyWarningTime";
        (*property_map)[0x9BB2E9F0] = "m_pDefaultImageRID";
        (*property_map)[0xF32CEA57] = "m_tWaitHitmanToComplyStateTime";
        (*property_map)[0xE63940D1] = "m_tLastInvestigationEnded";
        (*property_map)[0x8E07E3D2] = "m_tLastLookBack";
        (*property_map)[0x3E8A15CD] = "m_bReachedExitPosition";
        (*property_map)[0x8689CD04] = "m_rDialogSwitchOutfitActor";
        (*property_map)[0x637CE8E5] = "m_aMaterialCollection";
        (*property_map)[0x31A43BF8] = "m_bIsGameControllerBindingPage";
        (*property_map)[0x69B8A91E] = "m_fFieldOfView";
        (*property_map)[0xC8AB2986] = "m_bAllowedWhenDying";
        (*property_map)[0xEF84694C] = "m_pBehavior";
        (*property_map)[0xDE30D9DC] = "m_rConfig";
        (*property_map)[0x29617A79] = "m_nLoopIndex";
        (*property_map)[0x2BFF6B42] = "m_bFirst";
        (*property_map)[0x218568E4] = "normal";
        (*property_map)[0xAEA1D671] = "diffuse";
        (*property_map)[0xF2FF2259] = "specular";
        (*property_map)[0xC8F5CA78] = "accumulatedLight";
        (*property_map)[0x40D9CFD2] = "m_fRangeModifierCurious";
        (*property_map)[0xFFC29475] = "glossiness";
        (*property_map)[0xE03234D0] = "ao";
        (*property_map)[0xEFAB4072] = "m_Melee_widget";
        (*property_map)[0x128BAC01] = "m_fCrosshairMaxAngleCap";
        (*property_map)[0xECAA48FA] = "m_bSecondIntroMovieIsSkippable";
        (*property_map)[0x8188A954] = "m_aNavigationNodes";
        (*property_map)[0xFD0AAFC1] = "m_ActualMatrix";
        (*property_map)[0xDE268E55] = "m_rContextLocation";
        (*property_map)[0xCAB5F7A5] = "distraction";
        (*property_map)[0x202CF01A] = "m_TimersData";
        (*property_map)[0x1388361E] = "m_bRandomStartState";
        (*property_map)[0x51A44F0] = "m_RandomTimersData";
        (*property_map)[0xF24E6511] = "m_nMinNumber";
        (*property_map)[0xA2A49CA2] = "m_bIsMainOpportunity";
        (*property_map)[0xB211849F] = "m_fMemberSlotFormingDistanceMin";
        (*property_map)[0x3C3C4FC5] = "m_SaveData";
        (*property_map)[0x4905B9FB] = "m_rWaypoint";
        (*property_map)[0xA3006DBC] = "m_eFadeType";
        (*property_map)[0xADD0265] = "m_pLinked";
        (*property_map)[0xB07A34A2] = "m_bRequiresTargetFacing";
        (*property_map)[0x6C7C6FB9] = "m_eGateType";
        (*property_map)[0x8E166644] = "m_RuntimeNetworkResourceID";
        (*property_map)[0xFA2EDA6F] = "m_rRoom";
        (*property_map)[0x2671A61] = "m_NearFrustums";
        (*property_map)[0x246BBA53] = "m_eHealthDrain";
        (*property_map)[0x53ABFD86] = "m_sParamName";
        (*property_map)[0xB7269C63] = "m_pPendingLeader";
        (*property_map)[0x4B89574B] = "m_fDisguiseFollowTime";
        (*property_map)[0x37DCE4E6] = "m_fTargetRunningTime";
        (*property_map)[0xE9DA54C5] = "m_eLeaderMoveSpeed";
        (*property_map)[0xE2A7EFA9] = "m_tLastEscalateOrder";
        (*property_map)[0xBFE7E6E5] = "m_TrackingBones";
        (*property_map)[0x9EC4576A] = "m_tStartFollow";
        (*property_map)[0xF668F701] = "m_DeathContext";
        (*property_map)[0x944B754A] = "m_bMembersFollow";
        (*property_map)[0xDB3D9938] = "m_bRunningWarningIssued";
        (*property_map)[0xE565BBBA] = "m_nExitSelection";
        (*property_map)[0x213FEC85] = "m_bLookAt";
        (*property_map)[0xB86CAC97] = "fAlpha";
        (*property_map)[0xE2895F79] = "nColor";
        (*property_map)[0x200AE96] = "m_fTurnRadius";
        (*property_map)[0xD4B9B7AD] = "logic";
        (*property_map)[0x868565F6] = "m_fAgilityAttentionDurationModifier";
        (*property_map)[0xB0226002] = "m_bAdditive";
        (*property_map)[0x9F528978] = "m_bDeadBodyBystander";
        (*property_map)[0x6BBBAE26] = "m_bInvestigateCautiousBystander";
        (*property_map)[0xF193422] = "m_eHintSound";
        (*property_map)[0x137B9CA4] = "m_bReplacePhoneAct";
        (*property_map)[0x838BE292] = "m_bStopWhenDialogEnds";
        (*property_map)[0x122A0AE8] = "m_vScreenDecalsPairBSize";
        (*property_map)[0x19CB5575] = "m_rState";
        (*property_map)[0xDF1F2CF6] = "m_fVignetteStrength";
        (*property_map)[0x6331DDDB] = "m_nObstacleActorIndex";
        (*property_map)[0x4D167DA0] = "eConditionType";
        (*property_map)[0xB80D1AE2] = "m_fScreenDecalsFadeTime";
        (*property_map)[0x14279A4C] = "m_nActiveInvestigationGroup";
        (*property_map)[0xAD440D10] = "m_bKinematicOnStartup";
        (*property_map)[0xDFCA7B0B] = "m_rDestination";
        (*property_map)[0x28B7C886] = "m_bVisibleOnStartup";
        (*property_map)[0x7B30BC5C] = "m_sMessageCenterPage";
        (*property_map)[0x66AA36A3] = "m_rAttacher";
        (*property_map)[0x87159FD1] = "m_bRequestedCancelMove";
        (*property_map)[0x6591C0AE] = "m_pSectionName";
        (*property_map)[0x1FA8FCA7] = "m_rActArmed";
        (*property_map)[0x47BB9E88] = "m_bLoopControlPoints";
        (*property_map)[0x93F98405] = "m_bUseControlPointTime";
        (*property_map)[0x94614321] = "m_bEnableReverse";
        (*property_map)[0xEF27E27] = "m_SplineDebugColor";
        (*property_map)[0xB9BC0808] = "m_eDetachOnShot";
        (*property_map)[0x7E65388C] = "m_fSplineDebugThickness";
        (*property_map)[0xF7A0FEE0] = "m_pMarkerGizmo";
        (*property_map)[0x98001248] = "m_fDoubleVisionBlendStrength";
        (*property_map)[0x9439C78E] = "m_fAngleTolerance";
        (*property_map)[0xEF6E4626] = "m_rOriginalDrama";
        (*property_map)[0xA203F2C5] = "m_bShouldShow";
        (*property_map)[0xB2FC3972] = "m_situation";
        (*property_map)[0x1BD578FF] = "m_vGlowPlayerSeenInLVAFadeNearFar";
        (*property_map)[0xC4D14A50] = "blockingTime";
        (*property_map)[0xBAD046D3] = "m_aZones";
        (*property_map)[0x9C23F821] = "m_sBaseSceneResID";
        (*property_map)[0xF77A44F1] = "m_iPlayer";
        (*property_map)[0x368D2F18] = "m_testData";
        (*property_map)[0x135F7A4E] = "m_iType";
        (*property_map)[0xEA395DA6] = "m_iScoringMachine";
        (*property_map)[0xC5DC5E60] = "m_fDefaultAlignAngle";
        (*property_map)[0x96EC0752] = "m_rFormationController";
        (*property_map)[0xBB3403B8] = "m_nDragBodyReactionDelay";
        (*property_map)[0xA8605448] = "m_eMinTension";
        (*property_map)[0x3D0E85A6] = "m_fPathOffset";
        (*property_map)[0x734EC1B2] = "m_fCircleFitRadius";
        (*property_map)[0xF23B48C6] = "m_nNumActorsInGroup";
        (*property_map)[0x3515F14E] = "m_bPromoteOnStart";
        (*property_map)[0x7ABF33B1] = "m_rTracker";
        (*property_map)[0x996B04BA] = "m_bSendValuesOnChange";
        (*property_map)[0x604103B2] = "m_rTo";
        (*property_map)[0x4A540735] = "m_aTransConditions";
        (*property_map)[0xD9EBDCBE] = "m_sComponentName";
        (*property_map)[0x6B408DE7] = "m_additionalGuards";
        (*property_map)[0xB9EA4804] = "m_aArgs";
        (*property_map)[0xC9F06969] = "m_eInputActionFilter";
        (*property_map)[0x67F3917D] = "m_pWaypoint";
        (*property_map)[0x3DF67DD3] = "m_fDoubleVisionScaleSpeed1";
        (*property_map)[0xC0F5DCF8] = "m_bAgitated";
        (*property_map)[0x5AA6F71] = "m_bAllowReposition";
        (*property_map)[0x50C1BA27] = "m_bEquipRequiredItem";
        (*property_map)[0xC08EC036] = "m_bShooting";
        (*property_map)[0x48E05695] = "m_nStateId";
        (*property_map)[0xA022ACB9] = "m_bRunToWaypoint";
        (*property_map)[0x19B7B92F] = "m_nGroupFlowStartChannel";
        (*property_map)[0x647258FE] = "m_bPlayWaypointAct";
        (*property_map)[0x515F2322] = "m_bIgnoreWaypoint";
        (*property_map)[0xA225B4B0] = "m_bGoIntoCoverAtWaypoint";
        (*property_map)[0xA7DA2E4A] = "m_nTriggerCount";
        (*property_map)[0x6F6A7864] = "m_fWeaponDamageModifier";
        (*property_map)[0xD6FA60CF] = "m_rDollySpatial";
        (*property_map)[0xD8AF68AB] = "m_fStartTrackPosition";
        (*property_map)[0x9E4B33F8] = "m_bRotateDolly";
        (*property_map)[0xB52D24AE] = "m_fVelocity";
        (*property_map)[0xF150620F] = "m_fNearEndPointDistance";
        (*property_map)[0x21E73D02] = "m_fMinOcclusion";
        (*property_map)[0x39A14742] = "m_bOverrideBootParametersAndForceWaitForInput";
        (*property_map)[0xA7D017C9] = "m_VisibilitySetting";
        (*property_map)[0x94F0268A] = "m_IllegalWeaponSeen";
        (*property_map)[0x482F6998] = "m_DisguisePeripheralAttentionDuration";
        (*property_map)[0x8EF78A6] = "m_DynamicDisguisePeripheralAttentionDuration";
        (*property_map)[0x7C66F7C8] = "m_MurderDisguisePeripheralAttentionDuration";
        (*property_map)[0x39F9BBE3] = "m_fDisguiseAttractionDistanceInCrowdFactor";
        (*property_map)[0xB0542F9A] = "m_bLuminanceOverrideEnable";
        (*property_map)[0x34A2E6EB] = "m_fDisguisePeripheralAngle";
        (*property_map)[0xD3EAD48] = "DYNAMIC_TRASH_COLLIDABLES";
        (*property_map)[0xCA83F77A] = "m_fTrespassingAttractionRunPenalty";
        (*property_map)[0xDC38950F] = "m_fTrackPosition";
        (*property_map)[0x3A9695C7] = "m_TrespassingAttentionDuration";
        (*property_map)[0x44C655C] = "m_HuntedAttentionDuration";
        (*property_map)[0x661B1265] = "m_pMapNavigator";
        (*property_map)[0xABFBA9DF] = "m_HuntedPeripheralAttentionDuration";
        (*property_map)[0x5CF4CAE] = "m_fAgilityAttractionIdlePenalty";
        (*property_map)[0x74180C34] = "m_fAgilityAttractionMovePenalty";
        (*property_map)[0x67CFEDBF] = "m_AgilityAttentionDuration";
        (*property_map)[0x7DD6ED31] = "m_bDeafAndBlind";
        (*property_map)[0x295756E1] = "m_fAttentionDecreaseTime";
        (*property_map)[0xF11E7EC8] = "m_fSightReactionMinAttention";
        (*property_map)[0x9C6D48AB] = "m_sHudIconIntel";
        (*property_map)[0x9E15C011] = "m_fSightInvestigationMinAttention";
        (*property_map)[0xA3C458BF] = "m_rSpatialTarget";
        (*property_map)[0xF14CDE04] = "m_eVerbosity";
        (*property_map)[0xB75C795A] = "m_fPenaltyDuration";
        (*property_map)[0xAC58F973] = "m_bPrint";
        (*property_map)[0x9E96B282] = "m_RunningEvents";
        (*property_map)[0xB159D20E] = "m_bUseCameraOffset";
        (*property_map)[0x1D69B076] = "m_fTwistLowLimit";
        (*property_map)[0x241B79FF] = "m_SickActs";
        (*property_map)[0x22579797] = "m_fTwistHighLimit";
        (*property_map)[0xC8937816] = "m_fTwistSpring";
        (*property_map)[0x4D93BE87] = "m_fTwistSpringDamping";
        (*property_map)[0x11503E21] = "m_fTwistRestitution";
        (*property_map)[0xB29D79A1] = "m_fSwing1LimitOffset";
        (*property_map)[0x3B83DD6C] = "eSharedEvent";
        (*property_map)[0x3A239BB1] = "m_fSwing1Limit";
        (*property_map)[0x9AAF4358] = "m_fSwing1Spring";
        (*property_map)[0x96E38B36] = "m_nSegments";
        (*property_map)[0x9C84D5B9] = "m_eMotionSwing2";
        (*property_map)[0xAB4759C5] = "m_fSwing2Spring";
        (*property_map)[0x4EAEF0B8] = "m_fSwing2SpringDamping";
        (*property_map)[0x5C736170] = "m_Keywords";
        (*property_map)[0xE1127CA1] = "m_eMotionTransZ";
        (*property_map)[0xEC84704E] = "m_fTransLimit";
        (*property_map)[0xC54904D4] = "m_nFlags";
        (*property_map)[0x15002E3] = "m_fCrosshairScale";
        (*property_map)[0x6DFB9A20] = "m_fTransDriveStrength";
        (*property_map)[0x450159CF] = "m_bLookAtTargetSet";
        (*property_map)[0xDB3EF35C] = "m_fMotorForce";
        (*property_map)[0xCCC26B47] = "m_fGlowBackgroundOutlineWidth";
        (*property_map)[0x6F3FBEF8] = "m_pBootMovieOther";
        (*property_map)[0xB36BB501] = "m_bNotebookMapEnabled";
        (*property_map)[0x462054D2] = "m_NavpowerResourceID";
        (*property_map)[0xBC852829] = "m_bObjectivesBarEnabled";
        (*property_map)[0x9EA2A7CB] = "m_bDebriefingVideoEnabled";
        (*property_map)[0xBA03DB2] = "m_ItemSize";
        (*property_map)[0x490B44A2] = "m_eGripAnimType";
        (*property_map)[0x78E4C94F] = "m_EquipAbility";
        (*property_map)[0xF0567738] = "m_eItemHUDType";
        (*property_map)[0x98D7CA0D] = "m_fGlowCivilianAlternate";
        (*property_map)[0xCFFD7D37] = "m_ThrowVelocity";
        (*property_map)[0x5F0A7918] = "m_fPanningFieldAzimuth";
        (*property_map)[0x8EB73F] = "m_fRagdollImpactForce";
        (*property_map)[0xA0CD3EEA] = "m_fItemImpactForce";
        (*property_map)[0xF9017418] = "m_fItemImpactUpKick";
        (*property_map)[0x4FCDC53] = "m_fBoneWeight";
        (*property_map)[0xB9CC5335] = "m_ItemHandsCoverAnimLayer";
        (*property_map)[0x758AD28B] = "m_rEnablePickupOnRelease";
        (*property_map)[0x87625640] = "m_bCanBeDropped";
        (*property_map)[0x30A4776A] = "m_bUseFakePutDownPhysics";
        (*property_map)[0x792FCF71] = "m_bKinematicWhileCarried";
        (*property_map)[0x8A92A1BB] = "m_Material";
        (*property_map)[0x820D7762] = "m_bForbidCloseCombat";
        (*property_map)[0x87CE1F77] = "m_bPlacementUseHitNormal";
        (*property_map)[0x25224FB7] = "m_asOnlineTraits";
        (*property_map)[0x2CA0062B] = "m_ObjectToMeasureFrom";
        (*property_map)[0xC80849E6] = "m_NoLOSShotgunFirePatterns";
        (*property_map)[0x60D5E4A4] = "m_fInteractionTimeMultiplier";
        (*property_map)[0x95900DAA] = "m_eActionRewardType";
        (*property_map)[0x2EBA2662] = "m_nGlowStashedGunsFadeSpeed";
        (*property_map)[0x883F61C0] = "m_bIsPerceivedAsWeapon";
        (*property_map)[0xD061D7B2] = "m_eThrownCollisionLoudness";
        (*property_map)[0xEEDE6710] = "m_bRecognizableByCivilians";
        (*property_map)[0xF951B0CB] = "m_aEffectSetsFrontSide";
        (*property_map)[0xD8606F94] = "m_bMaySocializeStanding";
        (*property_map)[0x4064CEB1] = "m_GlowLTMemoryUnconsciousOutlineColor";
        (*property_map)[0xB514F8C0] = "m_eButtonFirst";
        (*property_map)[0xD8537298] = "m_eButtonSecond";
        (*property_map)[0x35D18B17] = "m_sTextHeadline";
        (*property_map)[0xB6EAE1F1] = "m_iPriority";
        (*property_map)[0x797EACBD] = "m_eHintType";
        (*property_map)[0x6EA69D40] = "m_DeadPoseRepository";
        (*property_map)[0x9F874CFD] = "m_bWorksInPuristDifficulty";
        (*property_map)[0x610F924C] = "m_bForceSubtitles";
        (*property_map)[0x6AC05E9C] = "m_sDebugAudioEventName";
        (*property_map)[0x92A97EA3] = "m_rQuickReaction";
        (*property_map)[0x9695C2CA] = "m_bCutsceneLight";
        (*property_map)[0xD798C34B] = "m_rGroup";
        (*property_map)[0xB7849D1C] = "m_eIntelType";
        (*property_map)[0x8A2689D5] = "m_sharedTarget";
        (*property_map)[0xF13BA264] = "m_time";
        (*property_map)[0x17059F23] = "m_aAllowedProfessionsDisabled";
        (*property_map)[0xC9A728D] = "m_bTreatDyingAsDead";
        (*property_map)[0xF81420F9] = "m_fClampRadiusBlendSpeedIn";
        (*property_map)[0x75E11F3A] = "m_IgnoreDeadByUnnoticed";
        (*property_map)[0xB69C1873] = "m_pDataControl";
        (*property_map)[0xFAB2DBC0] = "m_aOutfitRepositoryIds";
        (*property_map)[0xA6236B94] = "m_fClampRadiusIn";
        (*property_map)[0xFC89BB8F] = "m_fYawBaseAngle";
        (*property_map)[0x9E5BED84] = "m_fYawLimit";
        (*property_map)[0xCFF53639] = "m_fMaxPitch";
        (*property_map)[0x9A3F9881] = "m_fTrackAngleFraction";
        (*property_map)[0xEB83332B] = "m_rSpatialEntity";
        (*property_map)[0x63094D4] = "m_bUseAnimlayer";
        (*property_map)[0x5302B84] = "m_nAnimlayerTime";
        (*property_map)[0xE81ADBB3] = "m_fDamageClose";
        (*property_map)[0xD0BD917A] = "m_sAnimationEvent";
        (*property_map)[0x3F250C56] = "m_bPlayFastSwipeOutsideLocomotion";
        (*property_map)[0x91952DBD] = "m_bPlaySwipeIfThereIsNoAnim";
        (*property_map)[0x2F254C56] = "m_rAnimationNetworkName";
        (*property_map)[0xF041E31] = "m_rContextObjectSpatial";
        (*property_map)[0x2262DF09] = "m_bRestorePreSubactionItems";
        (*property_map)[0x83264C70] = "m_bAlignFreeAttacherToContextObject";
        (*property_map)[0x7C021A59] = "m_bIgnoreEmptyPromptDescriptionText";
        (*property_map)[0x9E1C24A3] = "m_bAlignFreeAttacherToInteractionPoint";
        (*property_map)[0xD55DE82F] = "m_vLastKnownTargetPosition";
        (*property_map)[0xE7BFD35A] = "m_bOverrideInteractionRange";
        (*property_map)[0x40DD5A0C] = "speakFallback";
        (*property_map)[0x77D309FA] = "m_bOverrideInteractionRangeVR";
        (*property_map)[0x82C0F239] = "m_fOverrideInteractionRangeValueVR";
        (*property_map)[0x79C3530C] = "m_bAvailabilityNotification";
        (*property_map)[0xED5A27DD] = "m_bOneItemPerSubaction";
        (*property_map)[0x94DD87FE] = "m_rVisibleCondition";
        (*property_map)[0x15ECD27E] = "m_rReadyCondition";
        (*property_map)[0x28C09BB7] = "m_rAbortCondition";
        (*property_map)[0xAF5F9E69] = "m_bInterruptibleAction";
        (*property_map)[0x830FAB7D] = "m_bShowPromptInWorld";
        (*property_map)[0xF82A5CF6] = "m_cBounceCurve";
        (*property_map)[0xB2B53413] = "m_sPromptTextPassive";
        (*property_map)[0xA20938D6] = "m_sPromptDescriptionPassiveText";
        (*property_map)[0x8879E8E5] = "rank";
        (*property_map)[0x2CE25EEC] = "m_rPromptTextPassiveResource";
        (*property_map)[0xDF2890B8] = "m_aPromptDescriptionTextPassive";
        (*property_map)[0x6B52CCB6] = "m_rNonExecutableDescriptionResource";
        (*property_map)[0x9359AA81] = "m_eActionGroupIcon";
        (*property_map)[0x9ADCF051] = "m_bUseItemIllegalityInInventorySubactionGroup";
        (*property_map)[0x16369D2D] = "m_aLocoAnimDBs";
        (*property_map)[0xBDE9DAF6] = "m_rUsableConditionKeywordsA";
        (*property_map)[0x660DEEC5] = "m_eUsableConditionKeywordsBehaviorA";
        (*property_map)[0x24E08B4C] = "m_rUsableConditionKeywordsB";
        (*property_map)[0x4B923428] = "m_bUsableConditionUseAllEquippableItems";
        (*property_map)[0x109C4882] = "m_bVRForceSwipeAnimation";
        (*property_map)[0x4715BCBE] = "m_bVRHideAnimation";
        (*property_map)[0x218343EB] = "m_bIsSafeRoom";
        (*property_map)[0x93055629] = "m_bIsRoutingNode";
        (*property_map)[0x21FBE3F1] = "m_bLeaveSafeRoomOnDeactivation";
        (*property_map)[0x2A88B980] = "m_eActorFaction";
        (*property_map)[0x2BC3C6FA] = "m_bThreatAssessmentIncludeGuardPoints";
        (*property_map)[0xC8021EF2] = "m_fMaxRandPower";
        (*property_map)[0x29F084F1] = "sInstanceId";
        (*property_map)[0x453686ED] = "m_disguiseZones";
        (*property_map)[0xD4F3ABDB] = "m_fRecoilRecoveryTime";
        (*property_map)[0x955DAB66] = "m_bNoEscortingDialog";
        (*property_map)[0xDA163881] = "m_eBystanderState";
        (*property_map)[0x5898477A] = "m_pUnmuteMusicEvent";
        (*property_map)[0x7DA96C05] = "m_bEnableStrafeSlowDown";
        (*property_map)[0x896742B5] = "m_tLastApproachOrder";
        (*property_map)[0xD6385578] = "m_tHuntTargetLastUpdate";
        (*property_map)[0xDA5FC908] = "m_tNewHuntTargetTimer";
        (*property_map)[0x342A10F] = "m_bLockdownZoneDisturbanceAdded";
        (*property_map)[0x8277CF81] = "m_bLockdownSituation";
        (*property_map)[0xE48A3484] = "m_bLockdownFalseAlarm";
        (*property_map)[0x577CD1BB] = "m_bLockdownGracePeriod";
        (*property_map)[0xD9632D82] = "m_bReasonToFrisk";
        (*property_map)[0xF2500364] = "m_bSpatialBlurEnabled";
        (*property_map)[0x82676DA5] = "m_bSomeoneHasBeenInCombat";
        (*property_map)[0x89A9F5A4] = "m_bStandDownGuards";
        (*property_map)[0x33266F3E] = "m_eMostSevereDisturbance";
        (*property_map)[0x5BC5366D] = "m_bDiscoveredDeadBody";
        (*property_map)[0x2C8D30BF] = "m_sRigidbodyImpactsDynamicSwitchGroup";
        (*property_map)[0xDFB9F26C] = "m_bDiscoveredPacifiedBody";
        (*property_map)[0xEACED4EA] = "m_pMovie";
        (*property_map)[0x850DD566] = "m_bMenuFrameEnabled";
        (*property_map)[0x56943BEC] = "m_iStoreReason";
        (*property_map)[0x536179E0] = "m_bShowHelpPage";
        (*property_map)[0x32B0C024] = "m_fBlindfirePrecisionModifier";
        (*property_map)[0x1CC410EE] = "m_sGroup";
        (*property_map)[0xC250B50E] = "m_fWireSpacing";
        (*property_map)[0xAAAAC396] = "m_pPageContent";
        (*property_map)[0x60651442] = "m_fHDRBloomFalloffDistancePower";
        (*property_map)[0xCFFB4D4E] = "m_TestData";
        (*property_map)[0x84464F68] = "m_vGlowVSOpponentVignetteRadii";
        (*property_map)[0xD058A198] = "m_nGlowInteractionFadeSpeed";
        (*property_map)[0xE33E850F] = "m_TestHelpData";
        (*property_map)[0x2BCF5DD6] = "m_fRecoilXPenalty";
        (*property_map)[0x5B3D6F02] = "m_bInFrontOfCamera";
        (*property_map)[0xAE70C6BB] = "m_rTypeKeywords";
        (*property_map)[0x456CB043] = "m_fGlowLTMemoryOutlineWidth";
        (*property_map)[0x53668813] = "m_sContainerInstanceId";
        (*property_map)[0x25B454C0] = "m_concealedItem";
        (*property_map)[0x67274A54] = "m_rEnterZone";
        (*property_map)[0xDFCE6710] = "m_bOnlyCulled";
        (*property_map)[0x8372654B] = "m_fStepSize";
        (*property_map)[0xBE6B4973] = "m_bIntegerValues";
        (*property_map)[0x300DBBD1] = "m_Situation";
        (*property_map)[0x4D028A55] = "m_PenaltyCombat";
        (*property_map)[0xCB780A35] = "m_ActivatedCategory";
        (*property_map)[0x74B098A0] = "m_UnavailableCategory";
        (*property_map)[0x6CDB2D67] = "m_Interaction1";
        (*property_map)[0xF5D27CDD] = "m_Interaction2";
        (*property_map)[0xE3DE8078] = "m_rTracerEffect";
        (*property_map)[0xDF259EBE] = "m_fTracerEveryNthBullet";
        (*property_map)[0x4DBCAE3] = "m_aPutdownScreenplays";
        (*property_map)[0x537F0DE6] = "m_TriggerSpatials";
        (*property_map)[0x1506E2E4] = "m_aBreakScreenplays";
        (*property_map)[0x8F00A533] = "m_IsStandingDown";
        (*property_map)[0x2FBC379C] = "m_AverageDelay";
        (*property_map)[0x33C96A60] = "m_DelayVariance";
        (*property_map)[0x2211C8F1] = "m_bHDRLensFlareEnabled";
        (*property_map)[0xB03A12EA] = "anyPlatformSpecificPropertiesRecursive";
        (*property_map)[0xCCB14C78] = "exposedTypes";
        (*property_map)[0x972C1BDA] = "runtimeType";
        (*property_map)[0xB8C5BE10] = "m_bInitiallyActive";
        (*property_map)[0xE765434A] = "runtimeBlueprint";
        (*property_map)[0xD604022C] = "inputPins";
        (*property_map)[0x54E4BD8F] = "outputPins";
        (*property_map)[0x775FE867] = "m_HeadOffsetFar";
        (*property_map)[0xD047047E] = "STAIRS";
        (*property_map)[0x880830ED] = "referencedEntityTypes";
        (*property_map)[0x94C3D3C7] = "m_fRadialBlurStart";
        (*property_map)[0x7E95FC36] = "exposedEntitiesTypes";
        (*property_map)[0x8CAFEB96] = "m_aOutfitKeywords";
        (*property_map)[0xEFF287F6] = "m_fSleeveConstraintFraction";
        (*property_map)[0xCD8FE01C] = "m_vScale";
        (*property_map)[0xE21BAE20] = "m_bVisibilityCheckCurrentMap";
        (*property_map)[0xE368DFED] = "m_ReporterIsVIPWithAmbientEscort";
        (*property_map)[0xC2974ADA] = "m_bVisibilityCheckDetectors";
        (*property_map)[0x3FE76031] = "m_sFormatting";
        (*property_map)[0x688C8762] = "m_bEnableEqualPowerCrossfade";
        (*property_map)[0xB91C86B2] = "m_bShowOnEdge";
        (*property_map)[0x383FDADC] = "m_bShowOnLegend";
        (*property_map)[0xDADB6817] = "m_bShowWhenZoomedIn";
        (*property_map)[0xABD8B32F] = "m_bShowWhenZoomedOut";
        (*property_map)[0x6B79C7D] = "m_fGlowInteractionSelectedOutlineWidth";
        (*property_map)[0xA0A5E54A] = "m_pHoverDataProvider";
        (*property_map)[0x517F1F9D] = "m_bOverrideListenerPosition";
        (*property_map)[0x8BE18BBA] = "m_ProjectorMap";
        (*property_map)[0xB21ACCF3] = "objects";
        (*property_map)[0x2105A18A] = "m_bOverrideOcclusionPosition";
        (*property_map)[0xCE9701E] = "m_bOverrideObstructionPosition";
        (*property_map)[0x9FBCF25B] = "m_rOrientationEntity";
        (*property_map)[0x93A2A0BE] = "m_rOcclusionPositionEntity";
        (*property_map)[0x6DDF5847] = "m_rObstructionPositionEntity";
        (*property_map)[0x973C07EB] = "m_bInstantiateNetworkPropertyVal";
        (*property_map)[0x7E355FB5] = "sEntityID";
        (*property_map)[0xD2438FEB] = "sTypeArgName";
        (*property_map)[0x3FF192F6] = "m_bRunInStopmode";
        (*property_map)[0xE8F91D81] = "m_eCurveType";
        (*property_map)[0xF9BB63F5] = "m_cautiousHuntData";
        (*property_map)[0xC02184AD] = "m_fOffset";
        (*property_map)[0xA5D1E0FE] = "m_bForceInstantInventoryChange";
        (*property_map)[0x10C95176] = "m_fStartTime";
        (*property_map)[0x7ABB8967] = "m_sendContractId";
        (*property_map)[0xF26FA91E] = "m_sendContractSessionId";
        (*property_map)[0x8576052F] = "m_sendRoomId";
        (*property_map)[0x46679574] = "m_vListenerAngularVelocity";
        (*property_map)[0x879221F6] = "m_rDefaultPickupNetwork";
        (*property_map)[0x9930DF] = "m_pRenderable";
        (*property_map)[0xDCE63B1] = "m_rNode";
        (*property_map)[0xC6BAA42F] = "m_fGlowTagFocusTaggedOutlineWidth";
        (*property_map)[0xB520F369] = "m_aNodeList";
        (*property_map)[0x8FD2B0D1] = "m_sBulletImpactsSwitchGroup";
        (*property_map)[0xC806026] = "m_fMemberSlotDistanceMin";
        (*property_map)[0x99230776] = "m_Display";
        (*property_map)[0xA584AE78] = "m_fGlowTaggedOutlineWidth";
        (*property_map)[0x4FB33326] = "m_uri";
        (*property_map)[0xCF92459D] = "m_eAnchor";
        (*property_map)[0xFA244C65] = "m_nTextOffsetX";
        (*property_map)[0xE348192] = "m_nMultiTrackCount";
        (*property_map)[0x672A8420] = "m_bRadialBlurEnabled";
        (*property_map)[0x8D237CF3] = "m_nTextOffsetY";
        (*property_map)[0x280DB675] = "m_fTrackRadius";
        (*property_map)[0xB90EEEB5] = "m_nTextSize";
        (*property_map)[0x527B084A] = "m_aPhysicsAccessors";
        (*property_map)[0xF326F624] = "m_pMainEvent";
        (*property_map)[0x5E237E06] = "name";
        (*property_map)[0xC87D14EC] = "m_aSituationChangeReasons";
        (*property_map)[0xB0541BA] = "flags";
        (*property_map)[0x759C17BE] = "m_bIsDollyAttached";
        (*property_map)[0x24939FD6] = "m_vDollyPosition";
        (*property_map)[0x9F747510] = "m_rGeomEntity";
        (*property_map)[0xD463C0F8] = "m_aExclusionVolumeShapes";
        (*property_map)[0x14B35A72] = "m_nGroupSize";
        (*property_map)[0xB7382422] = "m_fAngularVelocityThreshold";
        (*property_map)[0xABEF6512] = "m_aSeedPoints";
        (*property_map)[0x207C75AB] = "m_nIdleRatio";
        (*property_map)[0xEBE7B29] = "m_bPassed";
        (*property_map)[0x840615BB] = "m_aGeoms";
        (*property_map)[0xB806DEBB] = "m_fObstructionAttenuation";
        (*property_map)[0x9D04BB18] = "m_f3DWeight";
        (*property_map)[0x7F62EA9A] = "m_aCuriousEvents";
        (*property_map)[0xE98E64EE] = "m_fInsideGain";
        (*property_map)[0xDC88C9A7] = "m_eMixingMode";
        (*property_map)[0x2433CF81] = "m_aDisguisesAllowed";
        (*property_map)[0x91AB85C4] = "m_bFollowListenerOrientation";
        (*property_map)[0xEC1907E5] = "m_rPlayerIsInRangeOfSpread";
        (*property_map)[0xF03A4299] = "m_f2DMultichannelSpread";
        (*property_map)[0x45363B89] = "sUnlockableId";
        (*property_map)[0xA1E67258] = "repositoryId";
        (*property_map)[0xDB960928] = "aModifierIds";
        (*property_map)[0x169C96D9] = "m_bGuard";
        (*property_map)[0x49AFA943] = "m_aCollections";
        (*property_map)[0xDA328741] = "m_bCivilian";
        (*property_map)[0x8F2AED93] = "m_rSniperLocation";
        (*property_map)[0xEE562D34] = "m_bCivilianToHitman";
        (*property_map)[0x6A463418] = "m_nMaxWitnesses";
        (*property_map)[0x791EF502] = "m_aIncludedPropertyNames";
        (*property_map)[0x9F048FE5] = "m_replicaNSID";
        (*property_map)[0xFCE2081C] = "m_eMaxTensionAudio";
        (*property_map)[0xDBCBD26E] = "m_compromisedOnEnter";
        (*property_map)[0x40E568BB] = "m_sKeyword";
        (*property_map)[0xDC129E1D] = "m_rHolder";
        (*property_map)[0xE8284A6C] = "m_WaterSplashEffect";
        (*property_map)[0xB6422026] = "m_bMakeHiddenBodiesInvisibleToAI";
        (*property_map)[0x674FBA89] = "m_bDrownEvent";
        (*property_map)[0x2AC493F6] = "m_nResourceId";
        (*property_map)[0x46C4069F] = "m_aEntityIDs";
        (*property_map)[0xBC91D5D9] = "m_aEntityNames";
        (*property_map)[0xA115AE7E] = "m_vRotationOffset";
        (*property_map)[0xB340CD9F] = "m_bCenter2DPlane";
        (*property_map)[0x9E2AEF2D] = "m_fPlaneDistance";
        (*property_map)[0x7B36FEDA] = "m_aPoints";
        (*property_map)[0x52331971] = "m_fPlaneWidthSize";
        (*property_map)[0xF35DE271] = "m_fCapsuleAttacherFwdOffsetPSVR";
        (*property_map)[0xB6919AEE] = "m_fSpreadTimeThreshold";
        (*property_map)[0x8395CCDB] = "m_fCloseupPlaneWidthSize";
        (*property_map)[0x6843F97E] = "m_fCloseupPlaneAlpha";
        (*property_map)[0xC61F6EBC] = "m_fOccludedAlpha";
        (*property_map)[0x4F7CFEFE] = "m_fPlaneSwitchSmoothing";
        (*property_map)[0x1F173489] = "m_rAvailableCondition";
        (*property_map)[0xD5A6228F] = "m_nExpirationTime";
        (*property_map)[0xA98543AB] = "m_bFinishedStage";
        (*property_map)[0x102573CA] = "m_nGlowLTMemoryFadeSpeed";
        (*property_map)[0x732E1A97] = "m_aStages";
        (*property_map)[0x17442871] = "m_aDisabledStages";
        (*property_map)[0x6C864682] = "m_aFailedStages";
        (*property_map)[0xFAF844C9] = "m_fTransitionAmount";
        (*property_map)[0x5A2B7E60] = "m_fPulseWalkInc";
        (*property_map)[0x88E5738] = "m_bAlertedLow";
        (*property_map)[0x94F40D80] = "m_eIntelStage";
        (*property_map)[0xC0A9088B] = "m_fZoomedInLevel";
        (*property_map)[0xB0F86280] = "m_sConsumerID";
        (*property_map)[0x2F2ADF7C] = "m_pHandlerSpeak";
        (*property_map)[0x31CBED66] = "m_GlowCameraOutlineColor";
        (*property_map)[0x20BA6DE0] = "m_bUseLogChannel";
        (*property_map)[0x9196490B] = "m_fIntelNotificationDuration";
        (*property_map)[0xB1A12D45] = "m_rXAxis";
        (*property_map)[0x58E469B7] = "m_rHeadlineResource";
        (*property_map)[0x10E8C940] = "m_sFlavor";
        (*property_map)[0x1DC3EC32] = "m_nId";
        (*property_map)[0xEDC15C3] = "m_sFlavorFormatting";
        (*property_map)[0x751312BE] = "m_bBlockDrag";
        (*property_map)[0xA3875941] = "m_sSortingGroup";
        (*property_map)[0x6F0813CA] = "m_sNotificationHeadline";
        (*property_map)[0x6C81F039] = "m_rNotificationHeadlineResource";
        (*property_map)[0xFC833541] = "m_fPrecisionUnaimedMinDegree";
        (*property_map)[0xFF3617E3] = "m_rNotificationBodyResource";
        (*property_map)[0x19C16ACE] = "m_sHudIconCompleted";
        (*property_map)[0x4A5DD30E] = "m_disturbances";
        (*property_map)[0xD6237764] = "m_sHudIconFailed";
        (*property_map)[0x6D55DB28] = "m_bRelock";
        (*property_map)[0xDA505865] = "m_rOpportunityHint";
        (*property_map)[0xEF784A1F] = "m_bKill";
        (*property_map)[0xD0C2C54C] = "m_bKnockOut";
        (*property_map)[0x7ECDC637] = "m_bInvisibleToAI";
        (*property_map)[0x3456CE9E] = "m_GlowPlayerInLVAOutlineColor";
        (*property_map)[0xE5226564] = "m_bDisable";
        (*property_map)[0xAFA48E1E] = "m_bAllowDrag";
        (*property_map)[0x1A47913B] = "m_bBlockCloseCombat";
        (*property_map)[0x353B37ED] = "sMissingToolText";
        (*property_map)[0x6371079D] = "m_fCombatLastKnownPositionTime";
        (*property_map)[0xDAB4567] = "m_fCombatAggressionDeathIncrease";
        (*property_map)[0x293DC7E1] = "m_vContactNormal";
        (*property_map)[0x40391FEA] = "m_fCombatAggressionGunshotIncrease";
        (*property_map)[0xF7B36CEE] = "m_fCombatAggressionAimingIncreaseRate";
        (*property_map)[0xB33C7467] = "m_fPropagationProximityMaxDistance";
        (*property_map)[0x5AFD8168] = "m_fPropagationLOSMaxDistance";
        (*property_map)[0x4052FC34] = "m_fCivilianPropagationProximityMinDistance";
        (*property_map)[0x8C5F873B] = "m_fCivilianPropagationProximityMaxDistance";
        (*property_map)[0x8B05E8B6] = "m_fCivilianPropagationProximitySpreadPerSecond";
        (*property_map)[0x29BD6B1] = "m_fHuntGuardPointMaxDistance";
        (*property_map)[0x4F552B38] = "m_bSniperLevelHack";
        (*property_map)[0xB01EC378] = "m_fInvestigateCautiousPropagationProximityMinDistance";
        (*property_map)[0x94815F8B] = "m_fInvestigateCautiousPropagationProximitySpreadPerSecond";
        (*property_map)[0x4372290A] = "m_fInvestigateCautiousPropagationLOSSpreadPerSecond";
        (*property_map)[0xA7D42110] = "m_fInvestigateCautiousPropagationGracePeriod";
        (*property_map)[0x5FDF66E0] = "m_fInvestigateCautiousTargetDistance";
        (*property_map)[0x66E68B06] = "m_fInvestigateCautiousCivilianPropagationProximityMaxDistance";
        (*property_map)[0x87462723] = "m_fInvestigateCautiousCivilianPropagationLOSMaxDistance";
        (*property_map)[0xCB02BA36] = "m_fInvestigateCautiousCivilianPropagationLOSSpreadPerSecond";
        (*property_map)[0xB5E5E9A8] = "m_fInvestigateCautiousCivilianPropagationGracePeriod";
        (*property_map)[0x5851B9EF] = "m_fInvestigateCautiousCivilianTargetDistance";
        (*property_map)[0x92EDD32] = "m_LOSPistolFirePatterns";
        (*property_map)[0x4B508F8] = "m_NoLOSPistolFirePatterns";
        (*property_map)[0xC308A85A] = "m_pBoneMask";
        (*property_map)[0xC3779EF7] = "m_nTimeLimit";
        (*property_map)[0xAA9B92B9] = "m_nEmotionLimit";
        (*property_map)[0xE6F9000] = "m_nEmotionRemoveLimit";
        (*property_map)[0x1CE7ABEC] = "m_nTensionRemoveLimit";
        (*property_map)[0x3BA541DD] = "m_sFontId";
        (*property_map)[0x5201B5AE] = "m_BoxSize";
        (*property_map)[0x8328894A] = "m_eCustomRule";
        (*property_map)[0x1B85C24E] = "m_fBoxVisibleEnd";
        (*property_map)[0xE00D331C] = "m_fFadeOutSpeed";
        (*property_map)[0x66BA8141] = "m_BoxSpatial";
        (*property_map)[0x8C4CA2D9] = "m_sContractID";
        (*property_map)[0xB7FFD66F] = "m_pMapDynamicLayer";
        (*property_map)[0xB812C937] = "m_context";
        (*property_map)[0x91ADE5EF] = "m_pTrackerLayer";
        (*property_map)[0xA06BA9F0] = "m_vTrackerScaleFactor";
        (*property_map)[0x299034D7] = "m_pLayerView";
        (*property_map)[0x97FD3D5A] = "m_pMapLayerInfoDataProvider";
        (*property_map)[0x6841437E] = "m_fCorridorWidth";
        (*property_map)[0x67E33115] = "m_fWaitTimeout";
        (*property_map)[0x59B08CAF] = "m_bSuppressTrespass";
        (*property_map)[0xBB022ABE] = "m_AlertableGuardOnScreenCooldown";
        (*property_map)[0xC4B65E9E] = "m_pParentController";
        (*property_map)[0x9B1A8928] = "aKeywords";
        (*property_map)[0x157930FE] = "m_fBulletProof";
        (*property_map)[0xF1990F7D] = "m_fTetherDistanceMin";
        (*property_map)[0x6B553192] = "m_aLocations";
        (*property_map)[0x8A7752A5] = "durationVariance";
        (*property_map)[0x937480F2] = "keepFacing";
        (*property_map)[0xA627E29A] = "m_input0";
        (*property_map)[0xD120D20C] = "m_input1";
        (*property_map)[0x8BC27B14] = "m_bEnableBlendMask";
        (*property_map)[0xBF5548A4] = "speak_true";
        (*property_map)[0x693D5958] = "speak_false";
        (*property_map)[0xDD01C5F8] = "m_bDepthOfFieldEnabled";
        (*property_map)[0x9590D21E] = "m_iShotsPerMinute";
        (*property_map)[0xC6D6B4BF] = "m_fDepthOfFieldBlurriness";
        (*property_map)[0x2FAD017D] = "m_vRadialBlurCenter";
        (*property_map)[0xAB32F219] = "m_vSpatialBlurCenter";
        (*property_map)[0xD2FB11DB] = "m_bDistortionWobbleEnabled";
        (*property_map)[0x99DF223C] = "m_nGlowBackgroundUnmaskedFadeSpeed";
        (*property_map)[0xEED7C70A] = "m_fDistortionWobbleScale";
        (*property_map)[0xD2702951] = "lstrLocation";
        (*property_map)[0x5A963E52] = "m_vDistortionWobbleWaveLength";
        (*property_map)[0x267CCBCC] = "m_bDistortionWobbleUseRealTime";
        (*property_map)[0x4E8AB2A1] = "m_vHDRAdaptationSpeed";
        (*property_map)[0xB7AB42C4] = "m_vHDRAdaptationLuminanceMinMax";
        (*property_map)[0xEC5D129E] = "m_vHDRAdaptationMiddleGrayMinMax";
        (*property_map)[0x50889E6] = "m_fHDRWhitePoint";
        (*property_map)[0x588DEDA4] = "m_fHDRBrightPassThreshold";
        (*property_map)[0x5E74A7D5] = "m_HDRColorTint";
        (*property_map)[0x85130375] = "m_bAllowRollingNoBrick";
        (*property_map)[0xBD48EAED] = "m_aBrickList";
        (*property_map)[0xDD64DA45] = "m_fPoweredRagdollsImpulseScale";
        (*property_map)[0x10C4DCA2] = "m_fPoweredJointImpactDamping";
        (*property_map)[0xD35914DE] = "m_fPoweredJointFallDamping";
        (*property_map)[0xD05A091C] = "m_fPoweredJointRecoveryStrengthScale";
        (*property_map)[0x2D8EC693] = "m_fImpactMaxPFVerticalMaxDistance";
        (*property_map)[0xCF03B735] = "m_fFakeMultipleScattering";
        (*property_map)[0x2058D4E6] = "m_fImpactMaxPFHorizontalDistance";
        (*property_map)[0x805E48AF] = "m_bIgnoreDefaultSpreaders";
        (*property_map)[0x59D60360] = "m_bNormalize";
        (*property_map)[0xA6795E54] = "m_bStartedSignalSent";
        (*property_map)[0xB66881F3] = "m_bForceTimeout";
        (*property_map)[0x1F7F4903] = "m_vProjectorMapTiling";
        (*property_map)[0x4D6743E7] = "m_nDrawOrder";
        (*property_map)[0xFBF7BB97] = "m_fDepthOfFieldFocalFadeNear";
        (*property_map)[0xB79F8BC3] = "m_pArrestor";
        (*property_map)[0xA3B5994A] = "m_rIsland";
        (*property_map)[0xDF408F14] = "m_aAvoidDangerGroups";
        (*property_map)[0x7FE3C69B] = "m_bWarZone";
        (*property_map)[0x9644EB38] = "m_bAllowLockdown";
        (*property_map)[0xEF2EDFC0] = "m_Volumes";
        (*property_map)[0x405E0ECD] = "m_ClearOutVolumes";
        (*property_map)[0xEFD789BE] = "m_TargetSpatial";
        (*property_map)[0x140F3361] = "m_agitatedGuardPoints";
        (*property_map)[0x3D41457F] = "m_MiddlePlaneOffset";
        (*property_map)[0x2A8BB5F8] = "m_guardPoints";
        (*property_map)[0x89D64B14] = "m_PatrolRoutes";
        (*property_map)[0x8F7DC9B2] = "m_AlarmButtons";
        (*property_map)[0xA81ECDD0] = "m_rInfiniteLockdown";
        (*property_map)[0xC7F960AE] = "m_aEvacZones";
        (*property_map)[0xA9AAE268] = "m_aIgnoreList";
        (*property_map)[0xC6ABC026] = "m_bSpreadWhenDead";
        (*property_map)[0xB8BEA7B2] = "m_bSpreadWhenPacified";
        (*property_map)[0xC521D9CC] = "m_rEvaluator";
        (*property_map)[0xA1DA06EA] = "m_rActorCounter";
        (*property_map)[0x332E2978] = "m_pCollisionShatterEvent";
        (*property_map)[0x26B19372] = "m_RandomizeDefaultInfectedTime";
        (*property_map)[0xFE6CE25C] = "m_DefaultInfectedTimeMin";
        (*property_map)[0xC261DD05] = "m_DefaultInfectedTimeMax";
        (*property_map)[0x73D2859F] = "m_aSniperCombatGroups";
        (*property_map)[0x5C5611EA] = "m_fSizeA";
        (*property_map)[0xC55F4050] = "m_fSizeB";
        (*property_map)[0x3B7C6E5A] = "radius";
        (*property_map)[0xA516B255] = "m_fSlowDownFarDist";
        (*property_map)[0x112276E1] = "m_bEnableExplosionBlocker";
        (*property_map)[0x7979913F] = "m_fHeightAboveSeaLevel";
        (*property_map)[0x9EBA7161] = "m_RayleighColor";
        (*property_map)[0xF3F36EE1] = "m_fMieHeightScale";
        (*property_map)[0x450B83A6] = "m_fRayleighHeightScale";
        (*property_map)[0x26B78CBE] = "m_fMieDepthScale";
        (*property_map)[0xBB8EB8C4] = "m_fSunRadiusScale";
        (*property_map)[0x6664CE08] = "m_fMinimumDistance";
        (*property_map)[0x7AF0C665] = "m_pAccidentReaction";
        (*property_map)[0x5D85C914] = "m_bIsInGameMenu";
        (*property_map)[0xCE3E2DE2] = "m_iValue";
        (*property_map)[0x3589B0EA] = "m_bValid";
        (*property_map)[0xD87F7E0C] = "test";
        (*property_map)[0x2483267] = "m_bUseCurve";
        (*property_map)[0x283EAC4B] = "m_bCheckKill";
        (*property_map)[0x4D01C34A] = "m_fSkillChangeAmmoSpeedCoef";
        (*property_map)[0x69EA5AAB] = "m_fRecoilZPenalty";
        (*property_map)[0xB139466] = "m_locale";
        (*property_map)[0xC9C6B04B] = "m_role";
        (*property_map)[0xAF368C87] = "m_bHideActorsUseCircle";
        (*property_map)[0xAF3B2B8B] = "m_ThrowingActor";
        (*property_map)[0x8B469DC6] = "m_aDramas";
        (*property_map)[0x52CFC96F] = "m_rStartCondition";
        (*property_map)[0x34D9EB5A] = "m_bIsEnabledCondValid";
        (*property_map)[0xEC2F961F] = "m_bSendDirectly";
        (*property_map)[0x92460AF0] = "m_aVRRHStateConfigs";
        (*property_map)[0xD6A58889] = "m_aInclusionSpaces";
        (*property_map)[0xFCAC0CA6] = "m_FireMinSeconds";
        (*property_map)[0x45ECCC4D] = "m_idScreenDecalsSingleTex";
        (*property_map)[0x2BA56971] = "m_pDestructible";
        (*property_map)[0xD0E68460] = "m_eExplodingPropType";
        (*property_map)[0xCDE51646] = "m_fSwivelAngle";
        (*property_map)[0x3AF9868A] = "m_fSwivelContribution";
        (*property_map)[0x88745EA6] = "m_HeadVisibleThreshold";
        (*property_map)[0x427A65D7] = "m_WaveModifiers";
        (*property_map)[0xAAD035BF] = "m_sStartPage";
        (*property_map)[0x684338D5] = "Disturbance";
        (*property_map)[0x69DF71CD] = "m_IsDrownEvent";
        (*property_map)[0xC5288D1C] = "m_bTargetRegistered";
        (*property_map)[0xFA0B1611] = "m_rTargetSpatial";
        (*property_map)[0x8E9708A3] = "m_bPacifyTarget";
        (*property_map)[0x4AD4117B] = "m_bUseReverbBlendDistance";
        (*property_map)[0x7E38D13] = "m_vLookAtTarget";
        (*property_map)[0x6DF53C33] = "m_rChildNetworkEntity";
        (*property_map)[0xD4EBEDA8] = "m_targetEmotionState";
        (*property_map)[0xEBE268BD] = "m_bDeadbody";
        (*property_map)[0xE613A137] = "m_bExplosion";
        (*property_map)[0xF21B82DB] = "m_bFaceTargetSet";
        (*property_map)[0x421732A3] = "m_fSpringMassMedium";
        (*property_map)[0x97C3BFFF] = "m_fSpringMassPrimedSmall";
        (*property_map)[0xF533BDF1] = "m_fSpringBounceOffsetSmallSneak";
        (*property_map)[0xBA54D48B] = "m_fSpringBounceOffsetMedium";
        (*property_map)[0x906D46D8] = "m_fSpringBounceOffsetMediumSneak";
        (*property_map)[0x129EA77A] = "m_fSpringBounceOffsetLargeSneak";
        (*property_map)[0x89B1DB66] = "m_eCurrentRequest";
        (*property_map)[0x47784B04] = "m_bItemUpdateTransformChange";
        (*property_map)[0x24C2A28F] = "m_aEntranceZones";
        (*property_map)[0xF74867EB] = "m_aWarningZones";
        (*property_map)[0x73ED3E93] = "m_rMayPlayFriskingDialogCondition";
        (*property_map)[0xA2E9EDC2] = "m_rRequiredItemKeywords";
        (*property_map)[0x6E5A8CD4] = "m_fGreetingCooldownMax";
        (*property_map)[0x7EEEA520] = "m_fLoiteringCooldownMin";
        (*property_map)[0x42E39A79] = "m_fLoiteringCooldownMax";
        (*property_map)[0xAD8FA8DC] = "m_eReactionSpeak";
        (*property_map)[0xABBFD9FD] = "modifier";
        (*property_map)[0xCDB3325C] = "m_pBulletShellEmitterProxy";
        (*property_map)[0x8855022B] = "m_pBulletImpactEvent";
        (*property_map)[0x8677428] = "m_pCollisionRollingEventDynamic";
        (*property_map)[0xAF51FA9] = "m_rDistractionObject";
        (*property_map)[0xB2BE5AD1] = "m_pCollisionSlidingEventDynamic";
        (*property_map)[0x8DB7DC2A] = "m_pCollisionSlidingEventRagdoll";
        (*property_map)[0xE684AF6A] = "m_fRollingEventCooldownTime";
        (*property_map)[0xB144A8F0] = "m_pMaterialUndefined";
        (*property_map)[0x7A497EFA] = "m_pMaterialRagdollHead";
        (*property_map)[0x7AE0129F] = "m_fScreenDecalsSizeDeviation";
        (*property_map)[0x1435B1F5] = "m_pMaterialRagdollFoot";
        (*property_map)[0xFAD8CAE9] = "m_pMaterialRagdollHand";
        (*property_map)[0xF6F34EF4] = "m_fDistanceToExit";
        (*property_map)[0xC016932F] = "m_bNoInitialDialog";
        (*property_map)[0x129FC0E6] = "m_rTransformReference";
        (*property_map)[0xA9BADF0A] = "m_rWind";
        (*property_map)[0xB57E7978] = "m_rSpeed";
        (*property_map)[0x75154AC7] = "m_fThreshold";
        (*property_map)[0x4CA5B748] = "m_nAudioEmitterID";
        (*property_map)[0x40EB5161] = "m_aWaitingGuards";
        (*property_map)[0x6F2259DE] = "m_aFleeCandidates";
        (*property_map)[0x7F129A48] = "m_bHasFailed";
        (*property_map)[0x380D4827] = "m_triggerIfSpanning";
        (*property_map)[0xDB3D5286] = "m_aBoneScales";
        (*property_map)[0x8876B35A] = "m_rLinkedProxyEntity";
        (*property_map)[0x69DE9976] = "m_sBreadcrumbId";
        (*property_map)[0x56EC8F64] = "m_sMasterSwitchGroup";
        (*property_map)[0x1F30B8FA] = "m_sFootstepsSwitchGroup";
        (*property_map)[0x8C5979F1] = "m_sRigidbodyImpactsStaticSwitchGroup";
        (*property_map)[0xC18A9E19] = "m_fTangentHorizontalWeight";
        (*property_map)[0xF923C46E] = "m_sRigidbodyImpactsStaticSwitch";
        (*property_map)[0xD8881E5E] = "m_sRigidbodyImpactsSoliditySwitch";
        (*property_map)[0x6092629D] = "m_sBulletImpactsSwitch";
        (*property_map)[0xA8A190D5] = "m_rAudioSwitchWeaponCharacter";
        (*property_map)[0x39D3284F] = "m_rAudioSwitchWeaponClass";
        (*property_map)[0x899C910B] = "m_assistantApproachNode";
        (*property_map)[0x23E16BBE] = "m_rAudioSwitchWeaponFamily";
        (*property_map)[0xA0BA313B] = "m_bHideActorsUseBox";
        (*property_map)[0x14A9F9E] = "m_rAudioSwitchImpactType";
        (*property_map)[0xA85C610B] = "m_sOther";
        (*property_map)[0xC527E768] = "m_fExpireInterval";
        (*property_map)[0x7A94E80A] = "m_bValidCandidate";
        (*property_map)[0x9DBB630C] = "m_rTargetable";
        (*property_map)[0xF99D8B21] = "m_fVisualOffsetRadius";
        (*property_map)[0xF2BD7E55] = "m_aTipsConfigsForTypes";
        (*property_map)[0xCE3DEA55] = "m_bTransitionWhileColliding";
        (*property_map)[0x79082E59] = "m_bIgnoreProfileAlignment";
        (*property_map)[0xBDBF9549] = "m_eAnimSetBack";
        (*property_map)[0x29950C9C] = "m_nNumSteps";
        (*property_map)[0x97409DD3] = "m_bBottomEnabled";
        (*property_map)[0x18C9C1DC] = "m_DeadVIPsOrContractTargets";
        (*property_map)[0x385CA6E2] = "m_bTreatUnconsciousAsAlive";
        (*property_map)[0x38EF7AC0] = "m_bTreatHiddenUnconsciousAsDead";
        (*property_map)[0x9D28B4BB] = "m_bFlipNormal";
        (*property_map)[0x33255081] = "m_bPiercable";
        (*property_map)[0x88E8F3C6] = "m_fPierceExpansion";
        (*property_map)[0xF950163F] = "m_fScreenDecalsRefractionScale";
        (*property_map)[0x1B6EE2D3] = "m_curve";
        (*property_map)[0x4BC552C6] = "m_GridImportance";
        (*property_map)[0x91D8ACC3] = "m_AttachGlow0";
        (*property_map)[0x3F01B1CA] = "m_bUseTimer";
        (*property_map)[0xE72ED4A1] = "m_fImpactTriggerVelocity";
        (*property_map)[0xE41435B6] = "m_bShowProfileIndicator";
        (*property_map)[0x188B022] = "m_fImpactTriggerForce";
        (*property_map)[0x1A38845D] = "m_nHeightOffset";
        (*property_map)[0x19A1C512] = "m_bShowLoadingTips";
        (*property_map)[0x21E1961F] = "m_sRequest";
        (*property_map)[0x58689447] = "m_vColliderLinearVelocity";
        (*property_map)[0x640FB6B6] = "m_VisionConfiguration";
        (*property_map)[0xE1C6763C] = "m_iFullCircleTurnsRequired";
        (*property_map)[0xDB88523A] = "m_fInitCooldown";
        (*property_map)[0x7F4A070E] = "m_style";
        (*property_map)[0x293B3DA] = "m_nRepeat";
        (*property_map)[0xF7524E0] = "m_bLightOn";
        (*property_map)[0x757FBBED] = "m_sParameter";
        (*property_map)[0x1FC61165] = "m_PropagationSphereGrowthRate";
        (*property_map)[0xB6D81D51] = "m_MathLerpsData_SColorRGB";
        (*property_map)[0x9CE349D] = "m_nMaxShootingNPCs";
        (*property_map)[0xD9CC9D92] = "m_ShootingCooldown";
        (*property_map)[0x8B21FA9B] = "m_eventTracks";
        (*property_map)[0xDB3DC93A] = "m_fAddShooterCooldown";
        (*property_map)[0x1E5165C3] = "m_fMinimumTimeInPosition";
        (*property_map)[0x6E2914AB] = "m_fChangePositionMaxTime";
        (*property_map)[0xFE5AD437] = "m_usePreferredNextNodes";
        (*property_map)[0xC64E7E1C] = "m_bVIPHandlesCuriousInvestigations";
        (*property_map)[0x372615F1] = "m_pVideoDatabases";
        (*property_map)[0x9FA4C637] = "m_bShowEndgameMenu";
        (*property_map)[0xDF7E469C] = "m_eAxis";
        (*property_map)[0x23C71C53] = "m_nPos";
        (*property_map)[0xF937766D] = "m_pLabel";
        (*property_map)[0x30F2A9E7] = "m_fUniformScaleFactor";
        (*property_map)[0xBD7E833] = "m_WaypointList";
        (*property_map)[0x439BC82] = "m_LowVisibilityBits";
        (*property_map)[0xA4C44ECF] = "m_deadEndData";
        (*property_map)[0xAB9D30E0] = "m_bBlockEmptyGun";
        (*property_map)[0x53520FF1] = "m_bBlockGunShots";
        (*property_map)[0x2349B4CA] = "m_bBlockExplosions";
        (*property_map)[0x6F245F3C] = "m_bBlockThrownItemCollision";
        (*property_map)[0x1552E435] = "sKey";
        (*property_map)[0x636C91F] = "m_MathMultipliesData_Vector2";
        (*property_map)[0x7131F989] = "m_MathMultipliesData_Vector3";
        (*property_map)[0x86FC8BF4] = "m_ShotListenersData";
        (*property_map)[0xCD07624F] = "m_TrapsData";
        (*property_map)[0x6EB8C37F] = "m_ActorStandInEntitiesData";
        (*property_map)[0xC1E278DA] = "m_ActorBoneAttachmentsData";
        (*property_map)[0x7CFF1D46] = "m_ActorKeywordProxiesData";
        (*property_map)[0x55BEB5D9] = "m_LampCoreData";
        (*property_map)[0xA4AFC79B] = "m_VIPEvacuationNodesData";
        (*property_map)[0x81E120E2] = "m_BodyContainersData";
        (*property_map)[0x14F7B91C] = "m_MathLerpsData_float32";
        (*property_map)[0xCF0327F4] = "m_MathLerpsData_SColorRGBA";
        (*property_map)[0x1049B5CA] = "m_nGroupId";
        (*property_map)[0x9817FAAE] = "m_ActorPrimaryConfig";
        (*property_map)[0x18217319] = "m_HeroPrimaryConfig";
        (*property_map)[0x79549A39] = "m_HeroSecondaryConfig";
        (*property_map)[0x241930FE] = "m_aModelSkinSelections";
        (*property_map)[0x7B53CAA7] = "m_nModelScopeSelection";
        (*property_map)[0x489BF427] = "m_nModelMuzzleExtensionSelection";
        (*property_map)[0x5A7F480B] = "m_nModelMuzzleExtensionSkinSelection";
        (*property_map)[0x32C39130] = "m_nEffectFlashSelection";
        (*property_map)[0x84680089] = "m_bProjectVolumesToGround";
        (*property_map)[0x82F4053C] = "m_nAudioImpactType";
        (*property_map)[0xE2BBD090] = "m_mDynamicObjectIDGenerationValues";
        (*property_map)[0x2DB0A7E0] = "m_bIsCurrent";
        (*property_map)[0xDB96E34B] = "m_fDurability";
        (*property_map)[0x2166AB95] = "m_fMaxStrength";
        (*property_map)[0xED6BD09A] = "m_fMinStrength";
        (*property_map)[0x347C10AB] = "m_nStatePrevious";
        (*property_map)[0x476D0E05] = "m_fFractureStrength";
        (*property_map)[0x2683C9DD] = "m_fFractureShockAbsorption";
        (*property_map)[0x8806348E] = "m_MetricsValue";
        (*property_map)[0x6EA869DA] = "m_chance";
        (*property_map)[0xD3F89515] = "m_aReplicas";
        (*property_map)[0x801AA7C9] = "m_blocked";
        (*property_map)[0x88809CF2] = "toEntity";
        (*property_map)[0x14E660B4] = "m_eFlavor";
        (*property_map)[0x4A88CD36] = "m_aPageDefinitions";
        (*property_map)[0xCA4AAD28] = "m_bStandDownUnconscious";
        (*property_map)[0x89D9A6C8] = "m_bEscalateUnconscious";
        (*property_map)[0x79C79E15] = "m_fFadeTime";
        (*property_map)[0x8E74F25D] = "m_ModifierIDs";
        (*property_map)[0xD5F20616] = "m_rTitleTextResource";
        (*property_map)[0x27FECE71] = "m_sDefaultSwitch";
        (*property_map)[0x1FB2F29] = "m_fSeconds";
        (*property_map)[0xB0A0580D] = "m_bSendTotalSecondsOnChanged";
        (*property_map)[0x1C064D4E] = "m_aRoleEvents";
        (*property_map)[0xEB035146] = "m_aPrevSituationTypes";
        (*property_map)[0x1E260931] = "m_bPiPEnabled";
        (*property_map)[0xFD855FF5] = "m_fSmallGridSize";
        (*property_map)[0x4AEC8C7C] = "m_fHeadBoneAttacherFwdOffset";
        (*property_map)[0x599B657D] = "m_tInvestigatedSince";
        (*property_map)[0x8FFD5ED8] = "m_fGlobalLightBackIntensity";
        (*property_map)[0x929C236E] = "m_fGlobalLightPhong";
        (*property_map)[0x44DCE730] = "m_bDisableGlobalLightSpec";
        (*property_map)[0x8A82314F] = "m_nMaxMirrors";
        (*property_map)[0x8030070D] = "m_bRequestCameraRecenter";
        (*property_map)[0x33E1E3B] = "m_fRotationOffsetYaw";
        (*property_map)[0xC46CFC69] = "m_MovementData";
        (*property_map)[0xAA64F991] = "m_bLethalAgilityElementUsed";
        (*property_map)[0xAD2A58ED] = "m_bIsChangingClothes";
        (*property_map)[0x3AF01560] = "m_ContainedItems";
        (*property_map)[0xF3FBC0F7] = "m_aActorsSaveData";
        (*property_map)[0x70B305A] = "m_rStartBehaviour";
        (*property_map)[0x4E6382A0] = "m_bWereSubtitlesSeen";
        (*property_map)[0x7F9965E0] = "m_rSoundEvent_Help";
        (*property_map)[0x53C0897B] = "m_rSoundEvent_HelpLoud";
        (*property_map)[0xBFDC351B] = "m_rSoundEvent_Angry";
        (*property_map)[0x6A457B6A] = "m_rSoundEvent_Pain";
        (*property_map)[0xD92DF7A6] = "m_rSoundEvent_Pain_Behind";
        (*property_map)[0xCD4558CC] = "m_rSoundEvent_Warning";
        (*property_map)[0x6958F064] = "m_bUndefined";
        (*property_map)[0xE61D1B64] = "play_chance";
        (*property_map)[0x75B66685] = "m_bUseParentSpace";
        (*property_map)[0x42DFEDD] = "m_bSearching";
        (*property_map)[0x404C5A51] = "m_bArrest";
        (*property_map)[0x7D4FAF80] = "m_EmitterSource";
        (*property_map)[0xB68E3C38] = "m_nAttacksForTakeDown";
        (*property_map)[0x4F5F9519] = "m_fTakeDownWindow";
        (*property_map)[0x401FAB50] = "m_NearBackOccluders";
        (*property_map)[0xB6A818D8] = "m_bAllowAttackingHitman";
        (*property_map)[0x5D56109B] = "m_nKillMashNum";
        (*property_map)[0x6EBC4523] = "m_bBooleanValue";
        (*property_map)[0x977AC643] = "m_nNumChainsToKill";
        (*property_map)[0xC98ACB24] = "m_eActorCCPreset";
        (*property_map)[0xA2D8DB2] = "m_fBlendInTime";
        (*property_map)[0x146F3EA3] = "ref";
        (*property_map)[0x8F854663] = "m_fBlendSustainDuration";
        (*property_map)[0xF693D3A] = "m_nParameterId";
        (*property_map)[0x53C9CEF0] = "m_bGaussianBlurEnabled";
        (*property_map)[0x53A2EB4B] = "m_fGaussianBlurriness";
        (*property_map)[0x299E5452] = "m_fRadialBlurFadeFactor";
        (*property_map)[0x735B3D5B] = "m_bColorCorrectionEnabled";
        (*property_map)[0x206BE644] = "BillboardTexture";
        (*property_map)[0x8F1146BE] = "ColorCorrectionTextureHDR";
        (*property_map)[0x44B73AF1] = "m_fDepthOfFieldFocusEnd";
        (*property_map)[0xC25C5769] = "m_fDepthOfFieldFocalFadeFar";
        (*property_map)[0x4B4AC6BC] = "m_eBlurQuality";
        (*property_map)[0x8C8FC950] = "m_fDepthOfFieldFocalDistance";
        (*property_map)[0x93515020] = "m_fDepthOfFieldAperture";
        (*property_map)[0xFA8610E4] = "m_fLabsDofAnamorphic";
        (*property_map)[0x893E4F34] = "m_bActivateAimAssistanceOnStart";
        (*property_map)[0xA8563F5F] = "m_bDistortionBarrelEnabled";
        (*property_map)[0xB9DE2CFC] = "m_order";
        (*property_map)[0x394E1775] = "m_fDoubleVisionEyeSeparation";
        (*property_map)[0xBA68DA43] = "m_eBurstPattern";
        (*property_map)[0x8D7C062A] = "m_fDoubleVisionSpeed1";
        (*property_map)[0x14755790] = "m_fDoubleVisionSpeed2";
        (*property_map)[0xA4FF2C69] = "m_fDoubleVisionScaleSpeed2";
        (*property_map)[0x1DFB160A] = "m_fDoubleVisionMinScale";
        (*property_map)[0xFB9BB9EB] = "m_fDoubleVisionMaxScale";
        (*property_map)[0xC884DB70] = "m_bFogEnabled";
        (*property_map)[0xF75630FB] = "m_GlowBackgroundOutlineColor";
        (*property_map)[0x1E4B9B7C] = "m_bCreateSleeveConstrains";
        (*property_map)[0x4745C3C1] = "m_bFogGlobal";
        (*property_map)[0xE80C798C] = "m_fFogDensity";
        (*property_map)[0x7B08F1EE] = "m_HDRAdaptionMiddleGrayCurveEntity";
        (*property_map)[0xECEA673A] = "m_fHDRExposureMeteringSize";
        (*property_map)[0xB6058DCE] = "m_eHDRToneMapType";
        (*property_map)[0x45193869] = "m_bDebugSelectedActor";
        (*property_map)[0x8D650738] = "m_fHDRBloomScale";
        (*property_map)[0xB83E4729] = "m_fHDRStarScale";
        (*property_map)[0xB0DAEBFD] = "m_vHDRBloomMinFalloffDistanceMinMax";
        (*property_map)[0xB4C79998] = "m_fHDRLensFlareAttenuation";
        (*property_map)[0x32756AD7] = "m_fMinimumDisplayTime";
        (*property_map)[0xE8B7D28] = "m_fLevelSetupMultiplier";
        (*property_map)[0x42916D7A] = "m_fHDRLensFlareThresholdD";
        (*property_map)[0x5F36AE9D] = "m_vSSAOStrengthNearFar";
        (*property_map)[0x277FEF8C] = "m_vSSAODistanceNearFar";
        (*property_map)[0xC0D5D5EA] = "m_fSSAOOcclusionRange";
        (*property_map)[0xE90D317B] = "m_vSSAOScreenSpaceRadiusLimits";
        (*property_map)[0xF53ED6A6] = "m_fSSRGlossFadeIn";
        (*property_map)[0x1791D58B] = "m_fSSRGlossFadeOut";
        (*property_map)[0x2E558AE5] = "m_fSSRGlossRoughThrd";
        (*property_map)[0x122DC28F] = "m_nSSRNumStepsRough";
        (*property_map)[0x1DE24D8C] = "m_nSSRNumStepsGloss";
        (*property_map)[0x998978F] = "m_pVROldHeadReplacement";
        (*property_map)[0x68379B3F] = "m_vVignetteGradientLightColor";
        (*property_map)[0xD3D64E19] = "m_bDefaultIntroMovieIsSkippable";
        (*property_map)[0xAECF4209] = "m_pSecondIntroMovie";
        (*property_map)[0xBCA90C6C] = "m_bAnchorEnd";
        (*property_map)[0x149C1F68] = "m_pDefaultLoopingMovie";
        (*property_map)[0xB1EFCED1] = "m_PenaltyRangeInvestigateCautious";
        (*property_map)[0xA815505E] = "m_pGlobalTextList";
        (*property_map)[0x8D5284DC] = "animation";
        (*property_map)[0x4D2AC09C] = "m_bCompareWith";
        (*property_map)[0x154E279F] = "m_eAIEventType";
        (*property_map)[0x56D56323] = "m_nAIEventRange";
        (*property_map)[0xDF4BECE6] = "m_nRequiredCrowdRadius";
        (*property_map)[0x87964CFD] = "m_nAIEventTime";
        (*property_map)[0xEF2BA55F] = "fMinAge";
        (*property_map)[0x58710E18] = "m_fMinRandPower";
        (*property_map)[0x39F64E20] = "m_rOutfitRepositoryID";
        (*property_map)[0x730C3DE2] = "m_pBoundingVolume";
        (*property_map)[0x7AEED2F9] = "m_vFarAttenuation";
        (*property_map)[0x7DB7D225] = "m_nPixelsPerMeter";
        (*property_map)[0x35DDB0C7] = "m_aForceIncludeMarkers";
        (*property_map)[0xC4D18054] = "m_aAdditionalMapSegments";
        (*property_map)[0x844C33A9] = "m_aLookFromEntities";
        (*property_map)[0xCCD2E017] = "m_nMaxEntitiesPerRow";
        (*property_map)[0x9CC8FBF4] = "m_aLookAtTargets";
        (*property_map)[0x4DCA1CFA] = "m_GlowContractTargetUnconsciousOutlineColor";
        (*property_map)[0x19F810E0] = "m_aCharsets";
        (*property_map)[0x83033B9] = "m_knownByActors";
        (*property_map)[0xFAEEE824] = "m_rMaleSequences";
        (*property_map)[0x367A806] = "m_rFemaleSequences";
        (*property_map)[0xB80AC610] = "m_Entries";
        (*property_map)[0xA9238A5E] = "m_fExtraPickupMagazines";
        (*property_map)[0x7E2506C1] = "m_fStateTimer";
        (*property_map)[0xB4B67245] = "m_nTargetLocationIndex";
        (*property_map)[0xBABCAFE9] = "m_eVariationResourceMaxTension";
        (*property_map)[0xEA30087] = "m_aPreOrders";
        (*property_map)[0xB9A38D1D] = "m_aOrders";
        (*property_map)[0x3A3202F6] = "m_rCurrentDrama";
        (*property_map)[0x14B00DD1] = "m_HolderSaveableId";
        (*property_map)[0xD643B620] = "m_KeywordID";
        (*property_map)[0x73245572] = "m_bEnableAngularVelocityScale";
        (*property_map)[0x41CAD6B] = "m_fAngularVelocityBlendRange";
        (*property_map)[0xA0C77B1F] = "m_fFrustumFarDistanceProperty";
        (*property_map)[0x98622E3D] = "m_fSkirtConstraintOffset";
        (*property_map)[0xB2E8F0CE] = "m_fSkirtConstraintFraction";
        (*property_map)[0xF33F51FB] = "m_fSleeveConstraintOffset";
        (*property_map)[0x1DB8AAA7] = "m_bBreakAfter";
        (*property_map)[0x51FDC792] = "m_fFormationDistance";
        (*property_map)[0x7F1970CC] = "m_fSeekStrength";
        (*property_map)[0x6D4B45FB] = "WEAPONS";
        (*property_map)[0xED67C09C] = "m_rTestGroupPath";
        (*property_map)[0x9C855559] = "m_testActors";
        (*property_map)[0xE46AF636] = "m_pArgs";
        (*property_map)[0xB60AAA9D] = "m_bAddToHistory";
        (*property_map)[0x49D205A] = "m_bBackward";
        (*property_map)[0xBB28A45B] = "m_bClearCurrentPageContext";
        (*property_map)[0xC573E071] = "m_rEscortingMember";
        (*property_map)[0xF53668A2] = "m_vEscortOutExitLocation";
        (*property_map)[0xC0765566] = "m_bDisableNavmeshExport";
        (*property_map)[0x164A2BFD] = "ITEMS";
        (*property_map)[0x51AA9607] = "m_nNextGroupID";
        (*property_map)[0xC2DEED47] = "m_sExtraDataTypeName";
        (*property_map)[0xBB9213E5] = "m_Axis";
        (*property_map)[0xA2565BCE] = "m_bOn";
        (*property_map)[0x53F43982] = "m_fRampSpeed";
        (*property_map)[0x73D350A] = "m_bPacify";
        (*property_map)[0x98F87564] = "m_eCharacterType";
        (*property_map)[0xCDAEE415] = "m_bDebugKnowledge";
        (*property_map)[0x38D04E16] = "m_bDebugKnowledgeDumpEvents";
        (*property_map)[0x77CAFC52] = "m_eAISharedEventType";
        (*property_map)[0x340C3A3B] = "m_bDebugMoveOrder";
        (*property_map)[0x21F29842] = "m_bDebugBehavior";
        (*property_map)[0x134B4A1D] = "m_bDebugPerception";
        (*property_map)[0x54171B42] = "m_bDebugApplyNPCDialogSwitch";
        (*property_map)[0xB7655CAA] = "m_eDialogEntry";
        (*property_map)[0x599E4462] = "m_eAIEventInject";
        (*property_map)[0x6D2B7A4F] = "m_bDebugGeoTag";
        (*property_map)[0xD3F32C8F] = "m_nInitialStateIndex";
        (*property_map)[0x24BF2A57] = "m_pVolume";
        (*property_map)[0xABAA85A3] = "m_bRequiresFemale";
        (*property_map)[0xB94DFE8C] = "m_nMaxNeighbours";
        (*property_map)[0x3CFAA708] = "m_fHealthPercentTimeout";
        (*property_map)[0xF3191524] = "m_fIsolationDistance";
        (*property_map)[0x22E23C78] = "m_aLocoVariationIndecies";
        (*property_map)[0x325F8D96] = "m_aTrackers";
        (*property_map)[0xC7616DEC] = "m_rReceiver";
        (*property_map)[0xA2B69009] = "m_vEndPoint";
        (*property_map)[0xC05DEFFD] = "m_bUseTransformYAsTangents";
        (*property_map)[0xDFF971DA] = "m_fLineThickness";
        (*property_map)[0x381D6575] = "m_bArrowEnd";
        (*property_map)[0xB837F90C] = "m_aReactAnimDBs";
        (*property_map)[0x727FB664] = "m_resourceID";
        (*property_map)[0xB8E950A4] = "m_rInstance";
        (*property_map)[0x9EABAB2] = "m_aTagBlacklist";
        (*property_map)[0xD9B832BC] = "m_aParents";
        (*property_map)[0xB37D6235] = "m_nAimReactionDelay";
        (*property_map)[0xE5B0E7AB] = "m_nHMCCReactionDelay";
        (*property_map)[0x5C775285] = "m_pCCReaction";
        (*property_map)[0x46B60B32] = "m_nGlowEnemiesFadeSpeed";
        (*property_map)[0x249BEC90] = "m_nHMInCoverReactionDelay";
        (*property_map)[0x9DF0FA85] = "m_bCastOmniShadows";
        (*property_map)[0xDF8AF574] = "m_pPushedReaction";
        (*property_map)[0x9CB17A3E] = "m_pNPCCivilianAlertedReaction";
        (*property_map)[0xC30F3CD] = "m_eCollisionPairType";
        (*property_map)[0x3AF39809] = "m_aTargetableBones";
        (*property_map)[0xBEA8CA4C] = "m_pNPCScaredReaction";
        (*property_map)[0xFCF117D7] = "m_pNPCCivilianScaredReaction";
        (*property_map)[0x35835298] = "m_pNPCDeadReaction";
        (*property_map)[0x6E2A6F6B] = "m_pAIShootingReaction";
        (*property_map)[0x4D797FA8] = "m_bShotIntoCrowdEnabled";
        (*property_map)[0xECA3FA0D] = "m_fGlowBackgroundUnmaskedOutlineWidth";
        (*property_map)[0xABC4D4B5] = "m_pShotIntoCrowd";
        (*property_map)[0x90CB052A] = "m_pDeathReaction";
        (*property_map)[0x4E1791AF] = "bIsVisible";
        (*property_map)[0x6B9DBF64] = "m_bAccidentReactionEnabled";
        (*property_map)[0xE42C4F03] = "m_bPacifiedReactionEnabled";
        (*property_map)[0x28386D25] = "m_pPacifiedReaction";
        (*property_map)[0xF4942839] = "m_bExplosionEnabled";
        (*property_map)[0xD1C0071E] = "m_nNumScaredBeforeWarzone";
        (*property_map)[0xF7B3B4DA] = "m_nNumShotsBeforeWarzone";
        (*property_map)[0xF0A8AFEB] = "m_eIconType";
        (*property_map)[0x5F9173C5] = "m_bStopCurrentActFast";
        (*property_map)[0xB623FBA] = "m_bCalculateRestPose";
        (*property_map)[0x8B57FCA9] = "m_pVideo";
        (*property_map)[0x54511224] = "m_IgnoreGuards";
        (*property_map)[0x6DBE3839] = "m_IgnoreCivilians";
        (*property_map)[0x74E0D2F8] = "m_bEnableOnStart";
        (*property_map)[0x3B0E2139] = "m_bAlignActor";
        (*property_map)[0xDD48D2E9] = "m_bEnableDefault";
        (*property_map)[0x58823549] = "m_pGuard";
        (*property_map)[0x3DB2DBFC] = "m_fTriggerRadius";
        (*property_map)[0x7EE3753D] = "m_vDefaultAlignOffset";
        (*property_map)[0x22D67DB0] = "m_fReverseRadius";
        (*property_map)[0x44F00F9F] = "m_fReverseAlignAngle";
        (*property_map)[0x595FF39] = "m_fReverseActivationArc";
        (*property_map)[0x5360B6E1] = "m_vReverseAlignOffset";
        (*property_map)[0xC716BD33] = "m_pDefaultAct";
        (*property_map)[0xDE03C7F] = "m_fAnalogShootThreshold";
        (*property_map)[0x1FC89816] = "m_eSilenceRating";
        (*property_map)[0x5E1B3572] = "m_eCrosshairType";
        (*property_map)[0x9F01D76F] = "m_fCrosshairModifierVisualOffset";
        (*property_map)[0x8750C4D6] = "COLLISION_VOLUME_HITMAN_ON";
        (*property_map)[0x3B631B9C] = "m_iBulletsPerMagazine";
        (*property_map)[0xF07DBB21] = "m_nProjectilesPerShot";
        (*property_map)[0x6B7875D7] = "m_bAllowPrecisionShot";
        (*property_map)[0xCC353A93] = "m_bAllowPrecisionTimeSlowdown";
        (*property_map)[0x2988B29B] = "m_fPrecisionShotDuration";
        (*property_map)[0x57810502] = "m_fPrecisionMinDegree";
        (*property_map)[0x806AB743] = "m_fPrecisionMaxDegree";
        (*property_map)[0x4A2E5694] = "m_fPrecisionRecoverySpeed";
        (*property_map)[0xE2CD82CE] = "m_fStandMovingPrecisionModifier";
        (*property_map)[0x6CECB0E6] = "m_fScopeModePrecisionDegree";
        (*property_map)[0x1CA3F768] = "m_aScopeZoom";
        (*property_map)[0x4D012BD8] = "m_vBobbingAngleX";
        (*property_map)[0x5B8C2BF4] = "m_vBobbingFrequencyX";
        (*property_map)[0x2C8B1B62] = "m_vBobbingFrequencyY";
        (*property_map)[0x832539C6] = "m_fDragCorrectionFraction";
        (*property_map)[0x2292BA22] = "m_fZRecoilDistance";
        (*property_map)[0x8C4C0DCA] = "m_fZRecoilTime";
        (*property_map)[0x4243E13D] = "m_fCounterRotationFactor";
        (*property_map)[0xBE6F921] = "m_fPulseDecayTime";
        (*property_map)[0x54D8E510] = "m_fPulseDecayFraction";
        (*property_map)[0x26112A4F] = "m_fPulseRun";
        (*property_map)[0x4BE7A2FA] = "m_fPulseWalkPulse";
        (*property_map)[0xC6A64150] = "m_fRecoilX";
        (*property_map)[0x7FE48EA8] = "m_bAutoCalculateMaxDistance";
        (*property_map)[0xD7402A28] = "m_fRecoilVertialRange";
        (*property_map)[0xAC51972B] = "m_fRecoilDelay";
        (*property_map)[0xC26C3A78] = "m_fRecoilMax";
        (*property_map)[0xB180A346] = "m_bDelayInvestigateDialog";
        (*property_map)[0xD2E2A3DD] = "m_fRangeNear";
        (*property_map)[0xB2AEB326] = "m_RifleHolsterAttacher";
        (*property_map)[0xFAE2D746] = "m_fRangeMedium";
        (*property_map)[0xBFAAD951] = "m_fBurstRandomizationFactor";
        (*property_map)[0x1C5771A8] = "m_fBurstAngleH";
        (*property_map)[0xE6584CCB] = "m_fBurstAngleV";
        (*property_map)[0xAA755CBD] = "m_fBodyPartModifierFace";
        (*property_map)[0x892D146] = "m_fBodyPartModifierHead";
        (*property_map)[0xF362E83D] = "m_fBodyPartModifierArm";
        (*property_map)[0x3433D7BF] = "m_nGlowContractNonCriticalFadeSpeed";
        (*property_map)[0xB95A0E4E] = "m_fDamageNear";
        (*property_map)[0xDD71E240] = "m_fDamageFar";
        (*property_map)[0xCCC95D47] = "m_ShaderColor2";
        (*property_map)[0xB8FF6A64] = "m_fHitsNumberToFall";
        (*property_map)[0xB9617D9D] = "m_fHitsNumberTimeout";
        (*property_map)[0x6F1E17C1] = "m_pImpactEffect";
        (*property_map)[0xBE0B2173] = "m_pDeathImpactEffect";
        (*property_map)[0x6E72A8C1] = "header";
        (*property_map)[0x393D1D29] = "m_bParticleFxUseLockedCenter";
        (*property_map)[0x2B36786B] = "title";
        (*property_map)[0x89827967] = "m_bInverseHorizontalPolarity";
        (*property_map)[0x6D552BBD] = "m_bInverseVerticalPolarity";
        (*property_map)[0xEA40CC89] = "m_ScaleContext";
        (*property_map)[0x2794DC58] = "m_IsVictimDisposable";
        (*property_map)[0xF8CDD79] = "m_IsBystanderGuardsSearching";
        (*property_map)[0x4ABD2038] = "m_Resource";
        (*property_map)[0x4E28B5EF] = "m_bCheatSpawnGroup_Tools";
        (*property_map)[0x4C757137] = "m_bCheatSpawnGroup_Everything";
        (*property_map)[0xF4ABA63F] = "m_eCheatGroup";
        (*property_map)[0xB768C04] = "m_bIsFrameUpdateRegistered";
        (*property_map)[0x2A51C227] = "m_BulletImpactInvestigation";
        (*property_map)[0xE1FEFF56] = "m_SetPieceInvestigation";
        (*property_map)[0xE4893BD0] = "m_bIsNegativeFogBox";
        (*property_map)[0x50BA76FF] = "m_nNumWires";
        (*property_map)[0x3E6AFCB9] = "m_fWireLengthRandomFactor";
        (*property_map)[0xD5084C2A] = "m_nNumRenderSegments";
        (*property_map)[0x5DF08B58] = "m_fWireMaterialLength";
        (*property_map)[0x84B6247B] = "m_bTexCoordsFromPivot";
        (*property_map)[0x7C5130BB] = "m_fLongFrequency";
        (*property_map)[0x684E9AB] = "m_VertexColor";
        (*property_map)[0x9862ADA2] = "m_fMaterialOffsetInterval";
        (*property_map)[0x2CD36771] = "m_rAnchorTransform";
        (*property_map)[0x60295934] = "m_bDetachOnLengthExceed";
        (*property_map)[0x210C2597] = "m_nLODCutOff";
        (*property_map)[0x1DA2F7F1] = "m_fTimeSinceLossOfSight";
        (*property_map)[0x2AE72498] = "m_MainItemID";
        (*property_map)[0x4B5C91D7] = "m_GlowContractTargetNonCriticalUnconsciousColor";
        (*property_map)[0xDD3048CA] = "m_rTargetGroup";
        (*property_map)[0x69AF1EF9] = "m_aTargetGroups";
        (*property_map)[0x9D9C1AD9] = "m_EntitiesData";
        (*property_map)[0xB1942DE8] = "m_fMaxOcclusion";
        (*property_map)[0xA3D0B944] = "m_fMinMaxOcclusionDistance";
        (*property_map)[0xF351EBF4] = "m_fScreenDecalsTTL";
        (*property_map)[0xA7DB8DB8] = "m_vForceOffset";
        (*property_map)[0xBC876D18] = "m_bTimerEnabled";
        (*property_map)[0xE9DD2120] = "m_nApplyInterval";
        (*property_map)[0xB6A146B0] = "m_vForce";
        (*property_map)[0x3F73AE4F] = "m_vTorque";
        (*property_map)[0x4723983E] = "m_Morpheme";
        (*property_map)[0xD795CA9A] = "m_bSwitchState";
        (*property_map)[0xC448C54A] = "m_nDlcIndex";
        (*property_map)[0xB5E1D9A4] = "m_fScreenDecalsSettleTime";
        (*property_map)[0x21F319A6] = "m_fScreenDecalsScaleTime";
        (*property_map)[0xC6491797] = "m_fScreenDecalsOpacity";
        (*property_map)[0x4BE8B478] = "m_fScreenDecalsBlurrinessScale";
        (*property_map)[0x1104F835] = "m_ScreenDecalsColor";
        (*property_map)[0xC3F87B07] = "m_vScreenDecalsTiles";
        (*property_map)[0x71BEBB63] = "m_vScreenDecalsSingleSize";
        (*property_map)[0x558A7038] = "m_vScreenDecalsPairASize";
        (*property_map)[0x21C3AE6A] = "m_pSatelliteMap";
        (*property_map)[0xB7A68E18] = "iInteraction2";
        (*property_map)[0xE1F2723E] = "m_pNavigationalBoundsOverride";
        (*property_map)[0xD9F8EFF2] = "m_bBlendOutImmediatelyUponTimeout";
        (*property_map)[0x9B86568E] = "m_iStoredInventorySlotIndex";
        (*property_map)[0xEFEA4318] = "m_fNonFlickerPower";
        (*property_map)[0x9D96CDD7] = "m_fFadeInInterval";
        (*property_map)[0x6F5BEA11] = "m_fFadeOutInterval";
        (*property_map)[0xBA89105D] = "m_fShortOffset";
        (*property_map)[0xC0F4CF2C] = "m_fShortFrequency";
        (*property_map)[0x6FA26343] = "m_DistanceReferenceBone";
        (*property_map)[0xBBACA57F] = "m_fNewColorInterval";
        (*property_map)[0x823D8D42] = "m_bUpdateOnSet";
        (*property_map)[0x5A95C5E7] = "m_nTargetSharedEntity";
        (*property_map)[0x44C77E7D] = "m_aMemberData";
        (*property_map)[0x26F7FE7A] = "m_rPivot";
        (*property_map)[0xC16E2C08] = "m_fDamageMediumBonus";
        (*property_map)[0xE03874F5] = "m_bReportingToHitman";
        (*property_map)[0xCE2948F5] = "m_bMultipleBodies";
        (*property_map)[0x8FDC4F63] = "m_fBlendInDuration";
        (*property_map)[0x8EB4EB28] = "m_bGetHelpOrderCompleted";
        (*property_map)[0x74779A3] = "m_tGetHelpCompleted";
        (*property_map)[0x509D9B25] = "m_eSmoothingMode";
        (*property_map)[0xB9C717F1] = "m_rActorIKController";
        (*property_map)[0x682624F0] = "m_pDialogueAudioEmitter";
        (*property_map)[0x96934626] = "m_Animator";
        (*property_map)[0xFAF7DFCF] = "ActivityDatastoreRid";
        (*property_map)[0x86AC6CCD] = "m_rRagdollEntity";
        (*property_map)[0x12070B7D] = "m_rReactionNetwork";
        (*property_map)[0x1ED19297] = "m_bPreferredInvestigator";
        (*property_map)[0x1D9A5320] = "m_eHUDElement2";
        (*property_map)[0x6A9D63B6] = "m_eHUDElement3";
        (*property_map)[0xA29708C5] = "m_InitialOutfitId";
        (*property_map)[0xD98A8B15] = "m_placementconfiguration";
        (*property_map)[0x4FB0733A] = "m_bStreamableDisguiseGive";
        (*property_map)[0xBFA99717] = "m_CharacterId";
        (*property_map)[0xB94806EC] = "m_SeasonOneHead2";
        (*property_map)[0x272C934F] = "m_SeasonOneHead5";
        (*property_map)[0x7C2AAD7B] = "m_pGuideController";
        (*property_map)[0x1A0E2911] = "m_pIKController";
        (*property_map)[0x4A7F51CF] = "m_pCheatController";
        (*property_map)[0xDBDDCED1] = "m_pActorTagController";
        (*property_map)[0x8389238A] = "m_nTrakingPlayersMax";
        (*property_map)[0xBE1859FA] = "m_bIsSittingBench";
        (*property_map)[0xF2F1334D] = "m_fMinDistanceWorldSpace";
        (*property_map)[0x6EC8AFCA] = "m_fMaxDistanceAimSpace";
        (*property_map)[0x19BA6B89] = "m_fSnapToLocationSpeed";
        (*property_map)[0xEA3B1207] = "m_TimeToPulse";
        (*property_map)[0x2C7D0E8B] = "m_fSnapToLocationDuration";
        (*property_map)[0x1B3F887] = "m_fMaximumTrackingDistance";
        (*property_map)[0x289755A0] = "m_fMinimumSpeedMultiplier";
        (*property_map)[0xB0FE6D2C] = "m_fAssistanceAmount";
        (*property_map)[0x2B11DAA] = "m_fStationaryTargetCameraMovementThreshold";
        (*property_map)[0xB58FA68C] = "m_nSeekPosition";
        (*property_map)[0xB9CA5390] = "m_nPlayState";
        (*property_map)[0x70D57856] = "m_RecenterHintTime";
        (*property_map)[0x55653B4F] = "m_FadeToBlackTime";
        (*property_map)[0xAC904774] = "m_HeightThreshold";
        (*property_map)[0x2A4D1F17] = "m_GhostFadeSpeed";
        (*property_map)[0xF79624B7] = "m_rPeekTransitionSequence";
        (*property_map)[0xE7F8F461] = "m_GhostedOpacity";
        (*property_map)[0x7A97580] = "m_ClippingAvoidanceDistance";
        (*property_map)[0xA783ECB1] = "m_ChestAvoidanceDistanceSneaking";
        (*property_map)[0x52CAD858] = "m_ChestAvoidanceFadeActivationAngle";
        (*property_map)[0x6B8DB899] = "m_fCharacterWetnessSpecularMul";
        (*property_map)[0x3CFB6971] = "m_ChestAvoidanceFadeDeactivationAngle";
        (*property_map)[0x879B0FBC] = "m_AngleReferenceBone";
        (*property_map)[0xCD7A54FB] = "m_bAlternativeIntensity";
        (*property_map)[0x22C73C6B] = "m_ShaderColor0";
        (*property_map)[0x55C00CFD] = "m_ShaderColor1";
        (*property_map)[0xBBCE6DD1] = "m_ShaderColor3";
        (*property_map)[0x52ADC8E4] = "m_ShaderColor5";
        (*property_map)[0xCBA4995E] = "m_ShaderColor6";
        (*property_map)[0xFCC54775] = "m_GlowEnemiesColor";
        (*property_map)[0x4B9EDCB5] = "m_GlowEnemiesUnconsciousOutlineColor";
        (*property_map)[0x8E5D0C74] = "m_fGlowEnemiesOutlineWidth";
        (*property_map)[0xFCE7C0A0] = "m_GlowAlliesColor";
        (*property_map)[0x1A1898BF] = "m_GlowAlliesOutlineColor";
        (*property_map)[0x9FA09E16] = "m_GlowAlliesUnconsciousOutlineColor";
        (*property_map)[0x3BFA3DF] = "m_fGlowAlliesOutlineWidth";
        (*property_map)[0xE14F43AD] = "m_vGlowAlliesVignetteRadii";
        (*property_map)[0xECEFF170] = "m_vGlowAlliesFadeNearFar";
        (*property_map)[0xA2AA6ED9] = "m_GlowCivilianOutlineColor";
        (*property_map)[0x7F011E02] = "m_fGlowCivilianOutlineWidth";
        (*property_map)[0xF217C6E9] = "m_nGlowGunsFadeSpeed";
        (*property_map)[0xF0209A1F] = "m_vGlowGunsVignetteRadii";
        (*property_map)[0x240C431E] = "m_GlowStashedGunsColor";
        (*property_map)[0xCFF60338] = "m_vGlowStashedGunsVignetteRadii";
        (*property_map)[0x1FDA1737] = "m_vGlowSetPieceVignetteRadii";
        (*property_map)[0xA77909DB] = "m_vGlowSetPieceFadeNearFar";
        (*property_map)[0x3D0B3679] = "m_fGlowSetPieceAlternate";
        (*property_map)[0x149796D3] = "m_GlowBackgroundColor";
        (*property_map)[0x6314BBE0] = "m_nGlowBackgroundFadeSpeed";
        (*property_map)[0x7B608BFE] = "m_vGlowBackgroundVignetteRadii";
        (*property_map)[0xF78919D1] = "m_fGlowContractTargetOutlineWidth";
        (*property_map)[0xB1E8C862] = "m_fGlowContractAlternate";
        (*property_map)[0x2D4CACC1] = "m_nGridSizeX";
        (*property_map)[0x7D09CBB1] = "m_fGlowContractTargetNonCriticalOutlineWidth";
        (*property_map)[0x2BF4493A] = "m_vGlowContractNonCriticalFadeNearFar";
        (*property_map)[0xAFDC2FF2] = "m_fGlowContractNonCriticalAlternate";
        (*property_map)[0x4A7B9547] = "m_bDrawPush";
        (*property_map)[0x7794A3A1] = "m_GlowContractTargetSpecialColor";
        (*property_map)[0xB62545DA] = "m_GlowContractTargetSpecialOutlineColor";
        (*property_map)[0x365075D3] = "m_GlowContractTargetSpecialUnconsciousOutlineColor";
        (*property_map)[0xEB09BD12] = "m_GlowLTMemoryUnconsciousColor";
        (*property_map)[0x918621C7] = "m_fGlowContractTargetSpecialOutlineWidth";
        (*property_map)[0xCC286440] = "m_vGlowContractTargetSpecialFadeNearFar";
        (*property_map)[0xA6DA0387] = "m_GlowObjectivesColor";
        (*property_map)[0x41EB3BFA] = "m_fGlowObjectivesOutlineWidth";
        (*property_map)[0xCE3D6D9F] = "m_vGlowObjectivesVignetteRadii";
        (*property_map)[0x991CFFCC] = "m_fGlowObjectivesAlternate";
        (*property_map)[0x999C661D] = "m_GlowEnforcerUnconsciousColor";
        (*property_map)[0x160592C3] = "m_GlowEnforcerUnconsciousOutlineColor";
        (*property_map)[0x196E5EF2] = "m_nGlowEnforcerFadeSpeed";
        (*property_map)[0xA2D24003] = "m_vGlowEnforcerVignetteRadii";
        (*property_map)[0xD29EC866] = "m_vGlowEnforcerFadeNearFar";
        (*property_map)[0x98C7C098] = "m_GlowLTMemoryOutlineColor";
        (*property_map)[0xCD896B94] = "m_vGlowLTMemoryVignetteRadii";
        (*property_map)[0x5FBA0EE3] = "m_bDifficulty_Level1";
        (*property_map)[0x80DE21A3] = "m_vGlowLTMemoryFadeNearFar";
        (*property_map)[0x5FB49D55] = "m_GlowTaggedColor";
        (*property_map)[0x6A36354B] = "m_GlowTaggedUnconsciousColor";
        (*property_map)[0x725E0A69] = "m_GlowTaggedUnconsciousOutlineColor";
        (*property_map)[0xA95B4EEB] = "m_vGlowTaggedVignetteRadii";
        (*property_map)[0x43BF1973] = "m_GlowTagFocusTaggedColor";
        (*property_map)[0xF8701646] = "m_GlowTagFocusTaggedOutlineColor";
        (*property_map)[0x8AC8C98B] = "m_vGlowTagFocusTaggedFadeNearFar";
        (*property_map)[0x4E37E869] = "m_GlowTagFocusUntaggedUnconsciousColor";
        (*property_map)[0x108DD5D4] = "m_nGlowTagFocusUntaggedFadeSpeed";
        (*property_map)[0x150F3662] = "m_GlowBackgroundUnmaskedColor";
        (*property_map)[0x31826EB5] = "m_GlowBackgroundUnmaskedOutlineColor";
        (*property_map)[0x72BBA117] = "m_GlowInteractionColor";
        (*property_map)[0xCC07D6AC] = "m_GlowInteractionOutlineColor";
        (*property_map)[0x1E128C2B] = "m_pCollider";
        (*property_map)[0xF41AB68B] = "m_vGlowInteractionVignetteRadii";
        (*property_map)[0x9576FD9E] = "m_Firearm_widget";
        (*property_map)[0x511E53AC] = "m_vGlowInteractionFadeNearFar";
        (*property_map)[0xF072C64C] = "m_GlowInteractionSelectedColor";
        (*property_map)[0x28A4EBD] = "m_vGlowInteractionSelectedFadeNearFar";
        (*property_map)[0x795FABF9] = "m_GlowPlayerInLVAColor";
        (*property_map)[0x46AFFD25] = "m_nGlowPlayerInLVAFadeSpeed";
        (*property_map)[0x3C35B613] = "m_vGlowPlayerInLVAVignetteRadii";
        (*property_map)[0xCE95DA42] = "m_GlowPlayerSeenInLVAOutlineColor";
        (*property_map)[0xC9300B97] = "m_vGlowPlayerSeenInLVAVignetteRadii";
        (*property_map)[0x2EAFDFA2] = "iInteraction1";
        (*property_map)[0x1258CBD9] = "m_fStatsAccuracyDelta";
        (*property_map)[0xE56C8D9E] = "m_fGlowVSOpponentOutlineWidth";
        (*property_map)[0x104324D8] = "m_nGlowVSOpponentFadeSpeed";
        (*property_map)[0xB4F07DFC] = "m_aShowThroughWallsStates";
        (*property_map)[0xA594C98B] = "m_GlowTraversalColor";
        (*property_map)[0x71A7F844] = "m_fGlowTraversalOutlineWidth";
        (*property_map)[0xA7FFC552] = "m_vGlowTraversalVignetteRadii";
        (*property_map)[0x2338299] = "m_vGlowTraversalFadeNearFar";
        (*property_map)[0x1E6BFAA8] = "m_vGlowCameraVignetteRadii";
        (*property_map)[0x8BC0BACA] = "m_vGlowCameraFadeNearFar";
        (*property_map)[0xEF49F1E9] = "m_fCSMFadeDistance";
        (*property_map)[0xA7E8DC37] = "m_fProjectedAreaGateClipThresholdMirror";
        (*property_map)[0x232DCA5E] = "m_fProjectedAreaCullingMain";
        (*property_map)[0x685BB1D1] = "m_fGlobalGateCullDistance";
        (*property_map)[0x3F40B067] = "m_fGlobalShadowMapConstantBias";
        (*property_map)[0x9308BB6E] = "m_fGlobalShadowMapLinearBias";
        (*property_map)[0x1C83A48A] = "m_fMaxMirrorDistance";
        (*property_map)[0xD4D74F45] = "m_fParticleEmitterMaxDistanceScale";
        (*property_map)[0x5BEFA3D1] = "m_fGlobalLODScale";
        (*property_map)[0x15A21E55] = "m_fLODScaleNormal";
        (*property_map)[0x3AA36B4E] = "m_eDeathTypeFront";
        (*property_map)[0x8480DDB1] = "m_bColorCorrectionHDR";
        (*property_map)[0xF04CBE61] = "m_bTAAJitter";
        (*property_map)[0x76629761] = "m_fNeutralLUTBlendFactor";
        (*property_map)[0xBE85294B] = "m_sPath";
        (*property_map)[0x60F17697] = "m_eActionPrompt";
        (*property_map)[0x3307305A] = "m_bRequestedAction";
        (*property_map)[0xBEE7CE4A] = "m_CharacterReflectionMapID";
        (*property_map)[0x3F95EADF] = "m_fCharacterWetnessSpecularAdd";
        (*property_map)[0x13E41639] = "m_rCinemaModeConfig";
        (*property_map)[0x997D7607] = "m_rFadeConfig";
        (*property_map)[0x57C32688] = "m_bValues";
        (*property_map)[0x3FC8298F] = "m_fTimes";
        (*property_map)[0x9E1AC8BE] = "m_fBlendIn";
        (*property_map)[0x91DC79DD] = "m_aHandlers";
        (*property_map)[0xE6D4067D] = "m_rGeomEntities";
        (*property_map)[0x1229FA9E] = "PoolModificationEvents";
        (*property_map)[0x30F8787] = "m_fBlockSpacing";
        (*property_map)[0x91C3BAD1] = "m_aGroupPrefixes";
        (*property_map)[0x97CC4009] = "m_bKeepAfterUnconscious";
        (*property_map)[0x709B68D0] = "m_DistractedActor";
        (*property_map)[0x565CE740] = "m_CameraTransform";
        (*property_map)[0xDA5FC577] = "m_bShow";
        (*property_map)[0xA7E8EB9D] = "Location";
        (*property_map)[0xDB5C82A7] = "LegendTrackers";
        (*property_map)[0x68BECFA2] = "m_aAccessoryItems";
        (*property_map)[0x9C473D89] = "m_pXboxOneImageRID";
        (*property_map)[0x9BD84428] = "m_rSphereVolume";
        (*property_map)[0x20A6CA89] = "m_eSupport";
        (*property_map)[0x41B3FDDE] = "m_pListener";
        (*property_map)[0xD8C206A2] = "m_BoneName0";
        (*property_map)[0xAFC53634] = "m_BoneName1";
        (*property_map)[0x98BBB7A0] = "m_bOnlyOnce";
        (*property_map)[0x70C7D3AA] = "m_fEffectsRangeMultiplier";
        (*property_map)[0x5ACD12EA] = "m_fPoxStrength";
        (*property_map)[0xEE8FFB80] = "m_bLocalRotation";
        (*property_map)[0xB69D9DD8] = "m_bRespectsWalls";
        (*property_map)[0x71C2B0C] = "m_nExplisionPushSharpness";
        (*property_map)[0xA725EA1A] = "m_fShockwavePropagationSpeed";
        (*property_map)[0xA919D585] = "m_fRagdollImpulseMultiplier";
        (*property_map)[0x663C9D2F] = "m_bDrawLift";
        (*property_map)[0x1922D8EE] = "m_ProjectorRenderDestinationTexture";
        (*property_map)[0x2F760074] = "m_bAllowToCloseThePage";
        (*property_map)[0x32D0033] = "m_sClosePageButtonPromptLocaKeyOverride";
        (*property_map)[0xCFE38569] = "m_aEffectSetsBack";
        (*property_map)[0x6650342A] = "m_eDeathSpeakFront";
        (*property_map)[0x14B8A175] = "m_eDeathSpeakBack";
        (*property_map)[0x84F187BD] = "aTargets";
        (*property_map)[0xE7B61695] = "m_bUsingRecurringDialog";
        (*property_map)[0x5142E90A] = "m_flotationPoint";
        (*property_map)[0x6DF2DB32] = "m_bDisableLogic";
        (*property_map)[0x8B9E92FD] = "m_bStaticObject";
        (*property_map)[0x6D1B2338] = "m_bEnableSnapping";
        (*property_map)[0xB6D6A382] = "m_vAAScreenSize";
        (*property_map)[0x3E3BBBE9] = "m_fAimTime";
        (*property_map)[0xEBB68D89] = "m_bEnableSlowDown";
        (*property_map)[0x15E018EB] = "m_SlowDownBones";
        (*property_map)[0x1EEF2C56] = "m_fSlowDownNearDist";
        (*property_map)[0xD2B51675] = "m_fSlowDownFarFactor";
        (*property_map)[0x8D64E537] = "m_fSlowDownBlendOutTime";
        (*property_map)[0xAE7FE840] = "m_rSBCamProfiles";
        (*property_map)[0x67F8E157] = "m_bEnableTracking";
        (*property_map)[0x2C2BF923] = "m_newAimAssistConfiguration";
        (*property_map)[0xEE7B8E10] = "m_EmitterID";
        (*property_map)[0x85B2EE71] = "m_EventID";
        (*property_map)[0x504BD0F3] = "m_tGroupStarted";
        (*property_map)[0xF5507CC1] = "m_nAssistantApproachNode";
        (*property_map)[0x542AB267] = "m_eApproachOrderState";
        (*property_map)[0x984E18E0] = "m_fsmAssistantState";
        (*property_map)[0xA280571C] = "m_bReservedOccupancy";
        (*property_map)[0x3F23A827] = "m_investigateArea";
        (*property_map)[0xD3C20ACA] = "m_bValidTargetLOSField";
        (*property_map)[0xA337F637] = "m_aGuardMembers";
        (*property_map)[0xE4DA7F8C] = "m_Drama2ActorData";
        (*property_map)[0xF48149C0] = "speakIfNotAlone";
        (*property_map)[0x95A92EC8] = "sightingExpirationThreshold";
        (*property_map)[0x5D5D3A7D] = "m_vAnchorPointA";
        (*property_map)[0xC4546BC7] = "m_vAnchorPointB";
        (*property_map)[0xCDBE5AE4] = "m_TitleLoganID";
        (*property_map)[0x5B8BC28F] = "m_DescriptionLoganID";
        (*property_map)[0xB4B45D69] = "m_iPOICost";
        (*property_map)[0xDEE06727] = "m_fStatsRangeDelta";
        (*property_map)[0xB844ADD] = "m_fStatsDamageDelta";
        (*property_map)[0x2AF6D898] = "m_eWeaponUpgradeUse";
        (*property_map)[0xD697FF2] = "m_fDamageNearBonus";
        (*property_map)[0x1B7938B1] = "m_fDamageFarBonus";
        (*property_map)[0xAF57AAB7] = "m_fDamageMultiplierBonus";
        (*property_map)[0x7EC9DE30] = "m_fInitialMagazinesBonus";
        (*property_map)[0xEC797D75] = "m_iPrecisionNumBonus";
        (*property_map)[0x7D5198E7] = "m_fPrecisionMinDegreePenalty";
        (*property_map)[0x2CA823BA] = "m_fPrecisionMaxDegreePenalty";
        (*property_map)[0x531324E6] = "m_bShowBoneB";
        (*property_map)[0x80D9B562] = "m_runners";
        (*property_map)[0x292DB2C0] = "m_bTerminated";
        (*property_map)[0x154A9442] = "m_rGuard";
        (*property_map)[0xAAB253AA] = "m_bBodyInvestigated";
        (*property_map)[0xE53983D3] = "m_bHidden";
        (*property_map)[0x60D0FDD5] = "m_bDeadByExplosion";
        (*property_map)[0xA312753F] = "m_aIgnoreEntities";
        (*property_map)[0xC306CA3D] = "m_box";
        (*property_map)[0x5AC2DE27] = "m_rCameraPosition";
        (*property_map)[0xDF74EA92] = "m_pCameraSurface";
        (*property_map)[0xC7D55902] = "m_pCameraLookAtSurface";
        (*property_map)[0x84578837] = "m_eAttachToBone";
        (*property_map)[0xBFC6EDAF] = "m_nDensityRadius";
        (*property_map)[0xFC46D51F] = "m_nDensityMax";
        (*property_map)[0xF9F45A1B] = "m_nPrimaryZoneCoverPercent";
        (*property_map)[0x4F2DF65F] = "m_nFirstActorCoverPercent";
        (*property_map)[0xB2DE92C2] = "m_nSurroundedMinZones";
        (*property_map)[0xDE3EDE80] = "DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER_TRANSPARENT";
        (*property_map)[0x96BD061F] = "m_nBackTurnedCrowdAttentionRangeMod";
        (*property_map)[0x993BFFFE] = "m_bHideActors";
        (*property_map)[0x58AB1380] = "m_RequiredObstaclesByDistance";
        (*property_map)[0x9D4EFE51] = "m_CircleRadiusByDistance";
        (*property_map)[0x5F951C6D] = "m_BoxWidthByDistance";
        (*property_map)[0xFD25A473] = "m_ShieldColor";
        (*property_map)[0x1F29DDA6] = "m_nShieldHighlightPower";
        (*property_map)[0xD4061AC3] = "m_nHiddenHighlightPower";
        (*property_map)[0xA0A80FAB] = "m_rCurrentAmbience";
        (*property_map)[0xBD8294C7] = "m_rCurrentGate";
        (*property_map)[0x8279D8D5] = "m_bInTransition";
        (*property_map)[0x54DBAD1C] = "m_rOriginSpatial";
        (*property_map)[0xDE22BFB4] = "m_rDirectionSpatial";
        (*property_map)[0x17B3C0E6] = "m_aInsideVolumes";
        (*property_map)[0xD3424263] = "m_bAnyGunshotDisturbance";
        (*property_map)[0x79C32B27] = "m_aOutsideVolumes";
        (*property_map)[0xF31EF756] = "m_End";
        (*property_map)[0xEBA39534] = "ContractsDatastoreRid";
        (*property_map)[0x38EFC771] = "UnlockablesDatastoreRid";
        (*property_map)[0x320223D1] = "m_rInvestigator";
        (*property_map)[0x2C9D4816] = "m_fFadeDuration";
        (*property_map)[0xEB87330D] = "m_fCutDuration";
        (*property_map)[0xE0AE7DE2] = "transferee";
        (*property_map)[0x38B860A1] = "m_rOutfit";
        (*property_map)[0x2BB5CC3E] = "m_bCheckKiller";
        (*property_map)[0x6DBACA87] = "m_rClosetTransform";
        (*property_map)[0xD36BFD14] = "m_rPeekMovementSequence";
        (*property_map)[0x211DC911] = "m_eItemHandlingRightHand";
        (*property_map)[0x8CA6FB9A] = "m_eItemHandlingLeftHand";
        (*property_map)[0xCDEBD7E1] = "m_bDropBackHolsteredItem";
        (*property_map)[0xCC197C58] = "m_bSlowAnimation";
        (*property_map)[0x4239CC3A] = "m_bSafeDrop";
        (*property_map)[0xC5F5DF17] = "m_vSafeDropPosition";
        (*property_map)[0x8C39D073] = "m_bMakeBoxCollider";
        (*property_map)[0xCF416152] = "m_bMakeSphereCollider";
        (*property_map)[0xDF9A8AC3] = "m_bUnpauseOnStop";
        (*property_map)[0xF1F32D79] = "m_triggerOnChange";
        (*property_map)[0x55B52135] = "m_boneTests";
        (*property_map)[0xD54A265C] = "m_NearRangeUpperBound";
        (*property_map)[0xB08013] = "SHOT_ONLY_COLLISION";
        (*property_map)[0x18D82C03] = "DYNAMIC_COLLIDABLES_ONLY_TRANSPARENT";
        (*property_map)[0x6ACBB9B] = "STAIRS_STEPS";
        (*property_map)[0x6A42F46C] = "STAIRS_SLOPE";
        (*property_map)[0xE07EDE70] = "HERO_PROXY";
        (*property_map)[0x9BB5F9D3] = "CLIP";
        (*property_map)[0x568A6228] = "LEDGE_ANCHOR";
        (*property_map)[0xA06AF194] = "DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER";
        (*property_map)[0xBB9406D6] = "COLLIDE_WITH_STATIC_ONLY";
        (*property_map)[0x70CAF855] = "AI_VISION_BLOCKER_AMBIENT_ONLY";
        (*property_map)[0x185E313B] = "m_lateralShiftCriterion";
        (*property_map)[0x4D5D1AE9] = "m_farPositionOffset";
        (*property_map)[0x6B271C9C] = "m_middleBoxSize";
        (*property_map)[0x72EACE80] = "m_middleBoxDepthMarginFar";
        (*property_map)[0xEB93E0B] = "m_fHealthPerc";
        (*property_map)[0xC150BF2E] = "m_rValueXEntity";
        (*property_map)[0x56CFAE07] = "m_rValueZEntity";
        (*property_map)[0xD1D43DBA] = "m_fRangeModifierThrownItemCollision";
        (*property_map)[0xF7177599] = "m_bFastWakeup";
        (*property_map)[0x23ED425A] = "m_bDisableWhenUsed";
        (*property_map)[0xAB91F8EA] = "m_fBlendOutDuration";
        (*property_map)[0x105B03C9] = "m_DebugGreenColorScoreThreshold";
        (*property_map)[0xBFE3BC75] = "m_PenaltyCombatUnknown";
        (*property_map)[0xB5364B1] = "m_PenaltySpawnPointAtDeepTrespassing";
        (*property_map)[0xAD79310F] = "m_PenaltyRangeCombat";
        (*property_map)[0x103679DC] = "m_PenaltyRangeCombatUnknown";
        (*property_map)[0x4D4FBA03] = "m_PenaltyRangeLTMArrest";
        (*property_map)[0xED3669A4] = "m_PenaltyRangeLTMCombat";
        (*property_map)[0x5DB5964C] = "m_MinimalSpawnDistance";
        (*property_map)[0x2FD0FA6C] = "m_bDifficulty_Level4";
        (*property_map)[0x8928C8F8] = "m_MaximumSpawnPenalty";
        (*property_map)[0xC3BA489B] = "attack_impact";
        (*property_map)[0xD0410122] = "m_pVideoRenderDest";
        (*property_map)[0xC16219EF] = "m_pSkipAllowedCondition";
        (*property_map)[0x59EE9291] = "m_timeFlipDirectionSec";
        (*property_map)[0x77F7C470] = "m_HitmanToBlame";
        (*property_map)[0x109D4779] = "m_IsInvestigated";
        (*property_map)[0x28BD3E75] = "m_bDifficulty_Level0";
        (*property_map)[0x4AF549F8] = "m_arItems";
        (*property_map)[0xA541C5BD] = "m_fEventRememberMaxTime";
        (*property_map)[0xE58A96E7] = "m_rCustomAnimationSet";
        (*property_map)[0xB16883DF] = "m_vGlobalLightOverrideDirection";
        (*property_map)[0xDD6317E0] = "m_bRunDuringPause";
        (*property_map)[0xB9FB5F84] = "m_ParticleResourceID";
        (*property_map)[0x697F6DC] = "m_iMaxNumParticles";
        (*property_map)[0xBF2A8E1F] = "m_MiddlePlaneSize";
        (*property_map)[0x13E18D90] = "m_FarDepth";
        (*property_map)[0xBFD1CD1F] = "m_bCoverAtWaypoint";
        (*property_map)[0x913A2079] = "m_pTargetBoxes";
        (*property_map)[0x28BE9AEC] = "m_bUseLocalSpace";
        (*property_map)[0x2088F9F8] = "m_sSampleName";
        (*property_map)[0x59162E07] = "m_pNodeAttachTarget";
        (*property_map)[0xB43A878E] = "m_PistolHolsterAttacher";
        (*property_map)[0xB1789D19] = "MaterialInstanceOverrideDest";
        (*property_map)[0x4CB2F934] = "Active";
        (*property_map)[0xEFA98EB5] = "Global";
        (*property_map)[0x606D33E5] = "OverrideInherit";
        (*property_map)[0xD261F1F5] = "m_aDynamicEnforceEventsForSearchers";
        (*property_map)[0x876C2D13] = "Static";
        (*property_map)[0x796A7DC8] = "m_bPropertyClassB";
        (*property_map)[0x3B6ADD1A] = "m_EntityID";
        (*property_map)[0x50D3E3FF] = "m_fMinDist";
        (*property_map)[0x61289C89] = "m_pLocationController";
        (*property_map)[0xA9833B6F] = "m_bIsMultiplayerModeActive";
        (*property_map)[0xA178AA33] = "m_pBlobConfig";
        (*property_map)[0x98E51525] = "m_pContractsConfig";
        (*property_map)[0xB4C7A7B] = "m_pUnlockablesConfig";
        (*property_map)[0xE1595022] = "m_pActivitiesConfig";
        (*property_map)[0x2A8A391E] = "m_bIsSittingChairTable";
        (*property_map)[0xFF518B4] = "m_rSoundEvent_DischargeEmpty";
        (*property_map)[0xDE462689] = "bTriggered";
        (*property_map)[0x2025BD1] = "m_nSyncTrackIndex";
        (*property_map)[0x98C8A1AF] = "m_eSetState";
        (*property_map)[0xC5FBC62E] = "m_vRoomMax";
        (*property_map)[0xF80BB88F] = "m_bOutsideMaster";
        (*property_map)[0x54C5662B] = "m_bCSMRoom";
        (*property_map)[0xA95F9423] = "m_bContainsCrowd";
        (*property_map)[0x8C3763F6] = "m_nRoomPriority";
        (*property_map)[0x9EB95699] = "m_bGlobalLightOverrideEnable";
        (*property_map)[0xCA226361] = "m_fGlobalLightOverrideFrontIntensity";
        (*property_map)[0x712BFDA4] = "m_rProperty";
        (*property_map)[0x8F8A62B7] = "m_fScoreGameTimeStart";
        (*property_map)[0xD0E0396A] = "alpha";
        (*property_map)[0x3182E3B4] = "m_conditions";
        (*property_map)[0x9BD900A5] = "m_fDecay";
        (*property_map)[0x1846398E] = "m_bSmooth";
        (*property_map)[0x1A24C2CF] = "m_RootNodes";
        (*property_map)[0x30919BE1] = "m_rVRAgilityConfigDefault";
        (*property_map)[0x52BF4BAA] = "fPreferredDistance";
        (*property_map)[0x3D2B4096] = "m_rSoundEvent_Collision";
        (*property_map)[0xF35E55C6] = "m_aEntrances";
        (*property_map)[0xE5E6B641] = "m_aLinkedIslands";
        (*property_map)[0x1BC2FF8] = "m_bUpdateTransform";
        (*property_map)[0x1800B07F] = "m_nNumberOfRTPCs";
        (*property_map)[0x8976019B] = "objectiveConditions";
        (*property_map)[0x17CE0718] = "fX";
        (*property_map)[0x60C9378E] = "fY";
        (*property_map)[0x4E2B5F4E] = "m_eEnableValueCheck";
        (*property_map)[0xA9AD18CE] = "m_BillboardRenderDestinationTexture";
        (*property_map)[0x39142965] = "m_MasterLight";
        (*property_map)[0x37974C79] = "m_diffuseColor";
        (*property_map)[0xFF2E5E11] = "m_vAngularAttenuation";
        (*property_map)[0x75260351] = "m_fGridMapHeightOffsetsScale";
        (*property_map)[0x6E726A0F] = "m_fAspectXByY";
        (*property_map)[0x858FC767] = "m_fCapsuleHeight";
        (*property_map)[0x92244CDB] = "m_vCSMCascadeRadii";
        (*property_map)[0x4D923371] = "m_vBoxFadeNeg";
        (*property_map)[0x524A07BB] = "m_fLightFadeDistance";
        (*property_map)[0x401FC030] = "m_bCrowdCastShadows";
        (*property_map)[0xFE9ED10C] = "m_bUseStaticShadowMap";
        (*property_map)[0xAAA0783C] = "m_bHQDynamicShadowMap";
        (*property_map)[0x4A0ECE28] = "m_fBreakableTorque";
        (*property_map)[0xB47DE5E1] = "m_sIdentifier";
        (*property_map)[0xCBB0180D] = "m_rAimAssistConfiguration";
        (*property_map)[0x6E4C8C3C] = "m_rSharedSensor";
        (*property_map)[0x1DAB493B] = "m_bHasRecordings";
        (*property_map)[0x5F5BC34A] = "fInt16Prop";
        (*property_map)[0x3484C5E8] = "sStringProp";
        (*property_map)[0x5E5CB132] = "eEnumProp";
        (*property_map)[0xA082A89] = "aFixedArray";
        (*property_map)[0x571357EA] = "m_animSetResourceID";
        (*property_map)[0xCF170A1A] = "m_fVisionConeAngle";
        (*property_map)[0xAF0CD875] = "m_bNeedCollisionCheck";
        (*property_map)[0xBE640E10] = "m_sDesc";
        (*property_map)[0x86577514] = "m_cautiousVIPGroups";
        (*property_map)[0x5F341477] = "m_friskGroups";
        (*property_map)[0x10F888F7] = "m_nHuntTarget";
        (*property_map)[0x352CAF8F] = "m_disturbanceField";
        (*property_map)[0xA4676E2] = "m_bSuspendSearchOnInitialGetHelpGroup";
        (*property_map)[0x28E58853] = "m_lastArrestReason";
        (*property_map)[0x21123C56] = "m_lastStaticCombatReason";
        (*property_map)[0x383B0037] = "m_deadBodyMassiveReported";
        (*property_map)[0xDA0FD04E] = "m_deadBodiesDiscovered";
        (*property_map)[0xCCEB1220] = "m_vOriginalEntityTransform";
        (*property_map)[0x20F2F7D5] = "m_vTargetStartTransform";
        (*property_map)[0xB1B63173] = "m_rCameraProfileBlendDatabase";
        (*property_map)[0xBA0ABC32] = "m_rClosetPeekCameraProfile";
        (*property_map)[0xC13F4152] = "m_AnimationOrigin";
        (*property_map)[0xF69F65C] = "m_vQuaternionRotation";
        (*property_map)[0x34A9F59D] = "m_pClothRustleSound";
        (*property_map)[0xA59B048E] = "m_pPushReactionSound";
        (*property_map)[0xAE28A555] = "m_pPanicSound";
        (*property_map)[0x307FBD23] = "m_pGeneric00Sound";
        (*property_map)[0x76AC750A] = "m_Default";
        (*property_map)[0x8A98AD6E] = "m_TriggerOnHitman";
        (*property_map)[0xD5D4701D] = "m_TriggerOnActorAlive";
        (*property_map)[0xD035907D] = "m_TriggerOnActorPacified";
        (*property_map)[0x9C031942] = "m_TriggerOnItem";
        (*property_map)[0x691365F1] = "m_TriggerOnlyOnVisible";
        (*property_map)[0x68837771] = "m_TriggerEntities";
        (*property_map)[0x966E6E47] = "m_colorFill";
        (*property_map)[0x96D16859] = "m_colorStroke";
        (*property_map)[0x9018715A] = "m_fStrokeWidth";
        (*property_map)[0x1D1292C2] = "m_bFill";
        (*property_map)[0x642E311C] = "m_bStroke";
        (*property_map)[0x95E7F03F] = "m_aFlowChannels";
        (*property_map)[0x45710811] = "m_bClip";
        (*property_map)[0x1379BFD8] = "m_placementMaterial";
        (*property_map)[0x238470CE] = "m_rPlacementPrim";
        (*property_map)[0xEC8EA05C] = "m_rPlacementLinked";
        (*property_map)[0x2B471851] = "m_vCellConsts";
        (*property_map)[0x5A4B9C57] = "m_nGridSizeY";
        (*property_map)[0xF80A5618] = "m_CellFlags";
        (*property_map)[0xB8FF70AD] = "m_mObjectToWorld";
        (*property_map)[0x91E9B46C] = "m_aStateMachines";
        (*property_map)[0x39F409CD] = "m_pResumeAllEvent";
        (*property_map)[0xB1F0E319] = "m_FarRangeLowerBound";
        (*property_map)[0xB2C8781F] = "m_rConnectorEnd";
        (*property_map)[0x86B3EBD2] = "m_bRotateOnlyNearPoints";
        (*property_map)[0x5C22E625] = "m_fRotationNearPointDistance";
        (*property_map)[0xB28925FE] = "m_fLeftHandWeight";
        (*property_map)[0x3322DCE8] = "m_fRightHandTargetWeight";
        (*property_map)[0x314CD578] = "m_idVideo";
        (*property_map)[0x24B6B27E] = "m_pDebugShowSessionID";
        (*property_map)[0x6D9D6187] = "m_CrowdActorData";
        (*property_map)[0xBA5E4783] = "m_RegionData";
        (*property_map)[0x3B244AF0] = "m_aCrowdAiPoolActorsMale";
        (*property_map)[0x1381B5BA] = "m_pPIPMessageEntity";
        (*property_map)[0xEC29359C] = "m_fTangentVerticalWeight";
        (*property_map)[0xAF0F1E0D] = "m_sBaconShotText";
        (*property_map)[0xAFBF64A3] = "m_pBaconShotTextListResource";
        (*property_map)[0x54A9EB07] = "m_bTimeOfDayRangeInvert";
        (*property_map)[0x1778D317] = "AccumulatedDataBase64";
        (*property_map)[0x47CC8C92] = "action";
        (*property_map)[0x9EE75EAE] = "ActiveNodesBase64";
        (*property_map)[0xB278AAA5] = "AnimationGraphs";
        (*property_map)[0xFBD6372E] = "arguments";
        (*property_map)[0xA10CEEB7] = "array";
        (*property_map)[0x85B44097] = "bExists";
        (*property_map)[0x1AD05EAF] = "bInChangeList";
        (*property_map)[0xFDAC4B25] = "bInOurChangeList";
        (*property_map)[0x2F7FD08C] = "bInsideCrimeScene";
        (*property_map)[0xC8869496] = "bIsReadOnly";
        (*property_map)[0xDDF9EEB6] = "bLocked";
        (*property_map)[0x9BABB9CE] = "bOurLock";
        (*property_map)[0x89BAA988] = "bOurSubmitLock";
        (*property_map)[0x564ACEC8] = "bSubmitLocked";
        (*property_map)[0x7F1AE214] = "bValid";
        (*property_map)[0xDF8D7614] = "bVisible";
        (*property_map)[0xFBC78990] = "callID";
        (*property_map)[0x1682D9C2] = "CaptureWindowEnd";
        (*property_map)[0xDBB462A2] = "CaptureWindowStart";
        (*property_map)[0x736C2D08] = "classField";
        (*property_map)[0x46621370] = "clientFile";
        (*property_map)[0xBB93F06E] = "DefaultParameters";
        (*property_map)[0xC97C1D17] = "DefaultType";
        (*property_map)[0x6BFC0740] = "depotFile";
        (*property_map)[0x8FB46099] = "DownMixToMono";
        (*property_map)[0x8CF6A5FF] = "dynObj";
        (*property_map)[0x28A30C0E] = "EntityReference";
        (*property_map)[0x61ED060D] = "entityTemplates";
        (*property_map)[0x7E985A80] = "EventTrackStringTableBase64";
        (*property_map)[0x8C3F1954] = "expectResult";
        (*property_map)[0x8B477D66] = "fCooldown";
        (*property_map)[0xC7F46F5D] = "Files";
        (*property_map)[0xD91677E9] = "Format";
        (*property_map)[0xC11D4478] = "fPointShootingGaunge";
        (*property_map)[0x66EB8C0C] = "fSBDamage";
        (*property_map)[0x5908EFF9] = "fUnconsciousTimeLeft";
        (*property_map)[0xAFCE944F] = "functionName";
        (*property_map)[0xA55408CD] = "headAction";
        (*property_map)[0x1D102D5C] = "headType";
        (*property_map)[0x15289F6A] = "intArrayField";
        (*property_map)[0xFB289CBB] = "intValue";
        (*property_map)[0x2CDC6B0A] = "IsNotDucking";
        (*property_map)[0x79E92ACA] = "IsPlaceHolder";
        (*property_map)[0x8A90ABA9] = "key";
        (*property_map)[0xCC064F89] = "lastModificationTime";
        (*property_map)[0xC45C1EBE] = "LoadType";
        (*property_map)[0x6E173B79] = "m_aAttachVolumes";
        (*property_map)[0x260B688E] = "m_aAudioEvents";
        (*property_map)[0x12CA81F7] = "m_aBoolConditionSaveData";
        (*property_map)[0x365B5CC8] = "m_aBulletTypeAttenuation";
        (*property_map)[0xBCAE30F] = "m_aBulletTypePitch";
        (*property_map)[0x6398A6B1] = "m_aCrimeScenes";
        (*property_map)[0x4BE2EF24] = "m_aCrowdAvoidRegions";
        (*property_map)[0xC5CBDA18] = "m_aCrowdEntities";
        (*property_map)[0xAF3DB5A] = "m_Action";
        (*property_map)[0x6AC2DE88] = "m_ActivationArea";
        (*property_map)[0x679DE23F] = "m_ActorNoiseList";
        (*property_map)[0x7E4D93DB] = "m_ActorOutfit";
        (*property_map)[0x215BC1FD] = "m_aCustomDeaths";
        (*property_map)[0x464179D1] = "m_aCustomReactions";
        (*property_map)[0x347F2F10] = "m_aDynamicEntities";
        (*property_map)[0x8E56A18D] = "m_aDynamicParameters";
        (*property_map)[0x6934AAED] = "m_aGametimeTimerEntities";
        (*property_map)[0x59D54B1A] = "m_AimAssistDefault";
        (*property_map)[0xD8E3654E] = "m_AimAssistShootingBallet";
        (*property_map)[0xF555883F] = "m_AimAssistSniper";
        (*property_map)[0xAB388A50] = "m_aInvestigationGroupIDs";
        (*property_map)[0x1E2CD81A] = "m_aLegendConfigs";
        (*property_map)[0x4FE7B08C] = "m_aLocatedExtraData";
        (*property_map)[0xCF5D9835] = "m_aMuzzleExtensions";
        (*property_map)[0x560CBCC1] = "m_aNeighborhood";
        (*property_map)[0xB0A5FE60] = "m_AnimationSetResource";
        (*property_map)[0xF4DDD551] = "m_aOptions";
        (*property_map)[0xB2BC6EDB] = "m_aOriginalExtraData";
        (*property_map)[0x6BFBE675] = "m_aPanicDebrisResources";
        (*property_map)[0xF9D4DCF7] = "m_aRealtimeTimerEntities";
        (*property_map)[0xA8EAE762] = "m_aResourceIDs";
        (*property_map)[0x86905120] = "m_aRicochetTypeAttenuation";
        (*property_map)[0x3B1DA80F] = "m_aRicochetTypePitch";
        (*property_map)[0x43BBED2D] = "m_aRooms";
        (*property_map)[0x40A1390C] = "m_aRTPCs";
        (*property_map)[0x585893C3] = "m_aScopes";
        (*property_map)[0x6FBA5D24] = "m_aScopesFPS";
        (*property_map)[0x22BB0748] = "m_aSelectButtons";
        (*property_map)[0x2862EA91] = "m_aSoundDefinitions";
        (*property_map)[0xC7546A41] = "m_aSpawnPoints";
        (*property_map)[0x5349161D] = "m_aStateControllerSaveData";
        (*property_map)[0x472CD597] = "m_aStringTable";
        (*property_map)[0x9CDE5DFD] = "m_aSwitches";
        (*property_map)[0x7675068B] = "m_attachedContextValue";
        (*property_map)[0x379136C8] = "m_aTypeDescripters";
        (*property_map)[0x6C0C3215] = "m_aUIElements";
        (*property_map)[0xE5270BB2] = "m_aUISupportElements";
        (*property_map)[0x50961D9D] = "m_aValueBoolSaveData";
        (*property_map)[0x64D64359] = "m_aValueFloatSaveData";
        (*property_map)[0x58928595] = "m_aValueIntSaveData";
        (*property_map)[0xF487D48C] = "m_bActivateBoxVolume";
        (*property_map)[0xA7F5BC46] = "m_bAffectBone";
        (*property_map)[0xEF515CBE] = "m_bAimTarget";
        (*property_map)[0xA39AE598] = "m_bAlignListener";
        (*property_map)[0xA5714FA8] = "m_bAllowHumanShield";
        (*property_map)[0xD5803807] = "m_bAllowInstinctGlow";
        (*property_map)[0x783C0BD0] = "m_bAllowInstinctPaths";
        (*property_map)[0x7B08D7FF] = "m_bAllowPlayerToExecute";
        (*property_map)[0x574F8A06] = "m_bAlwaysShowSupportItemInInventory";
        (*property_map)[0x53B8159A] = "m_bAnnouncingHunt";
        (*property_map)[0x59596560] = "m_bAnyWeapon";
        (*property_map)[0xF113A89] = "m_bAutomaticEvaluation";
        (*property_map)[0x3452230E] = "m_bByFrameUpdate";
        (*property_map)[0xDD29629F] = "m_bCanCancelShootingBallet";
        (*property_map)[0xCCF3EA36] = "m_bCanEnter";
        (*property_map)[0x7D8040CC] = "m_bCanExit";
        (*property_map)[0xBB7715BC] = "m_bCanPeekEndVent";
        (*property_map)[0x413D867F] = "m_bCanPeekStartVent";
        (*property_map)[0xE5936AA4] = "m_bCanSelfInterrupt";
        (*property_map)[0xC8C396AD] = "m_bCheckedWhenFrisked";
        (*property_map)[0x42E9C6DE] = "m_bCollidable";
        (*property_map)[0x9BA7E123] = "m_bCollisions";
        (*property_map)[0xF37C8DF6] = "m_bCoreInitialIgnoreCharacters";
        (*property_map)[0x36D0B283] = "m_bCustomReaction";
        (*property_map)[0x8BA4E9B4] = "m_bDeadActors";
        (*property_map)[0x7E572462] = "m_bDelayCollidables";
        (*property_map)[0xE11772D8] = "m_bDisableFuseBox";
        (*property_map)[0x23DC3DB0] = "m_bDisableParentCollision";
        (*property_map)[0xF20462A7] = "m_bDisguiseGreetingsEnabled";
        (*property_map)[0x9FED22C5] = "m_bDoneReportingToHitman";
        (*property_map)[0x8A4BE793] = "m_bDummyBone";
        (*property_map)[0x655931DA] = "m_bEnableBodyPullDebug";
        (*property_map)[0xD380900E] = "m_bEnableLightSimplification";
        (*property_map)[0xFE963A1] = "m_bExclamationMark";
        (*property_map)[0xE28D60FB] = "m_bExecuteWhenInstinctRunsOut";
        (*property_map)[0xC10AE5BC] = "m_bExemptFromSlowDown";
        (*property_map)[0x9BFD6BD2] = "m_bExemptFromThreatRadar";
        (*property_map)[0xCF2F1792] = "m_bFocusShotCostEnable";
        (*property_map)[0x31663105] = "m_bGroupEngaged";
        (*property_map)[0x27D0D129] = "m_bHasCheckedLastKnownPosition";
        (*property_map)[0x6B39542A] = "m_bHasThingsTodo";
        (*property_map)[0xDE1F06] = "m_bHeroPacifyPushedNPC";
        (*property_map)[0xFDB2724A] = "m_bHiddenWithHitman";
        (*property_map)[0xF5E94FB2] = "m_bHideHitman";
        (*property_map)[0xA42BADC6] = "m_bHighEntrance";
        (*property_map)[0x66CF1577] = "m_bHitmanBizarre";
        (*property_map)[0x9A904B65] = "m_bHitmanWarned";
        (*property_map)[0xA8735259] = "m_bHitmanWasBizarre";
        (*property_map)[0xAF6530C5] = "m_bHybridTransitionEnabled";
        (*property_map)[0xD6EFD431] = "m_bIgnoreCharacterCollision";
        (*property_map)[0x5D25EDCB] = "m_bImportant";
        (*property_map)[0x815555BC] = "m_bInfiniteAmmo";
        (*property_map)[0x53066E52] = "m_bInitialIgnoreCharacters";
        (*property_map)[0xA9C90816] = "m_bIsExplosive";
        (*property_map)[0x9DD407B6] = "m_bIsForDifficulty";
        (*property_map)[0xF883D603] = "m_bIsHanging";
        (*property_map)[0xF488632F] = "m_bIsReinforcing";
        (*property_map)[0x36AB22FF] = "m_bIsSnoring";
        (*property_map)[0x3416EF28] = "m_bIsWorldRoom";
        (*property_map)[0xD1804B30] = "m_bKeyCardReader";
        (*property_map)[0xA4A6C230] = "m_bListenerFollowCamZoom";
        (*property_map)[0x53BD6096] = "m_bLockOnNavmesh";
        (*property_map)[0xF3E67ADB] = "m_blood";
        (*property_map)[0x2378DA6C] = "m_Blur";
        (*property_map)[0x7A7AB774] = "m_bMuteGameWhilePlaying";
        (*property_map)[0xCD64EA0E] = "m_bNavigationTarget";
        (*property_map)[0x4C1F87A6] = "m_bNotify";
        (*property_map)[0xFA82560A] = "m_bNotifyChange";
        (*property_map)[0xDFF9E566] = "m_bOcclusionGroup1";
        (*property_map)[0x50D5E2A9] = "m_bOcclusionGroup10";
        (*property_map)[0x27D2D23F] = "m_bOcclusionGroup11";
        (*property_map)[0xBEDB8385] = "m_bOcclusionGroup12";
        (*property_map)[0xC9DCB313] = "m_bOcclusionGroup13";
        (*property_map)[0x57B826B0] = "m_bOcclusionGroup14";
        (*property_map)[0x20BF1626] = "m_bOcclusionGroup15";
        (*property_map)[0xB9B6479C] = "m_bOcclusionGroup16";
        (*property_map)[0xCEB1770A] = "m_bOcclusionGroup17";
        (*property_map)[0x5E0E6A9B] = "m_bOcclusionGroup18";
        (*property_map)[0x29095A0D] = "m_bOcclusionGroup19";
        (*property_map)[0x46F0B4DC] = "m_bOcclusionGroup2";
        (*property_map)[0x7BF8B16A] = "m_bOcclusionGroup20";
        (*property_map)[0xCFF81FC] = "m_bOcclusionGroup21";
        (*property_map)[0x95F6D046] = "m_bOcclusionGroup22";
        (*property_map)[0xE2F1E0D0] = "m_bOcclusionGroup23";
        (*property_map)[0x7C957573] = "m_bOcclusionGroup24";
        (*property_map)[0xB9245E5] = "m_bOcclusionGroup25";
        (*property_map)[0x929B145F] = "m_bOcclusionGroup26";
        (*property_map)[0xE59C24C9] = "m_bOcclusionGroup27";
        (*property_map)[0x75233958] = "m_bOcclusionGroup28";
        (*property_map)[0x22409CE] = "m_bOcclusionGroup29";
        (*property_map)[0x31F7844A] = "m_bOcclusionGroup3";
        (*property_map)[0x62E3802B] = "m_bOcclusionGroup30";
        (*property_map)[0x15E4B0BD] = "m_bOcclusionGroup31";
        (*property_map)[0x8CEDE107] = "m_bOcclusionGroup32";
        (*property_map)[0xFBEAD191] = "m_bOcclusionGroup33";
        (*property_map)[0x658E4432] = "m_bOcclusionGroup34";
        (*property_map)[0x128974A4] = "m_bOcclusionGroup35";
        (*property_map)[0x8B80251E] = "m_bOcclusionGroup36";
        (*property_map)[0xFC871588] = "m_bOcclusionGroup37";
        (*property_map)[0x6C380819] = "m_bOcclusionGroup38";
        (*property_map)[0x1B3F388F] = "m_bOcclusionGroup39";
        (*property_map)[0xAF9311E9] = "m_bOcclusionGroup4";
        (*property_map)[0x2DA216EC] = "m_bOcclusionGroup40";
        (*property_map)[0x5AA5267A] = "m_bOcclusionGroup41";
        (*property_map)[0xC3AC77C0] = "m_bOcclusionGroup42";
        (*property_map)[0xB4AB4756] = "m_bOcclusionGroup43";
        (*property_map)[0x2ACFD2F5] = "m_bOcclusionGroup44";
        (*property_map)[0x5DC8E263] = "m_bOcclusionGroup45";
        (*property_map)[0xC4C1B3D9] = "m_bOcclusionGroup46";
        (*property_map)[0xB3C6834F] = "m_bOcclusionGroup47";
        (*property_map)[0x23799EDE] = "m_bOcclusionGroup48";
        (*property_map)[0x547EAE48] = "m_bOcclusionGroup49";
        (*property_map)[0xD894217F] = "m_bOcclusionGroup5";
        (*property_map)[0x34B927AD] = "m_bOcclusionGroup50";
        (*property_map)[0x43BE173B] = "m_bOcclusionGroup51";
        (*property_map)[0xDAB74681] = "m_bOcclusionGroup52";
        (*property_map)[0xADB07617] = "m_bOcclusionGroup53";
        (*property_map)[0x33D4E3B4] = "m_bOcclusionGroup54";
        (*property_map)[0x44D3D322] = "m_bOcclusionGroup55";
        (*property_map)[0xDDDA8298] = "m_bOcclusionGroup56";
        (*property_map)[0xAADDB20E] = "m_bOcclusionGroup57";
        (*property_map)[0x3A62AF9F] = "m_bOcclusionGroup58";
        (*property_map)[0x4D659F09] = "m_bOcclusionGroup59";
        (*property_map)[0x419D70C5] = "m_bOcclusionGroup6";
        (*property_map)[0x1F94746E] = "m_bOcclusionGroup60";
        (*property_map)[0x689344F8] = "m_bOcclusionGroup61";
        (*property_map)[0xF19A1542] = "m_bOcclusionGroup62";
        (*property_map)[0x869D25D4] = "m_bOcclusionGroup63";
        (*property_map)[0x18F9B077] = "m_bOcclusionGroup64";
        (*property_map)[0x369A4053] = "m_bOcclusionGroup7";
        (*property_map)[0xA6255DC2] = "m_bOcclusionGroup8";
        (*property_map)[0xD1226D54] = "m_bOcclusionGroup9";
        (*property_map)[0xCFD40D] = "m_bOnlyHitman";
        (*property_map)[0x5F38519B] = "m_bPerceivedAsWeapon";
        (*property_map)[0x1C53D312] = "m_bPiercing";
        (*property_map)[0xD22130FF] = "m_bPointShootingAllowed";
        (*property_map)[0x2D256673] = "m_bPoolInvestigated";
        (*property_map)[0x857B0ED0] = "m_bPoolKnownOnly";
        (*property_map)[0xF4690B7B] = "m_bPossessed";
        (*property_map)[0x8F1E314D] = "m_bRemovePhysicsOnSleep";
        (*property_map)[0x2D94D2CE] = "m_bRepeatable";
        (*property_map)[0xFE359BC9] = "m_bRequireOnlineToStart";
        (*property_map)[0x8550C4] = "m_BRInputs";
        (*property_map)[0x55ECC670] = "m_bRunningForOutfit";
        (*property_map)[0x8FC05F22] = "m_bShootingBallet";
        (*property_map)[0xFE982792] = "m_bShots";
        (*property_map)[0x13FFE32E] = "m_bShowHolsteredWeapon";
        (*property_map)[0x25B7461F] = "m_bShowWhenLocked";
        (*property_map)[0xE2131BD1] = "m_bSilenced";
        (*property_map)[0x9088A940] = "m_bSlowdownTarget";
        (*property_map)[0x780C8D59] = "m_bSmallOcclusionRoom";
        (*property_map)[0x8E2A391B] = "m_bSniperSlowDownCostEnable";
        (*property_map)[0x8BF607D8] = "m_bSpawnOutsideCrowd";
        (*property_map)[0x1FB8012A] = "m_bSplineFollower";
        (*property_map)[0xE685637A] = "m_bSumSegments";
        (*property_map)[0xF54AC2C4] = "m_bTestActors";
        (*property_map)[0x99987288] = "m_bTestHitman";
        (*property_map)[0x42CB149F] = "m_bUpdateTranslation";
        (*property_map)[0x3C4E4FF4] = "m_bUseAllInputChannels";
        (*property_map)[0x23810370] = "m_bUseDefaultBehavior";
        (*property_map)[0xA14BF6B3] = "m_bUseDelayShards";
        (*property_map)[0x2543C2B9] = "m_bUseFocusEconomy";
        (*property_map)[0xE5B87451] = "m_bUseLabsDof";
        (*property_map)[0x8A62FD41] = "m_bUseSupportConnection";
        (*property_map)[0x22E47643] = "m_bVentilatorShaft";
        (*property_map)[0xBFF91CD7] = "m_bWarned";
        (*property_map)[0xF4D68939] = "m_bWasMoved";
        (*property_map)[0x423DE90E] = "m_bWasShattered";
        (*property_map)[0xCC6EDD26] = "m_CharacterEntity";
        (*property_map)[0xAC4F2B9D] = "m_ClothBundleSpawn";
        (*property_map)[0x95090BFE] = "m_CollisionEffectID";
        (*property_map)[0x574CB954] = "m_ColorDefault";
        (*property_map)[0xA06BD35A] = "m_ColorImportant";
        (*property_map)[0xF9E4B58C] = "m_ColorImportantRepeat";
        (*property_map)[0xB1500F3C] = "m_ColorRepeat";
        (*property_map)[0x26C4B11E] = "m_combatZone";
        (*property_map)[0x4A2EDE34] = "m_ContainedItem";
        (*property_map)[0xDAE414B6] = "m_ContainerSaveData";
        (*property_map)[0xB5A188CA] = "m_ControlPoints";
        (*property_map)[0x8C846660] = "m_CrimeSceneServiceData";
        (*property_map)[0x88BB47C2] = "m_Description";
        (*property_map)[0x7ED875E2] = "m_DescriptionLocked";
        (*property_map)[0x3C5D0EB3] = "m_DescriptionRepeat";
        (*property_map)[0xA2F13805] = "m_Displays";
        (*property_map)[0x4153019] = "m_eAIEvent";
        (*property_map)[0x62AAF0E9] = "m_eAmbientFlowUsage";
        (*property_map)[0x77FF0F88] = "m_Easy";
        (*property_map)[0x596A2D9F] = "m_EasyAISensorDef";
        (*property_map)[0xD39400D9] = "m_EasyAISoundSensorDef";
        (*property_map)[0xA4125534] = "m_EasyFocus";
        (*property_map)[0x9D36E72] = "m_eBehavior";
        (*property_map)[0xE9D843DC] = "m_eBrushStampMode";
        (*property_map)[0x9B2D3E2A] = "m_eBulletType";
        (*property_map)[0x1C5316C7] = "m_eColliderMaterial";
        (*property_map)[0xD76F1B77] = "m_eCorePhysics";
        (*property_map)[0xFEDEF43F] = "m_eFocusAddRemove";
        (*property_map)[0x4EAF9DA1] = "m_eFocusValue";
        (*property_map)[0x53CAAC0F] = "m_eGameModeFilter";
        (*property_map)[0xE0CDE1F8] = "m_eHDRAdaptationType";
        (*property_map)[0xB4EA541D] = "m_eItemType";
        (*property_map)[0x89C44D27] = "m_eListenerMaterial";
        (*property_map)[0x549B5C5B] = "m_eMinDifficulty";
        (*property_map)[0xA649F290] = "m_ePropagation";
        (*property_map)[0xF68DE531] = "m_eRicochetType";
        (*property_map)[0x4621A372] = "m_escalateTarget";
        (*property_map)[0xEE282AB6] = "m_eSettingsParam";
        (*property_map)[0x7B262C18] = "m_eShootIntensity";
        (*property_map)[0xA3D3F0B1] = "m_eSpeakSound";
        (*property_map)[0x2F8C2D82] = "m_eStandDownType";
        (*property_map)[0xFC64AEA9] = "m_eventName";
        (*property_map)[0x46DA79D9] = "m_eWeaponType";
        (*property_map)[0x224C48A] = "m_Expert";
        (*property_map)[0xF3BB2118] = "m_ExpertAISensorDef";
        (*property_map)[0x4227D887] = "m_ExpertAISoundSensorDef";
        (*property_map)[0xAC0A3831] = "m_ExpertFocus";
        (*property_map)[0x44A8DAD2] = "m_fAccidentDramaDelay";
        (*property_map)[0xEBA87A61] = "m_fAiAttentionModifier";
        (*property_map)[0xA120F69F] = "m_fAIEventDistance";
        (*property_map)[0x31FE4176] = "m_fAiPerceptionRange";
        (*property_map)[0x13431135] = "m_fArrestEscalateIllegalWeapon";
        (*property_map)[0x28D7A057] = "m_fArrestEscalateMaximum";
        (*property_map)[0x5D0952AA] = "m_fArrestEscalateMoving";
        (*property_map)[0x1D9BDC3E] = "m_fArrestEscalateRunning";
        (*property_map)[0x3A55997] = "m_fArrestTargetLostTime";
        (*property_map)[0x5BADEA50] = "m_fAttenuationRandomization";
        (*property_map)[0xA8B72C88] = "m_fBlendInMin";
        (*property_map)[0x9628FCD8] = "m_fCCChokePressCount_Civilian";
        (*property_map)[0x2FDA77A] = "m_fCCChokePressCount_Elite";
        (*property_map)[0x7A622AA7] = "m_fCCChokePressCount_Guard";
        (*property_map)[0xDBEC8B61] = "m_fCCShowSnapNeckPressCount_Civilian";
        (*property_map)[0x18CF6D8B] = "m_fCCShowSnapNeckPressCount_Elite";
        (*property_map)[0x6050E056] = "m_fCCShowSnapNeckPressCount_Guard";
        (*property_map)[0x932CA2C2] = "m_fColliderWeight";
        (*property_map)[0xC8FAB7B4] = "m_fCollisionNormalMaxSpeed";
        (*property_map)[0x8792CA88] = "m_fCollisionTangentMaxSpeed";
        (*property_map)[0xBA076E41] = "m_fCollisionThreshold";
        (*property_map)[0x5FEA35D9] = "m_fCondSillyTime";
        (*property_map)[0xBA3813FF] = "m_fCondStandingTime";
        (*property_map)[0xDDD4DFBC] = "m_fCooldownGreeting";
        (*property_map)[0xD1D4BD64] = "m_fCooldownGreetingAlertLevel";
        (*property_map)[0xF38A401D] = "m_fCooldownGreetingLastSpeak";
        (*property_map)[0x8FE8E0E8] = "m_fCooldownGreetingSequence";
        (*property_map)[0x270D01C2] = "m_fCooldownHitmanGreeting";
        (*property_map)[0x466F7619] = "m_fCooldownLookAt";
        (*property_map)[0x7C982EAF] = "m_fCooldownRunning";
        (*property_map)[0xAB7CDF72] = "m_fCooldownSilly";
        (*property_map)[0xFAADF1CF] = "m_fCooldownStanding";
        (*property_map)[0xD8CE14] = "m_fCurveStartFractionHead";
        (*property_map)[0xACE30EA5] = "m_fDamageMultiplier";
        (*property_map)[0xF5E6919F] = "m_fDamageRadius";
        (*property_map)[0xD991C50A] = "m_fDeathSleep";
        (*property_map)[0x6EACDEC2] = "m_fDefer";
        (*property_map)[0x9C74C4B2] = "m_fDirectionalAttenuation";
        (*property_map)[0x8B7C550C] = "m_fDirectionalAttenuationCurve";
        (*property_map)[0x9F719883] = "m_fDisguiseAttentionTimeFar";
        (*property_map)[0xAF29483B] = "m_fDisguiseAttentionTimeMid";
        (*property_map)[0xBB759D3E] = "m_fDisguiseAttentionTimeNear";
        (*property_map)[0xD6D81C68] = "m_fDisguiseAttractionDistanceFar";
        (*property_map)[0xE680CCD0] = "m_fDisguiseAttractionDistanceMid";
        (*property_map)[0x8CE40F4A] = "m_fDisguiseAttractionDistanceNear";
        (*property_map)[0x6ABF0844] = "m_fDistanceAttenuation";
        (*property_map)[0x1D266EBC] = "m_fDistCollision";
        (*property_map)[0xC410DF04] = "m_fDistGreeting";
        (*property_map)[0x1779B5B4] = "m_fDistLookAt";
        (*property_map)[0x666F2DA7] = "m_fDistLookAtAttention";
        (*property_map)[0xD4AEE739] = "m_fDistRunning";
        (*property_map)[0x63BF4843] = "m_fDistSilly";
        (*property_map)[0xE369F177] = "m_fDistStanding";
        (*property_map)[0x8A9FD464] = "m_fDurationSeconds";
        (*property_map)[0xC00A11BF] = "m_fEaseOutTime";
        (*property_map)[0xFB3FDD52] = "m_fEndMultiplier";
        (*property_map)[0x656E99] = "m_fETimeToEndEvent1";
        (*property_map)[0x996C3F23] = "m_fETimeToEndEvent2";
        (*property_map)[0xEE6B0FB5] = "m_fETimeToEndEvent3";
        (*property_map)[0xDA4FDAC9] = "m_fEventDistance";
        (*property_map)[0x539C9D95] = "m_fFixTime";
        (*property_map)[0xADF83E01] = "m_fFocusCostOnDamage";
        (*property_map)[0x639C9E94] = "m_fFocusMaxUseRate";
        (*property_map)[0x62900420] = "m_fFocusMinUseRate";
        (*property_map)[0x247AA49C] = "m_fFocusShotUseRate";
        (*property_map)[0x53CFDD3E] = "m_fGainAccidentKill";
        (*property_map)[0xA08DE5EE] = "m_fGainChokeOut";
        (*property_map)[0x6D7A01D2] = "m_fGainEnvironmentKill";
        (*property_map)[0x9B7925FA] = "m_fGainFiberwireKill";
        (*property_map)[0x8A4AB380] = "m_fGainFromDisguiseUnlock";
        (*property_map)[0x7E438545] = "m_fGainFromHealthStation";
        (*property_map)[0x7C05631A] = "m_fGainHeadShot";
        (*property_map)[0xA6A73264] = "m_fGainHideBody";
        (*property_map)[0x12B5AB5B] = "m_fGainItemKillBack";
        (*property_map)[0xBA8034EF] = "m_fGainItemKillFront";
        (*property_map)[0x80938C97] = "m_fGainItemThrowKill";
        (*property_map)[0xC017F793] = "m_fGainKnockOut";
        (*property_map)[0x16F14DD3] = "m_fGainNoKillsOnCP";
        (*property_map)[0x1B27A469] = "m_fGainObjectiveComplete";
        (*property_map)[0xF5A48BC2] = "m_fGainObjectiveCompleteInStealth";
        (*property_map)[0x762AA8CD] = "m_fGainPistolExecute";
        (*property_map)[0x41A57537] = "m_fGainSnapNeck";
        (*property_map)[0x937D6A6] = "m_fGainUnspottedOnCP";
        (*property_map)[0xF72AC33C] = "m_fGreetingChance";
        (*property_map)[0x5E838B74] = "m_fGreetingDelayInitial";
        (*property_map)[0x4E59E0BD] = "m_fGreetingDelayMax";
        (*property_map)[0x7254DFE4] = "m_fGreetingDelayMin";
        (*property_map)[0x844169CD] = "m_fHearRangeAccident";
        (*property_map)[0xAC8A736C] = "m_fHearRangeAlarm";
        (*property_map)[0x2A5B39FC] = "m_fHearRangeBulletImpact";
        (*property_map)[0xC0842B89] = "m_fHearRangeCCBreakNeck";
        (*property_map)[0x1C0824D8] = "m_fHearRangeCCChoke";
        (*property_map)[0x1F1531F5] = "m_fHearRangeCCFiberWire";
        (*property_map)[0xB90F4079] = "m_fHearRangeCCHitFromBehind";
        (*property_map)[0x76D46D61] = "m_fHearRangeCloseCombat";
        (*property_map)[0x80E81040] = "m_fHearRangeEmptyGun";
        (*property_map)[0x4944D36A] = "m_fHearRangeExplosion";
        (*property_map)[0x9225225B] = "m_fHearRangeFootstep";
        (*property_map)[0x8962E932] = "m_fHearRangeFootstepLimitVisionAreaRun";
        (*property_map)[0xB77F403C] = "m_fHearRangeFootstepLimitVisionAreaSneakWalk";
        (*property_map)[0xDEFBFD28] = "m_fHearRangeFootstepLimitVisionAreaWalk";
        (*property_map)[0x2F8C831D] = "m_fHearRangeGunShot";
        (*property_map)[0xE2FD175B] = "m_fHearRangeGunSilenced";
        (*property_map)[0x181A6D24] = "m_fHearRangeLoudFootstep";
        (*property_map)[0x38532EBE] = "m_fHearRangeRadio";
        (*property_map)[0xA7D27A14] = "m_fHearRangeSpeakAngry";
        (*property_map)[0xD34D03BD] = "m_fHearRangeSpeakPain";
        (*property_map)[0xACBE000C] = "m_fHearRangeSpeakWarning";
        (*property_map)[0x2E977484] = "m_fHearRangeStrange";
        (*property_map)[0x5DBA0DD8] = "m_fHearThrownItemCollision";
        (*property_map)[0xBE038828] = "m_fHideInPlainSight_GainCap";
        (*property_map)[0x3BD45AE1] = "m_fHitmanGreetingChance";
        (*property_map)[0x5CB97C12] = "m_fHuntedAttractionHumanShieldPenalty";
        (*property_map)[0xD92AA7AF] = "m_fIllegalWeaponTime";
        (*property_map)[0x900A3B60] = "m_fInitial";
        (*property_map)[0x7511D7AE] = "m_fInitialZoomFactor";
        (*property_map)[0x5B896A99] = "m_fInstinctMinFocus";
        (*property_map)[0x864A613A] = "m_fKillCivilanDrain";
        (*property_map)[0x5CEEC53E] = "m_fLeaveDelay";
        (*property_map)[0x11801D7] = "m_fLightSimplificationDistance";
        (*property_map)[0x52DA59D7] = "m_fLightSimplificationLength";
        (*property_map)[0xBFA39235] = "m_fLocalAmbientInscatterFraction";
        (*property_map)[0x906464AB] = "m_fLocalInscatterIntensity";
        (*property_map)[0xDF73F602] = "m_fLocalMieDepthScale";
        (*property_map)[0xAB5A50DB] = "m_fLocalMultipleScatteringHack";
        (*property_map)[0x3A1A3704] = "m_fLocalRayleighDepthScale";
        (*property_map)[0x1340C531] = "m_fLocalShadowIntensityMultiplier";
        (*property_map)[0x622CC7B4] = "m_fLocalShadowRange";
        (*property_map)[0x6D2450A8] = "m_fLookAtMinAttention";
        (*property_map)[0xCBD63345] = "m_fLookAtTimeMax";
        (*property_map)[0xF7DB0C1C] = "m_fLookAtTimeMin";
        (*property_map)[0x5AF0087B] = "m_fMaxDelay";
        (*property_map)[0x6577AA33] = "m_fMaxZoom";
        (*property_map)[0xCF1DABB0] = "m_fMinAccidentDuration";
        (*property_map)[0xE8B6208] = "m_fMinAngle";
        (*property_map)[0x8671477A] = "m_fMinFreeShardsThreshold";
        (*property_map)[0x9ED3575A] = "m_fMinInSightTime";
        (*property_map)[0xA3D28A35] = "m_fMinTimeForHitmanView";
        (*property_map)[0x66844A7C] = "m_fMinZoom";
        (*property_map)[0x662BB5B7] = "m_fMirrorGaussScale";
        (*property_map)[0xB868A759] = "m_fMultiplier";
        (*property_map)[0xCBF5A36C] = "m_fMuteGameFadeTime";
        (*property_map)[0x169EA6F3] = "m_fNextTriggerTime";
        (*property_map)[0x5C60B825] = "m_fObstructionRadius";
        (*property_map)[0xC4760B0A] = "m_fOcclusionAttenuation";
        (*property_map)[0xD4BA6E58] = "m_fOcclusionFactorXNeg";
        (*property_map)[0x2237DCD5] = "m_fOcclusionFactorXPos";
        (*property_map)[0x6C06093D] = "m_fOcclusionFactorYNeg";
        (*property_map)[0x9A8BBBB0] = "m_fOcclusionFactorYPos";
        (*property_map)[0x7EB3A6D3] = "m_fOcclusionFactorZNeg";
        (*property_map)[0x883E145E] = "m_fOcclusionFactorZPos";
        (*property_map)[0xC9A25842] = "m_fOcclusionLowpass";
        (*property_map)[0x4DA0AD7] = "m_fOcclusionLowPassFilter";
        (*property_map)[0x7C0C00AA] = "m_fOcclusionLowpassHigh";
        (*property_map)[0x936172BE] = "m_fOcclusionLowpassLow";
        (*property_map)[0x3CC766A3] = "m_fOverRelaxation";
        (*property_map)[0xA95B2CA8] = "m_fPanningStrength";
        (*property_map)[0xC2ACD965] = "m_fPeekDistance";
        (*property_map)[0x7E8A37B8] = "m_fPeripheralRange";
        (*property_map)[0xFD69B584] = "m_fPerishProbability";
        (*property_map)[0x87AAB235] = "m_fPitchRandomization";
        (*property_map)[0x198BF052] = "m_fPointShootingCoolDownTime";
        (*property_map)[0xB03CEEA6] = "m_fPointShootingTagTime";
        (*property_map)[0xF0B9FD7] = "m_fPreciseShotUseRate";
        (*property_map)[0x3502D393] = "m_fPrecisionShotMultiplier";
        (*property_map)[0x8EB73F] = "m_fRagdollImpactForce";
        (*property_map)[0xDBB9124F] = "m_fRecoveryAnimationTimer";
        (*property_map)[0xBC678202] = "m_fReleaseCond";
        (*property_map)[0x2BC5332F] = "m_fResponseChance";
        (*property_map)[0xA19AFC4F] = "m_fReturnDelay";
        (*property_map)[0xDDB8EE44] = "m_fReverbLevelFar";
        (*property_map)[0xBE37035B] = "m_fReverbLevelNear";
        (*property_map)[0xD54480D3] = "m_fReverbRadius";
        (*property_map)[0x33648878] = "m_fRoll";
        (*property_map)[0xAE27F859] = "m_fSBBurnRateA";
        (*property_map)[0x372EA9E3] = "m_fSBBurnRateB";
        (*property_map)[0xBE13683A] = "m_fSBDamageMultiplier";
        (*property_map)[0x39F6F388] = "m_fSBDamageToFocusFactor";
        (*property_map)[0xDB6FBAB3] = "m_fSBEnemyRadius";
        (*property_map)[0x9C588A12] = "m_fSBExecutionGameTime";
        (*property_map)[0x1D7E7069] = "m_fSBExecutionMultiplier";
        (*property_map)[0x64F8031B] = "m_fSBExecutionTimeMultiplier";
        (*property_map)[0xA2480BF9] = "m_fSBFocusToDamageRatio";
        (*property_map)[0x8878C7E2] = "m_fSBImpulseMultiplier";
        (*property_map)[0x24F41265] = "m_fSBInitFocus";
        (*property_map)[0x96342845] = "m_fSBMinimumFocus";
        (*property_map)[0x4303FF31] = "m_fSBMinTagDistance";
        (*property_map)[0x8D915DDF] = "m_fSBPropDamageCost";
        (*property_map)[0x3AFE164D] = "m_fSBRushZoomTimeMul";
        (*property_map)[0xB3C46FE1] = "m_fSBTagRampUpMultiplier";
        (*property_map)[0x70C2EDA4] = "m_fSBTagSequenceTimeMultiplier";
        (*property_map)[0x2CC9DA35] = "m_fSBTagSlotBonus";
        (*property_map)[0x584BF9BD] = "m_fSBTagSlotTime";
        (*property_map)[0xB321C1E] = "m_fSBTagTime";
        (*property_map)[0xF75F6EAC] = "m_fSBTagTimeRampUpFocus";
        (*property_map)[0x1956BAAB] = "m_fSBTagTimeRampUpFraction";
        (*property_map)[0x6A41F87D] = "m_fSBTimeAB";
        (*property_map)[0xBBFD93E2] = "m_fShatterEventCooldownTime";
        (*property_map)[0x6BD92A9E] = "m_fShootingBalletCooldown";
        (*property_map)[0x42591AEB] = "m_fSilencerGFXOffset";
        (*property_map)[0x1D4F196F] = "m_fSkyboxInscatterIntensity";
        (*property_map)[0x876FA428] = "m_fSkyboxMieDepthScale";
        (*property_map)[0xB7314AC0] = "m_fSkyboxRayleighDepthScale";
        (*property_map)[0xF6E0AE8F] = "m_fSniperSlowDownUseRate";
        (*property_map)[0xB5D0CB5D] = "m_fSplineFollowerSpeed";
        (*property_map)[0xB7FF0A0A] = "m_fSpotHitmanShootingDistance";
        (*property_map)[0xA4BBAEE0] = "m_fStartMultiplier";
        (*property_map)[0x744E9961] = "m_fTagModeTimeMultiplyerOverride";
        (*property_map)[0x1E1D3502] = "m_fTagTimeToEndEvent1";
        (*property_map)[0x871464B8] = "m_fTagTimeToEndEvent2";
        (*property_map)[0xF013542E] = "m_fTagTimeToEndEvent3";
        (*property_map)[0xD7F595E8] = "m_fTangentScale";
        (*property_map)[0x66793D1] = "m_fThicknessScale";
        (*property_map)[0x23ABC881] = "m_fTimeSinceLastGroan";
        (*property_map)[0xC6629DCA] = "m_fTrespassingAttractionHumanShieldPenalty";
        (*property_map)[0x18944467] = "m_fTurnLookAtMinAttention";
        (*property_map)[0x248B7272] = "m_fUnconsciousReviveTime";
        (*property_map)[0x458E7162] = "m_fUnconsciousTime";
        (*property_map)[0x2BEA08BE] = "m_fUnconsciousTimeOnThrow";
        (*property_map)[0x9A64A963] = "m_fUnconsciousTimerModifier";
        (*property_map)[0x2331F5F2] = "m_guardPointEntity";
        (*property_map)[0xFF888ACD] = "m_Hard";
        (*property_map)[0xEA456A04] = "m_HardAISensorDef";
        (*property_map)[0x8F788AFA] = "m_HardAISoundSensorDef";
        (*property_map)[0x4C62A892] = "m_HardFocus";
        (*property_map)[0xE390CFA9] = "m_HighlightColor";
        (*property_map)[0x9CF7209F] = "m_HighlightMaterialCounter";
        (*property_map)[0x510A440C] = "m_HighlightMaterialHigh";
        (*property_map)[0x97CA5941] = "m_HighlightMaterialLow";
        (*property_map)[0xB257B95C] = "m_HitEntity";
        (*property_map)[0xC8BAEB5A] = "m_HitmanOutfit";
        (*property_map)[0x8E1D84B8] = "m_HybridCurveSide0";
        (*property_map)[0xF91AB42E] = "m_HybridCurveSide1";
        (*property_map)[0xF26CFFD9] = "m_iAmmoSucking";
        (*property_map)[0xCD8AD751] = "m_iconType";
        (*property_map)[0x6F7A3E98] = "m_idleOverrideClips";
        (*property_map)[0xED499846] = "m_idScatterData";
        (*property_map)[0x2F64A16D] = "m_iIntValue";
        (*property_map)[0x1970F4AC] = "m_initValue";
        (*property_map)[0x8C8FC393] = "m_InterestingEntities";
        (*property_map)[0xC1653B] = "m_iRoomOcclusionPriority";
        (*property_map)[0xCB81E57B] = "m_iSBMaxNumTags";
        (*property_map)[0xA751EFEE] = "m_iSBTagSlots";
        (*property_map)[0x28BC53B5] = "m_iSpeak";
        (*property_map)[0xEEDA0907] = "m_ItemHands";
        (*property_map)[0xE62CCC97] = "m_Light";
        (*property_map)[0xDAC10B71] = "m_LightEffect";
        (*property_map)[0x9BE4B2F2] = "m_ListenerOrientation";
        (*property_map)[0x2491192B] = "m_ListenerPos";
        (*property_map)[0x8B4C127F] = "m_Medium";
        (*property_map)[0xF2EFA875] = "m_MediumAISensorDef";
        (*property_map)[0xC17A25E7] = "m_MediumAISoundSensorDef";
        (*property_map)[0x8364B8C3] = "m_MediumFocus";
        (*property_map)[0x5E830AA7] = "m_Mini";
        (*property_map)[0xE1E7D89A] = "m_MuzzleFlash";
        (*property_map)[0x76F6EE3] = "m_MuzzleSmoke";
        (*property_map)[0x22FB0D93] = "m_nActionWheelIndex";
        (*property_map)[0xC5BD6C2B] = "m_nActivateAngle";
        (*property_map)[0xCC2B4326] = "m_nActorChecksPerFrame";
        (*property_map)[0xA3909F09] = "m_nActorVariantIndex";
        (*property_map)[0x5BC8AAB8] = "m_nAlphaLocked";
        (*property_map)[0x87889FE2] = "m_nAudioBusFlags";
        (*property_map)[0x9EB11C5B] = "m_nCollisionEffectInstances";
        (*property_map)[0x14CE8F8C] = "m_nContainerSize";
        (*property_map)[0xF3337E89] = "m_nCrowdLODCount3";
        (*property_map)[0x4E366091] = "m_nCrowdLODDist3";
        (*property_map)[0x91F79B37] = "m_nCrowdSize";
        (*property_map)[0xE18D0E46] = "m_nGetUpPoseNodeId";
        (*property_map)[0xD2D13692] = "m_nHighlightFadeSpeed";
        (*property_map)[0x69A7D2AD] = "m_nHighlightPower";
        (*property_map)[0xB8FF0CCB] = "m_nInitialOutfitVariation";
        (*property_map)[0xCB228BF4] = "m_nInputDelay";
        (*property_map)[0xE00EEF3D] = "m_nInvestigationCount";
        (*property_map)[0x66B6B0F9] = "m_nInvestigationId";
        (*property_map)[0x6557F6] = "m_nLeaderTargetNodeIndex";
        (*property_map)[0xEFD3A2A2] = "m_nMaxCrowdAttentionMod";
        (*property_map)[0xDEA39545] = "m_nMaxCrowdAttentionRangeMod";
        (*property_map)[0xC6AD5FBA] = "m_nMinCrowdAttentionMod";
        (*property_map)[0xE0165DCD] = "m_nMinCrowdAttentionRangeMod";
        (*property_map)[0x7BE4EE6F] = "m_nMinOcclusion";
        (*property_map)[0x89BB7AB1] = "m_nNumberOfAmmo";
        (*property_map)[0x900B959C] = "m_Nodes";
        (*property_map)[0xD484A609] = "m_nOverrideStage1";
        (*property_map)[0x4D8DF7B3] = "m_nOverrideStage2";
        (*property_map)[0x3A8AC725] = "m_nOverrideStage3";
        (*property_map)[0xCF4FF253] = "m_nOverrideStageFlags";
        (*property_map)[0xC6A13DA0] = "m_nPanicDebrisFactor";
        (*property_map)[0xE649AFBA] = "m_nPanicDebrisTime";
        (*property_map)[0xD4B4534E] = "m_nPerishEffectInstances";
        (*property_map)[0x10FC6BC] = "m_nPlacementRadius";
        (*property_map)[0x9F80C509] = "m_nPlayCount";
        (*property_map)[0x851032FD] = "m_nPointShootingMaxTags";
        (*property_map)[0xA7B427B] = "m_nPositionMemberIndex";
        (*property_map)[0x100471] = "m_nProbeDistance";
        (*property_map)[0xDC88E5E3] = "m_nSBShotsThreshold";
        (*property_map)[0x62B85BB5] = "m_nSBTimeMultiplierDelay";
        (*property_map)[0x2884EF99] = "m_nSelected";
        (*property_map)[0x17C850DA] = "m_nShardEffectInstances";
        (*property_map)[0x88EF93A7] = "m_nShardPool";
        (*property_map)[0xA152C8B0] = "m_nShatterEffectInstances";
        (*property_map)[0xAF21269A] = "m_nSituationIndex";
        (*property_map)[0x6D7C140F] = "m_nSizeVariation";
        (*property_map)[0xF3991F41] = "m_nSubdivision";
        (*property_map)[0xF5DE8F7B] = "m_nTest";
        (*property_map)[0x7C8948E8] = "m_nTimeout";
        (*property_map)[0x64B619C1] = "m_nTotalDisturbances";
        (*property_map)[0x156ACC75] = "m_NudeOutfit";
        (*property_map)[0xFA60B45A] = "m_nVoiceVariation";
        (*property_map)[0xAD09555B] = "m_nWeight";
        (*property_map)[0x652EEE94] = "m_Occlusion";
        (*property_map)[0x53A3450F] = "m_OSDCam";
        (*property_map)[0xD7388BA7] = "m_OutfitResourceID";
        (*property_map)[0xD0106AE2] = "m_overrideClips";
        (*property_map)[0x40A6CAD9] = "m_pAbsolutionTextlists";
        (*property_map)[0xD36308A1] = "m_pAlertVolume";
        (*property_map)[0x3C5C4EF4] = "m_pAmbienceTransition";
        (*property_map)[0xD60F95F2] = "m_pAmbientTextureRID";
        (*property_map)[0x23F7898D] = "m_pAmmoProperties";
        (*property_map)[0xC2E2B901] = "m_pAnimLayersController";
        (*property_map)[0xB329BCB6] = "m_pAnnoyedTextureRID";
        (*property_map)[0x7E809812] = "m_pArrestingTextureRID";
        (*property_map)[0xD394B52C] = "m_pBaseVolume";
        (*property_map)[0xB23784E0] = "m_pBriefingVideoList";
        (*property_map)[0xD83FFC93] = "m_pCanBreakDisguise";
        (*property_map)[0xE6725543] = "m_pChainReactionProjectile";
        (*property_map)[0xFD7B8CDE] = "m_pCharacterResource";
        (*property_map)[0x499282F0] = "m_pCircle";
        (*property_map)[0x283F7839] = "m_pCircleFilled";
        (*property_map)[0x6EA8B68A] = "m_pCircleFilledOrange";
        (*property_map)[0xACDFD1C5] = "m_pCircleOrange";
        (*property_map)[0x605C6257] = "m_pCircleRed";
        (*property_map)[0xBD8B01FC] = "m_pCuriousTextureRID";
        (*property_map)[0xD8443FEB] = "m_pDialog";
        (*property_map)[0x18887474] = "m_pDialogueAudioEvent";
        (*property_map)[0xF44B6AC] = "m_pDiffuseVariationTexture1";
        (*property_map)[0x964DE716] = "m_pDiffuseVariationTexture2";
        (*property_map)[0xE14AD780] = "m_pDiffuseVariationTexture3";
        (*property_map)[0x5C278C43] = "m_pDisguiseBroken";
        (*property_map)[0xBA193B5] = "m_pDisguiseSuspicious";
        (*property_map)[0x75EE38D] = "m_PerishEffectID";
        (*property_map)[0x5CB9F503] = "m_pFightingTextureRID";
        (*property_map)[0xA0035D94] = "m_pFixAct";
        (*property_map)[0xFC5EA83A] = "m_pFontResource";
        (*property_map)[0x58D29C6C] = "m_pGettingHelpTextureRID";
        (*property_map)[0x6A9E99EA] = "m_pGreetingTextureRID";
        (*property_map)[0xA8087B72] = "m_pHelperEntrance";
        (*property_map)[0xE472C8D] = "m_pHelperEvent";
        (*property_map)[0x1B9E4E1C] = "m_pHuntingTextureRID";
        (*property_map)[0xB642631E] = "m_pInitialSoundAmbience";
        (*property_map)[0x11E3C86B] = "m_pInventoryController";
        (*property_map)[0xC6D5B5CC] = "m_pInventoryCtrl";
        (*property_map)[0x1E6B61A0] = "m_PlayingID";
        (*property_map)[0xEA1111AE] = "m_pLegendLabel";
        (*property_map)[0x3A400381] = "m_pLocoAnimDB";
        (*property_map)[0x4F2B633B] = "m_pMaterialResource0";
        (*property_map)[0x382C53AD] = "m_pMaterialResource1";
        (*property_map)[0xA1250217] = "m_pMaterialResource2";
        (*property_map)[0xD6223281] = "m_pMaterialResource3";
        (*property_map)[0x3B0952D6] = "m_pMetaData";
        (*property_map)[0x130DF0D8] = "m_pOptionTimer";
        (*property_map)[0xEFC5CC3C] = "m_pOSDRootNode";
        (*property_map)[0x5027D122] = "m_positionRequestMember";
        (*property_map)[0x1A7C6166] = "m_pPanicDebrisResource";
        (*property_map)[0x9A6761E6] = "m_pPlayAudioInputEvent";
        (*property_map)[0xF8CC1398] = "m_pProcessingMember";
        (*property_map)[0x664A42E7] = "m_pProjectile";
        (*property_map)[0x128BD2F1] = "m_pReactAnimDB";
        (*property_map)[0xA722F8B] = "m_pReactionActs";
        (*property_map)[0xE1686ED0] = "m_pRelationship";
        (*property_map)[0x827E4AEE] = "m_pReloadController";
        (*property_map)[0x8B877C60] = "m_pReloadCtrl";
        (*property_map)[0x22A2E8AC] = "m_pRenderDestEntity";
        (*property_map)[0xAD42AF55] = "m_pRenderDestination";
        (*property_map)[0xB99F2E3F] = "m_pRepairLocation";
        (*property_map)[0x28A250A1] = "m_pRicochetMatrix";
        (*property_map)[0x9077CA66] = "m_PrimitiveContainerEntity";
        (*property_map)[0x6CA2C72E] = "m_pSBReaction";
        (*property_map)[0x83B0B002] = "m_pScaredTextureRID";
        (*property_map)[0x81CBC589] = "m_pSearchingTextureRID";
        (*property_map)[0xAF8CE5EB] = "m_pSecondary";
        (*property_map)[0x29B87528] = "m_pShatterSystem";
        (*property_map)[0x3B59920] = "m_pSoundController";
        (*property_map)[0x5A7FE0AC] = "m_pStartState";
        (*property_map)[0xB8FEC000] = "m_pSurrenderReaction";
        (*property_map)[0x4F88F146] = "m_pSuspiciousTextureRID";
        (*property_map)[0xA8D36360] = "m_pUI";
        (*property_map)[0xF8C4CAEB] = "m_pUISpatial";
        (*property_map)[0xF2A16336] = "m_pUITimer";
        (*property_map)[0x13A793BD] = "m_Purist";
        (*property_map)[0x46EBEF89] = "m_PuristAISensorDef";
        (*property_map)[0xCDDD3B5F] = "m_PuristAISoundSensorDef";
        (*property_map)[0x597BED75] = "m_PuristFocus";
        (*property_map)[0x98E9E8A0] = "m_pVictim";
        (*property_map)[0x1F30FBE3] = "m_pVolumeMatrix";
        (*property_map)[0xF416406F] = "m_pWeapon";
        (*property_map)[0x93658B1C] = "m_pWeaponStatusControl";
        (*property_map)[0x76B78A28] = "m_rAccidentScreenplay";
        (*property_map)[0x26F375A8] = "m_rAccidentSetup";
        (*property_map)[0xEA26F0AF] = "m_rAudioEvent";
        (*property_map)[0x14CA7F2B] = "m_rContentItemKeys";
        (*property_map)[0x867FBA53] = "m_rCurveEntity";
        (*property_map)[0x608DFB86] = "m_rDeadBodyScreenplay";
        (*property_map)[0x254DFD9] = "m_rDoor";
        (*property_map)[0x96A3C52E] = "m_rDoorHighEnterSequence";
        (*property_map)[0xBA1FB241] = "m_rDoorHighExitBackwardsSequence";
        (*property_map)[0x8054B81B] = "m_rDoorHighExitForwardSequence";
        (*property_map)[0x77320EA7] = "m_rDoorLowEnterSequence";
        (*property_map)[0xFBFFCC48] = "m_rDoorLowExitBackwardsSequence";
        (*property_map)[0x8AF21897] = "m_rDoorLowExitForwardSequence";
        (*property_map)[0xCC61CCE8] = "m_RepairMen";
        (*property_map)[0xCC9A4D99] = "m_rFailSafeWeapon";
        (*property_map)[0x2B67C509] = "m_rGlobalOutfitKit";
        (*property_map)[0xAF9B9611] = "m_rHumanIKEntity";
        (*property_map)[0x48B7EF1B] = "m_rKeywordFaction";
        (*property_map)[0xECAC6B5D] = "m_rPeekPosition";
        (*property_map)[0xAE77181A] = "m_rPosToSpawnHitmanOn";
        (*property_map)[0xEF47C630] = "m_rPrimaryTarget";
        (*property_map)[0x16F39548] = "m_rReferenceTransform";
        (*property_map)[0x5C153256] = "m_rTestDynamicEntityReference";
        (*property_map)[0xA344D5BB] = "m_rTestLostEntityReference";
        (*property_map)[0xE04B3FFC] = "m_rTestStaticEntityReference";
        (*property_map)[0xDF566DDE] = "m_rTestTemplatedEntityReference";
        (*property_map)[0xC48DDCEB] = "m_rThrownItem";
        (*property_map)[0x866BAB40] = "m_runtimeValue";
        (*property_map)[0x5AB516F] = "m_rVent";
        (*property_map)[0xFBF782F1] = "m_rVoiceVariation";
        (*property_map)[0x438E9CC3] = "m_sAnimationName";
        (*property_map)[0xB9A97576] = "m_SBCamHitmanTargetProfiles";
        (*property_map)[0x86B32DEC] = "m_SBCamHitmanViewProfiles";
        (*property_map)[0x9764B96D] = "m_SBCamTargetProfiles";
        (*property_map)[0x17113C56] = "m_SBConfigurationBulletImpact";
        (*property_map)[0xC25E7219] = "m_SBConfigurationBulletImpactPre";
        (*property_map)[0x728D9E4A] = "m_SBConfigurationHitmanView";
        (*property_map)[0xA1B6C9EE] = "m_SBConfigurationLastKill";
        (*property_map)[0x23A52EEB] = "m_SBConfigurationShot";
        (*property_map)[0x90FAC3ED] = "m_SBMaterialID";
        (*property_map)[0x8036FEC8] = "m_scenarioFilePtr";
        (*property_map)[0x23B7A976] = "m_SettingsParamMultipliers";
        (*property_map)[0xFC4E91E5] = "m_sFailSafeDisarmWeaponid";
        (*property_map)[0xCC0E8E57] = "m_ShardEffectID";
        (*property_map)[0xB12B4165] = "m_ShatterEffectID";
        (*property_map)[0xC8998D0B] = "m_ShatterResourceID";
        (*property_map)[0x9D037F9C] = "m_ShatterShards";
        (*property_map)[0x19354A65] = "m_sItemId";
        (*property_map)[0xA32A42A8] = "m_sLeftAnalogTriggerName";
        (*property_map)[0xACE7D7AB] = "m_sLeftAnalogXName";
        (*property_map)[0x9187FE1B] = "m_sLeftAnalogYName";
        (*property_map)[0x28529CF4] = "m_sLegendIconId";
        (*property_map)[0x639E3FC7] = "m_SocialSensorDef";
        (*property_map)[0xEEBFC0B9] = "m_SpatialPosOnVent";
        (*property_map)[0x6B27BCFB] = "m_SpeechState";
        (*property_map)[0x11738DE1] = "m_sRightAnalogTriggerName";
        (*property_map)[0x9944AC6E] = "m_sRightAnalogXName";
        (*property_map)[0xA42485DE] = "m_sRightAnalogYName";
        (*property_map)[0xACCCEF88] = "m_sSlotID";
        (*property_map)[0x68CD48BA] = "m_sTrackName";
        (*property_map)[0xBCE8E849] = "m_sTriggerGroup";
        (*property_map)[0xD6AB288D] = "m_TextLineResource";
        (*property_map)[0xC7DC3B] = "m_TextureBlockSize";
        (*property_map)[0xBC6BF77C] = "m_tGroupTime";
        (*property_map)[0xF8BF62AD] = "m_tHitmanBizarre";
        (*property_map)[0x24F1D287] = "m_tLastBump";
        (*property_map)[0x26CCEEE8] = "m_tLastComment";
        (*property_map)[0x59CD5B95] = "m_tLastPositionRequest";
        (*property_map)[0xD6552083] = "m_tLastWeaponFiredTime";
        (*property_map)[0x94D0FC7D] = "m_tResolveStartTime";
        (*property_map)[0x339200A3] = "m_tStartTime";
        (*property_map)[0xC1A298B5] = "m_vArmMovementHorizontal";
        (*property_map)[0x66003B1B] = "m_vArmMovementVertical";
        (*property_map)[0x2433C238] = "m_vBulletCountPos";
        (*property_map)[0x5807AC8C] = "m_vHighlightSize";
        (*property_map)[0x9357F238] = "m_vLowerRight";
        (*property_map)[0xF4A59A16] = "m_Volume";
        (*property_map)[0xD938B602] = "m_vPositionInCover";
        (*property_map)[0x56A4CB93] = "m_vScaleNormal";
        (*property_map)[0xB708D5C4] = "m_vScaleSelected";
        (*property_map)[0x6DD28509] = "m_vTagSlotLowerRight";
        (*property_map)[0xD492CFA2] = "m_vTagSlotUpperLeft";
        (*property_map)[0x85FE883A] = "m_vUpperLeft";
        (*property_map)[0x49E4B9D8] = "m_WwiseEventResource";
        (*property_map)[0xE6DEECA6] = "MakeLoopSafe";
        (*property_map)[0x94C06EA2] = "mat";
        (*property_map)[0xAC72EFF9] = "nAutoReviveDisableCount";
        (*property_map)[0x40C0815D] = "nHaveRevision";
        (*property_map)[0x673D1D0B] = "nHeadChange";
        (*property_map)[0x22107C0D] = "nHeadRevision";
        (*property_map)[0x751DDF] = "nNeighbor6";
        (*property_map)[0xE33AB1E5] = "OptimizeSampleRate";
        (*property_map)[0x6D9CD615] = "otherLockUser";
        (*property_map)[0x6C3C51BC] = "otherOpenUser";
        (*property_map)[0x4626698] = "otherSubmitLockUser";
        (*property_map)[0x21AE908A] = "PackInterleaved";
        (*property_map)[0xCC0E2FD1] = "PlatformOverrides";
        (*property_map)[0x74CE6A8B] = "PlayingAnimationsBase64";
        (*property_map)[0x464137F5] = "pObstacle";
        (*property_map)[0xB30F328C] = "Quality";
        (*property_map)[0x64E33CC] = "RepeatCountLimits";
        (*property_map)[0x136AC113] = "result";
        (*property_map)[0xDB5D118E] = "rItem";
        (*property_map)[0x2820DB73] = "rSetpiece";
        (*property_map)[0x7DBB26BA] = "sErrorMessage";
        (*property_map)[0x42941719] = "SignatureBase64";
        (*property_map)[0xAF5136A7] = "sOutputMessage";
        (*property_map)[0x490BC305] = "standdown";
        (*property_map)[0x9EBEB2A9] = "string";
        (*property_map)[0x69FCEBFA] = "sUpheldContract";
        (*property_map)[0xF143BFAD] = "variant";
    }
}

void initialize_enum_map()
{
    if (!enum_map_initialized)
    {
        enum_map_initialized = true;

        enum_map = new std::map<std::string, std::map<uint32_t, std::string>>();

        (*enum_map)["EItemMeleeDamageBehavior"] = std::map<uint32_t, std::string>{
        { 0, "EIMDB_Undefined" },
        { 1, "EIMDB_Bouncing" },
        { 2, "EIMDB_Slashing" },
        { 3, "EIMDB_Sticking" }
        };

        (*enum_map)["ZStateControllerEntity.WrapMode"] = std::map<uint32_t, std::string>{
            { 0, "WM_Loop" },
            { 1, "WM_Clamp" }
        };

        (*enum_map)["ZFriskSuspectGroup.EAssistantState"] = std::map<uint32_t, std::string>{
            { 0, "AS_Waiting" },
            { 1, "AS_Approach" },
            { 2, "AS_Approaching" },
            { 3, "AS_Covering" }
        };

        (*enum_map)["ZRenderPostfilterParametersEntity.EHDRGlareType"] = std::map<uint32_t, std::string>{
            { 0, "eCamera" },
            { 1, "eNatural" },
            { 2, "eCheapLens" },
            { 3, "eFilterCrossScreen" },
            { 4, "eFilterCrossScreenSpectral" },
            { 5, "eFilterSnowCross" },
            { 6, "eFilterSnowCrossSpectral" },
            { 7, "eFilterSunnyCross" },
            { 8, "eFilterSunnyCrossSpectral" },
            { 9, "eCinecamVerticalSlits" },
            { 10, "eCinecamHorizontalSlits" }
        };

        (*enum_map)["ZOperatorBool.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["EGSEvent"] = std::map<uint32_t, std::string>{
            { 0, "GSEvent_StrayBullet" },
            { 1, "GSEvent_TookDamage" },
            { 2, "GSEvent_UsedCover" },
            { 3, "GSEvent_UsedRun" },
            { 4, "GSEvent_UsedEliminate" },
            { 5, "GSEvent_UsedChokeOut" },
            { 6, "GSEvent_UsedKnockOutByHand" },
            { 7, "GSEvent_Pacified" },
            { 8, "GSEvent_TargetPacified" },
            { 9, "GSEvent_Recover" },
            { 10, "GSEvent_CaughtArmed" },
            { 11, "GSEvent_BodyFound" },
            { 12, "GSEvent_InvestigateCurious" },
            { 13, "GSEvent_NPCKilled" },
            { 14, "GSEvent_KilledNPCHidden" },
            { 15, "GSEvent_InvestigateCautious" },
            { 16, "GSEvent_PrecisionAimedKill" },
            { 17, "GSEvent_BlendInActivated" },
            { 18, "GSEvent_PropKill" },
            { 19, "GSEvent_SpottedThroughDisguise" },
            { 20, "GSEvent_TakeOutNonLethal" },
            { 21, "GSEvent_TargetKilled" },
            { 22, "GSEvent_NonTargetKilled" },
            { 23, "GSEvent_AllTargetsKilled" },
            { 24, "GSEvent_AccidentKill" },
            { 25, "GSEvent_ObjectiveUpdate" },
            { 26, "GSEvent_TargetAccidentKill" },
            { 27, "GSEvent_ObjectiveActivate" },
            { 28, "GSEvent_Headshot" },
            { 29, "GSEvent_Fiberwire" },
            { 30, "GSEvent_UsedEliminateSilent" },
            { 31, "GSEvent_ItemFound" },
            { 32, "GSEvent_ItemStashed" },
            { 33, "GSEvent_BodyBagPlaced" },
            { 34, "GSEvent_Projectile_HeadShot" },
            { 35, "GSEvent_Projectile_BodyShot" },
            { 36, "GSEvent_Projectile_Crowd_HeadShot" },
            { 37, "GSEvent_Projectile_Crowd_BodyShot" },
            { 38, "GSEvent_Projectile_Missed" },
            { 39, "GSEvent_Projectile_MultiKillTargets" }
        };

        (*enum_map)["ZCautiousInvestigateGroup.EApproachOrderState"] = std::map<uint32_t, std::string>{
            { 0, "AOS_RequestHuntApproachNode" },
            { 1, "AOS_RequestLeaderApproachNode" },
            { 2, "AOS_WaitForApproachNode" },
            { 3, "AOS_RequestLOSNode" },
            { 4, "AOS_Ready" },
            { 5, "AOS_NoApproachNode" }
        };

        (*enum_map)["EGSKillSituation"] = std::map<uint32_t, std::string>{
            { 0, "GSKILLSITUATION_UNKNOWN" },
            { 1, "GSKILLSITUATION_CLOSECOMBAT" },
            { 2, "GSKILLSITUATION_SILENT" },
            { 3, "GSKILLSITUATION_ENVIRONMENT" },
            { 4, "GSKILLSITUATION_ACTION" },
            { 5, "GSKILLSITUATION_DEADLYTHROW" },
            { 6, "GSKILLSITUATION_PULLTHROUGHWINDOW" },
            { 7, "GSKILLSITUATION_PUSHOVERRAILING" },
            { 8, "GSKILLSITUATION_PACIFY" },
            { 9, "GSKILLSITUATION_FIBERWIRE" }
        };

        (*enum_map)["ZHUDCamera3DControllerEntity.ESpace"] = std::map<uint32_t, std::string>{
            { 0, "View" },
            { 1, "Camera" },
            { 2, "Player" },
            { 3, "World" }
        };

        (*enum_map)["ETessellationMode"] = std::map<uint32_t, std::string>{
            { 0, "TESSELLATIONMODE_FLAT" },
            { 1, "TESSELLATIONMODE_PHONG" }
        };

        (*enum_map)["EBystanderPointType"] = std::map<uint32_t, std::string>{
            { 0, "BPT_Scared" },
            { 1, "BPT_Alerted" }
        };

        (*enum_map)["_EUIOptionKey"] = std::map<uint32_t, std::string>{
            { 2001, "UI_OPTION_GRAPHICS_SUBTITLES_SIZE" },
            { 220, "UI_OPTION_GAME_INVERT_INVENTORY_EMOTE" },
            { 100, "UI_OPTION_GAME_VIBRATION" },
            { 365, "UI_OPTION_GAME_AID_VITAL_INFO" },
            { 301, "UI_OPTION_GAME_INVERT_MOUSE_X" },
            { 2620, "UI_OPTION_GRAPHICS_SHADOW_RESOLUTION" },
            { 3003, "UI_OPTION_VR_HEAD_BASED_MOVEMENT" },
            { 110, "UI_OPTION_GAME_ADAPTIVE_FEEDBACK" },
            { 373, "UI_OPTION_GAME_MINI_MAP_SHOW_TARGETS" },
            { 1060, "UI_OPTION_SOUND_MUTE_OTHER_PLAYER" },
            { 200, "UI_OPTION_GAME_AIM_ASSIST" },
            { 210, "UI_OPTION_GAME_AIM_CAUSAL" },
            { 364, "UI_OPTION_GAME_AID_ATTENTION" },
            { 300, "UI_OPTION_GAME_INVERT_X" },
            { 2010, "UI_OPTION_GRAPHICS_SUBTITLES_VR" },
            { 3004, "UI_OPTION_VR_FADE_ON_COLLISION" },
            { 2747, "UI_OPTION_GRAPHICS_VRS" },
            { 366, "UI_OPTION_GAME_AID_INTERACTION_H" },
            { 2310, "UI_OPTION_DISPLAY_STEREOSCOPIC" },
            { 302, "UI_OPTION_GAME_INVERT_MOUSE_Y" },
            { 2771, "UI_OPTION_GSM_AUTHORITY_END_MARKER" },
            { 374, "UI_OPTION_GAME_CHALLENGES_FILTER" },
            { 310, "UI_OPTION_GAME_INVERT_Y" },
            { 2000, "UI_OPTION_GRAPHICS_SUBTITLES" },
            { 1220, "UI_OPTION_SOUND_DYNAMIC_RANGE_MODE" },
            { 375, "UI_OPTION_GAME_AID_MISSION_TIMER" },
            { 2240, "UI_OPTION_DISPLAY_REFRESHRATE" },
            { 311, "UI_OPTION_GAME_CONTROL_SCHEME" },
            { 376, "UI_OPTION_GAME_DIFFICULTY_LEVEL_HUD" },
            { 2320, "UI_OPTION_DISPLAY_STEREO_DEPTH" },
            { 312, "UI_OPTION_GAME_FAST_TARGET" },
            { 385, "UI_OPTION_GAME_AID_AIM_TOGGLE" },
            { 2210, "UI_OPTION_GRAPHICS_SAFE_AREA_X" },
            { 321, "UI_OPTION_GAME_DIFFICULTY" },
            { 2743, "UI_OPTION_GRAPHICS_MOTION_BLUR" },
            { 330, "UI_OPTION_GAME_TRIGGER_SHOOT" },
            { 3006, "UI_OPTION_VR_TURN_SPEED" },
            { 332, "UI_OPTION_GAME_SWITCH_SNEAK_AND_CAMERA" },
            { 333, "UI_OPTION_GAME_LEFT_HANDED_CONTROLS" },
            { 334, "UI_OPTION_GAME_CAMERA_SENSITIVITY_HORZ" },
            { 2610, "UI_OPTION_GRAPHICS_SHADOW_QUALITY" },
            { 3001, "UI_OPTION_VR_BLINDERS_STRENGTH" },
            { 335, "UI_OPTION_GAME_CAMERA_SENSITIVITY_VERT" },
            { 400, "UI_OPTION_GAME_AUTOSAVE_HUD" },
            { 336, "UI_OPTION_GAME_MOUSE_SENSITIVITY" },
            { 2600, "UI_OPTION_GRAPHICS_QUALITY" },
            { 2011, "UI_OPTION_GRAPHICS_SUBTITLES_SIZE_VR" },
            { 337, "UI_OPTION_GAME_AIM_SENSITIVITY" },
            { 338, "UI_OPTION_GAME_CAMERA_SENSITIVITY" },
            { 340, "UI_OPTION_GAME_INSTINCT_PATHS" },
            { 342, "UI_OPTION_GAME_INSTINCT_NPCGLOW" },
            { 3000, "UI_OPTION_VR_BLINDERS_ENABLED" },
            { 346, "UI_OPTION_GAME_HINTS_TUTORIAL" },
            { 348, "UI_OPTION_GAME_HINTS_INSTINCT" },
            { 350, "UI_OPTION_GAME_LANG_AUDIO" },
            { 2690, "UI_OPTION_GRAPHICS_MIRRORS" },
            { 351, "UI_OPTION_GAME_LANG_TEXT" },
            { 352, "UI_OPTION_GAME_COVER_TOGGLE" },
            { 2680, "UI_OPTION_GRAPHICS_TESSELLATION" },
            { 353, "UI_OPTION_GAME_FIXED_MAP" },
            { 354, "UI_OPTION_GAME_MAP_SHOW_NORTH_INDICATOR" },
            { 1221, "UI_OPTION_SOUND_DYNAMIC_RANGE_MODE_VR" },
            { 360, "UI_OPTION_GAME_AID_OPPORTUNITIES" },
            { 1010, "UI_OPTION_SOUND_VOLUME_MASTER" },
            { 3007, "UI_OPTION_VR_MINIMUM_REPROJECTION_COLOR" },
            { 361, "UI_OPTION_GAME_AID_INSTINCT" },
            { 2775, "UI_OPTION_GRAPHICS_DIFFUSE_COLOR_CLAMP" },
            { 362, "UI_OPTION_GAME_AID_MINI_MAP" },
            { 2260, "UI_OPTION_DISPLAY_EXCLUSIVE" },
            { 1040, "UI_OPTION_SOUND_VOLUME_DIALOGUE" },
            { 363, "UI_OPTION_GAME_AID_NPC_ICONS" },
            { 367, "UI_OPTION_GAME_AID_OBJECTIVES" },
            { 2770, "UI_OPTION_GRAPHICS_FRAMERATE_LIMIT" },
            { 368, "UI_OPTION_GAME_AID_WEAPON_HUD" },
            { 1030, "UI_OPTION_SOUND_VOLUME_MUSIC" },
            { 369, "UI_OPTION_GAME_AID_CHALLENGEDESCRIPTION" },
            { 2760, "UI_OPTION_GRAPHICS_GAMMA" },
            { 370, "UI_OPTION_GAME_MINI_MAP_SHOW_NPCS" },
            { 371, "UI_OPTION_GAME_AID_TARGET_INFO" },
            { 2710, "UI_OPTION_GRAPHICS_LOD" },
            { 372, "UI_OPTION_GAME_AID_CHALLENGE_HUD" },
            { 377, "UI_OPTION_GAME_AID_GLOBAL_HINTS" },
            { 378, "UI_OPTION_GAME_SCORE_HUD" },
            { 379, "UI_OPTION_GAME_AID_LVA" },
            { 380, "UI_OPTION_GAME_AID_PICTURE_IN_PICTURE" },
            { 2270, "UI_OPTION_DISPLAY_VSYNC" },
            { 1050, "UI_OPTION_SOUND_MUTE_MICROPHONE" },
            { 381, "UI_OPTION_GAME_AID_INTERACTION_PROMPT" },
            { 2772, "UI_OPTION_GRAPHICS_HDR_GAMMA" },
            { 382, "UI_OPTION_GAME_AID_SA_HUD" },
            { 383, "UI_OPTION_GAME_AID_RELOAD_HUD" },
            { 384, "UI_OPTION_GAME_AID_CAMERA_GRID" },
            { 386, "UI_OPTION_GAME_AID_OBJECTIVES_VR" },
            { 387, "UI_OPTION_GAME_AID_WEAPON_HUD_VR" },
            { 388, "UI_OPTION_GAME_AID_RELOAD_HUD_VR" },
            { 1020, "UI_OPTION_SOUND_VOLUME_EFFECTS" },
            { 390, "UI_OPTION_GAME_AUTOSAVE" },
            { 2650, "UI_OPTION_GRAPHICS_ASSAO_QUALITY" },
            { 391, "UI_OPTION_GAME_AUTOSAVE_VR" },
            { 1200, "UI_OPTION_SOUND_OUTPUTMODE" },
            { 1210, "UI_OPTION_SOUND_SIMULATION_QUALITY" },
            { 3005, "UI_OPTION_VR_TURN_RATE" },
            { 2742, "UI_OPTION_GRAPHICS_REFLECTION_QUALITY" },
            { 2220, "UI_OPTION_GRAPHICS_SAFE_AREA_Y" },
            { 2221, "UI_OPTION_GSM_AUTHORITY_BEGIN_MARKER" },
            { 2230, "UI_OPTION_DISPLAY_RESOLUTION" },
            { 2250, "UI_OPTION_DISPLAY_FULLSCREEN" },
            { 2280, "UI_OPTION_DISPLAY_MONITOR" },
            { 2290, "UI_OPTION_DISPLAY_QUALITY" },
            { 2630, "UI_OPTION_GRAPHICS_TEXTURE_QUALITY" },
            { 2300, "UI_OPTION_DISPLAY_ASPECT" },
            { 2330, "UI_OPTION_DISPLAY_STEREO_STRENGTH" },
            { 2640, "UI_OPTION_GRAPHICS_TEXTURE_FILTER" },
            { 2660, "UI_OPTION_GRAPHICS_VSYNC" },
            { 2720, "UI_OPTION_GRAPHICS_MOTIONBLUR" },
            { 2740, "UI_OPTION_GRAPHICS_BOKEH" },
            { 2744, "UI_OPTION_GRAPHICS_DYNAMIC_SHARPENING" },
            { 2745, "UI_OPTION_GRAPHICS_SIMULATION_QUALITY" },
            { 2746, "UI_OPTION_GRAPHICS_SSR" },
            { 2750, "UI_OPTION_GRAPHICS_SUPER_SAMPLING" },
            { 2773, "UI_OPTION_GRAPHICS_RENDERING_QUALITY" },
            { 2774, "UI_OPTION_GRAPHICS_NEUTRAL_LUT_BLEND" },
            { 3002, "UI_OPTION_VR_TURN_MODE" },
            { 3008, "UI_OPTION_VR_LEFT_HAND_FOLLOW_RIGHT" },
            { 3009, "UI_OPTION_VR_SAFE_AREA_ENABLED" }
        };

        (*enum_map)["EVRConfigCameraRotationAllowed"] = std::map<uint32_t, std::string>{
            { 0, "EVRCCRA_RotationEnabled" },
            { 1, "EVRCCRA_RotationDisabled" },
            { 2, "EVRCCRA_KeepCurrent" }
        };

        (*enum_map)["ZInteractionGuideData.EDirection"] = std::map<uint32_t, std::string>{
            { 0, "UP" },
            { 1, "DOWN" },
            { 2, "LEFT" },
            { 3, "RIGHT" }
        };

        (*enum_map)["ZInteractionData.EFilterResult"] = std::map<uint32_t, std::string>{
            { 0, "EFR_REQUESTED" },
            { 1, "EFR_ENABLE_INTERACTION" },
            { 2, "EFR_EXPANDED_NOT_INTERACTABLE" },
            { 3, "EFR_COLLAPSED_CANT_OPERATE" },
            { 4, "EFR_COLLAPSED_PRIORITY" },
            { 5, "EFR_COLLAPSED_GAMESTATE" },
            { 6, "EFR_COLLAPSED_ACTION_FILTER" },
            { 7, "EFR_REJECTED_OTHER" },
            { 8, "EFR_REJECTED_OUTOFRANGE" },
            { 9, "EFR_REJECTED_OCCLUDED" },
            { 10, "EFR_REJECTED_OUTOFVIEW" },
            { 11, "EFR_REJECTED_GAMESTATE" },
            { 12, "EFR_REJECTED_ACTION_FILTER" },
            { 13, "EFR_REJECTED_INVALIDDATA" },
            { 14, "EFR_REJECTED_PRIORITY" }
        };

        (*enum_map)["ZHeroEscortSituation2Entity.ETargetState"] = std::map<uint32_t, std::string>{
            { 0, "ETS_Unknown" },
            { 1, "ETS_NoTarget" },
            { 2, "ETS_RunningActBehavior" },
            { 3, "ETS_RunningDummyBehavior" },
            { 4, "ETS_RunningOtherBehavior" },
            { 5, "ETS_Dead" },
            { 6, "ETS_TargetIsHitman" }
        };

        (*enum_map)["EOutfitAICategory"] = std::map<uint32_t, std::string>{
            { 0, "OAC_Undefined" },
            { 1, "OAC_Fallback" },
            { 2, "OAC_47Suit" },
            { 3, "OAC_47TRAINING" },
            { 4, "OAC_47MARRAKESH" },
            { 5, "OAC_47SAPIENZA" },
            { 6, "OAC_47BANGKOK" },
            { 7, "OAC_47COLORADO" },
            { 8, "OAC_47HOKKAIDO" },
            { 9, "OAC_47PREORDER" },
            { 10, "OAC_47COLUMBIA" },
            { 11, "OAC_47ISLAND" },
            { 12, "OAC_47MUMBAI" },
            { 13, "OAC_47NEWZEALAND" },
            { 14, "OAC_47STARTCLASSY" },
            { 15, "OAC_47STARTOUTFIT" },
            { 16, "OAC_47SUBURBIA" },
            { 17, "OAC_Bodyguard" },
            { 18, "OAC_CameraMan" },
            { 19, "OAC_Chef" },
            { 20, "OAC_Cleaner" },
            { 21, "OAC_Cop" },
            { 22, "OAC_Crew" },
            { 23, "OAC_DaSilva" },
            { 24, "OAC_Gardner" },
            { 25, "OAC_Hazmat" },
            { 26, "OAC_HouseStaff" },
            { 27, "OAC_Maintenance" },
            { 28, "OAC_Officer" },
            { 29, "OAC_Priest" },
            { 30, "OAC_Printer" },
            { 31, "OAC_Scientist" },
            { 32, "OAC_Security" },
            { 33, "OAC_Sheik" },
            { 34, "OAC_Stylist" },
            { 35, "OAC_Waiter" },
            { 36, "OAC_Busker" },
            { 37, "OAC_Caddie" },
            { 38, "OAC_ChurchStaff" },
            { 39, "OAC_Cyclist" },
            { 40, "OAC_Filmcrew" },
            { 41, "OAC_Fortune" },
            { 42, "OAC_Hippie" },
            { 43, "OAC_Investor" },
            { 44, "OAC_KGB" },
            { 45, "OAC_Lawyer" },
            { 46, "OAC_Kruger" },
            { 47, "OAC_Masseur" },
            { 48, "OAC_Mechanic" },
            { 49, "OAC_Norfolk" },
            { 50, "OAC_Plague" },
            { 51, "OAC_Psych" },
            { 52, "OAC_Shopkeep" },
            { 53, "OAC_Soldier" },
            { 54, "OAC_Tux" },
            { 55, "OAC_Vampire" },
            { 56, "OAC_YachtCrew" },
            { 57, "OAC_Exterminator" },
            { 58, "OAC_SoundCrew" },
            { 59, "OAC_Intern" },
            { 60, "OAC_Stalker" },
            { 61, "OAC_Scarecrow" },
            { 62, "OAC_Hacker" },
            { 63, "OAC_SpecOps" },
            { 64, "OAC_Berg" },
            { 65, "OAC_Morgue" },
            { 66, "OAC_Surgeon" },
            { 67, "OAC_Doctor" },
            { 68, "OAC_Director" },
            { 69, "OAC_Ninja" },
            { 70, "OAC_Baseball" },
            { 71, "OAC_KillBill" },
            { 72, "OAC_Pilot" },
            { 73, "OAC_Yoga" },
            { 74, "OAC_Mummy" },
            { 75, "OAC_Cowboy" },
            { 76, "OAC_Staff" },
            { 77, "OAC_Santa" },
            { 78, "OAC_MambaCrew" },
            { 79, "OAC_Delivery" },
            { 80, "OAC_GOTY_Clown" },
            { 81, "OAC_GOTY_Cowboy" },
            { 82, "OAC_GOTY_DarkSniper" },
            { 83, "OAC_Driver" },
            { 84, "OAC_Driverpale" },
            { 85, "OAC_Eventstff" },
            { 86, "OAC_Flrdaman" },
            { 87, "OAC_Food" },
            { 88, "OAC_Journal" },
            { 89, "OAC_Krnstdtengnr" },
            { 90, "OAC_Krnstdtsec" },
            { 91, "OAC_Ktchstff" },
            { 92, "OAC_Lee" },
            { 93, "OAC_Mascot" },
            { 94, "OAC_Medic" },
            { 95, "OAC_Mendez" },
            { 96, "OAC_Musician" },
            { 97, "OAC_Racecoord" },
            { 98, "OAC_Racemarsh" },
            { 99, "OAC_Thug" },
            { 100, "OAC_Worker" },
            { 101, "OAC_Research" },
            { 102, "OAC_Tatoo" },
            { 103, "OAC_Shaman" },
            { 104, "OAC_Artist" },
            { 105, "OAC_Barber" },
            { 106, "OAC_DJ" },
            { 107, "OAC_Kshmrn" },
            { 108, "OAC_Master" },
            { 109, "OAC_Burial" },
            { 110, "OAC_Ravenmaster" },
            { 111, "OAC_Sentinel" },
            { 112, "OAC_Blake" },
            { 113, "OAC_Actor" },
            { 114, "OAC_Architect" },
            { 115, "OAC_Arkian" },
            { 116, "OAC_Arkptrn" },
            { 117, "OAC_Bbq" },
            { 118, "OAC_Bollycrew" },
            { 119, "OAC_Cavegd" },
            { 120, "OAC_Civilian" },
            { 121, "OAC_Cavewkr" },
            { 122, "OAC_Clothsale" },
            { 123, "OAC_Counsellor" },
            { 124, "OAC_Cowboyhat" },
            { 125, "OAC_Custdn" },
            { 126, "OAC_Dancer" },
            { 127, "OAC_Dbbwll" },
            { 128, "OAC_Deadjanus" },
            { 129, "OAC_Driverch" },
            { 130, "OAC_Driversa" },
            { 131, "OAC_Driveruk" },
            { 132, "OAC_Driverus" },
            { 133, "OAC_Druglabwkr" },
            { 134, "OAC_Elite" },
            { 135, "OAC_Fakemlstrm" },
            { 136, "OAC_Farm" },
            { 137, "OAC_Fieldgrd" },
            { 138, "OAC_Headmaster" },
            { 139, "OAC_Knight" },
            { 140, "OAC_Laundrywkr" },
            { 141, "OAC_Lndrygrd" },
            { 142, "OAC_Mailman" },
            { 143, "OAC_Mansiongd" },
            { 144, "OAC_Mech_Miami" },
            { 145, "OAC_Mechch" },
            { 146, "OAC_Mechit" },
            { 147, "OAC_Mechkrnstdt" },
            { 148, "OAC_Mechsa" },
            { 149, "OAC_Mechuk" },
            { 150, "OAC_Mechus" },
            { 151, "OAC_Metalwkr" },
            { 152, "OAC_Militiasec" },
            { 153, "OAC_Mime" },
            { 154, "OAC_Moviemnstr" },
            { 155, "OAC_Mumbaiserv" },
            { 156, "OAC_Mumbsec" },
            { 157, "OAC_Nitiate" },
            { 158, "OAC_Nurse" },
            { 159, "OAC_Orson" },
            { 160, "OAC_Parka" },
            { 161, "OAC_Politicasst" },
            { 162, "OAC_Politician" },
            { 163, "OAC_Priest_VP" },
            { 164, "OAC_Queensgrd" },
            { 165, "OAC_Queenthug" },
            { 166, "OAC_Rangangrd" },
            { 167, "OAC_Rangansec" },
            { 168, "OAC_Realstbroke" },
            { 169, "OAC_Resident" },
            { 170, "OAC_Resortstaff" },
            { 171, "OAC_Sitewkr" },
            { 172, "OAC_Snowtrek" },
            { 173, "OAC_Subwkr" },
            { 174, "OAC_Suit" },
            { 175, "OAC_Tailor" },
            { 176, "OAC_Teaserv" },
            { 177, "OAC_Terminus" },
            { 178, "OAC_Trainserv" },
            { 179, "OAC_Villagegd" },
            { 180, "OAC_Winter" },
            { 181, "OAC_Wiseman" },
            { 182, "OAC_Investbanker" },
            { 183, "OAC_Bankteller" },
            { 184, "OAC_Robber" },
            { 185, "OAC_TechCrew" },
            { 186, "OAC_Attendant" },
            { 187, "OAC_Critic" },
            { 188, "OAC_FamilyGrd" },
            { 189, "OAC_PrivateEye" },
            { 190, "OAC_Undertaker" },
            { 191, "OAC_Photographer" },
            { 192, "OAC_LawyerBd" },
            { 193, "OAC_Biker" },
            { 194, "OAC_DeliveryFox" },
            { 195, "OAC_Dealer" },
            { 196, "OAC_ClubOwner" },
            { 197, "OAC_Bartender" },
            { 198, "OAC_ClubStaff" },
            { 199, "OAC_ClubTech" },
            { 200, "OAC_Herald" },
            { 201, "OAC_Gaucho" },
            { 202, "OAC_WineMkr" },
            { 203, "OAC_Fixer" },
            { 204, "OAC_Sommelier" }
        };

        (*enum_map)["ESoundChannelGroup"] = std::map<uint32_t, std::string>{
            { 0, "ESCG_NONE" },
            { 1, "SND_Default" },
            { 2, "SND_Phys_Rigidbody" },
            { 3, "SND_Phys_Shatter" },
            { 4, "SND_Wpn_HM" },
            { 5, "SND_Wpn_HM_Shot" },
            { 6, "SND_Wpn_NPC" },
            { 7, "SND_Wpn_Impacts" },
            { 8, "SND_Wpn_Flybys" },
            { 9, "SND_Characters" },
            { 10, "SND_Ambience" },
            { 11, "SND_Props" },
            { 12, "SND_Prop_Item" },
            { 13, "SND_FS_HM" },
            { 14, "SND_FS_NPC" },
            { 15, "SND_VisualFX" },
            { 16, "SND_VisualFX_ShotActivate" },
            { 17, "SND_VisualFX_Explosions" },
            { 18, "SND_Diag_HM" },
            { 19, "SND_Diag_NPC" },
            { 20, "SND_CS_Ingame" },
            { 21, "SND_CS_PreRend" },
            { 22, "SND_Music_Env" },
            { 23, "SND_MusicNonEnv" },
            { 24, "SND_SpecialFX_CC" },
            { 25, "SND_SpecialFX_Jumps" },
            { 26, "SND_SpecialFX_Closet" },
            { 27, "SND_SpecialFX_Vent" },
            { 28, "SND_SpecialFX_Instinct" },
            { 29, "SND_SpecialFX_Disguise" },
            { 30, "SND_SpecialFX_Agility" },
            { 31, "SND_Stingers_SFX" },
            { 32, "SND_GUI" },
            { 33, "SND_Test" },
            { 34, "ANIM_HM_Generic" },
            { 35, "ANIM_HM_Loco" },
            { 36, "ANIM_HM_Agility" },
            { 37, "ANIM_HM_Suit_Flap" },
            { 38, "ANIM_HM_Suit_Release" },
            { 39, "ANIM_HM_Suit_Slide" },
            { 40, "ANIM_HM_Suit_LandImpact" },
            { 41, "ANIM_HM_Suit_Grab" },
            { 42, "ANIM_HM_Suit_Roll" },
            { 43, "ANIM_HM_Suit_Swoosh_Body" },
            { 44, "ANIM_HM_Suit_Swoosh_Fast" },
            { 45, "ANIM_HM_CC_Impacts" },
            { 46, "ANIM_HM_CC_Movement" },
            { 47, "ANIM_NPC_Generic" },
            { 48, "ANIM_NPC_Loco" },
            { 49, "ANIM_NPC_CC_Impacts" },
            { 50, "ANIM_NPC_CC_Movement" },
            { 51, "SND_Wpn_HM_Tail" },
            { 52, "SND_Wpn_NPC_Tail" },
            { 53, "SND_Diag_NPC_Crowd" },
            { 54, "SND_Phys_RigidBody_Ragdoll" },
            { 55, "SND_Diag_VO" },
            { 56, "SND_GUI_Menu" },
            { 57, "SND_Props_Doors" },
            { 58, "SND_Wpn_HM_Handguns" },
            { 59, "SND_Wpn_HM_Revolvers" },
            { 60, "SND_Wpn_HM_Rifles" },
            { 61, "SND_Wpn_HM_Shotguns" },
            { 62, "SND_Wpn_HM_Smgs" },
            { 63, "SND_Wpn_HM_Snipers" },
            { 64, "SND_Wpn_HM_Other" },
            { 65, "SND_Wpn_NPC_Handguns" },
            { 66, "SND_Wpn_NPC_Revolvers" },
            { 67, "SND_Wpn_NPC_Rifles" },
            { 68, "SND_Wpn_NPC_Shotguns" },
            { 69, "SND_Wpn_NPC_Smgs" },
            { 70, "SND_Wpn_NPC_Snipers" },
            { 71, "SND_Wpn_NPC_Other" },
            { 72, "SND_Diag_NPC_Oneliner" },
            { 73, "SND_Moments" }
        };

        (*enum_map)["ZActorKeywordCondition.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "ANY_TRUE_IF_NO_REQS" }
        };

        (*enum_map)["EGSExplodingPropType"] = std::map<uint32_t, std::string>{
            { 0, "eGSExplodingProp_Unknown" },
            { 1, "eGSExplodingProp_Barrel" },
            { 2, "eGSExplodingProp_PetrolCan" },
            { 3, "eGSExplodingProp_GasCanister" },
            { 4, "eGSExplodingProp_GasTank" },
            { 5, "eGSExplodingProp_Mine" },
            { 6, "eGSExplodingProp_Pig" },
            { 7, "eGSExplodingProp_Pigeon" },
            { 8, "eGSExplodingProp_FireworksBox" },
            { 9, "eGSExplodingProp_Car" }
        };

        (*enum_map)["ZCrowdActorEntity.ECharacterSoundType"] = std::map<uint32_t, std::string>{
            { 0, "eSoundType_Male" },
            { 1, "eSoundType_Female" },
            { 2, "eSoundType_Female_Heels" }
        };

        (*enum_map)["eItemType"] = std::map<uint32_t, std::string>{
            { 690, "eCC_PoolCue" },
            { 40, "eCC_Shovel" },
            { 650, "eOther_ContractEnvelope" },
            { 0, "eCC_FireExtinguisher_01" },
            { 680, "eCC_GlassShiv" },
            { 50, "eCC_Hammer" },
            { 640, "eCC_Scissor" },
            { 10, "eCC_Knife" },
            { 60, "eCC_Cleaver" },
            { 670, "eCC_BaseballBat" },
            { 20, "eCC_Axe" },
            { 70, "eCC_AntitankMine" },
            { 660, "eItemIrrelevantWeapon" },
            { 30, "eCC_PetrolCan" },
            { 730, "eCC_Guitar" },
            { 80, "eCC_C4Brick" },
            { 720, "eCC_Crutch" },
            { 90, "eCC_RemoteExplosive" },
            { 100, "eDetonator" },
            { 740, "eCC_Iron" },
            { 110, "eCC_Wrench" },
            { 120, "eCC_Dildo" },
            { 903, "eDLC_TEST_Prop_02" },
            { 130, "eCC_Bottle" },
            { 140, "eCC_Saw" },
            { 150, "eCC_Screwdriver" },
            { 160, "eCC_MetalPipe" },
            { 170, "eCC_Gavel" },
            { 901, "eDLC_TEST_FireArm_02" },
            { 180, "eCC_Radio" },
            { 190, "eCC_Bong" },
            { 530, "eSniper_AS50" },
            { 921, "eCC_SledgeHammer" },
            { 200, "eCC_Statue_Bust_A" },
            { 520, "eSMG_TMP" },
            { 210, "eGun_44AutoMag" },
            { 550, "eSniper_SakoTRG" },
            { 220, "eGun_44AutoMagSilenced" },
            { 230, "eGun_Derringer" },
            { 540, "eSniper_Remington700" },
            { 923, "eCC_Brick" },
            { 570, "eItemAmmo" },
            { 240, "eGun_Glock" },
            { 560, "eSniper_SakoTRGSilenced" },
            { 250, "eGun_GlockDirty" },
            { 910, "eHotSauceBarrel" },
            { 260, "eGun_GlockSilenced" },
            { 900, "eDLC_TEST_FireArm_01" },
            { 270, "eGun_HardBaller_01" },
            { 280, "eGun_HardBallerSilenced" },
            { 290, "eGun_Sig226" },
            { 300, "eGun_Sig226Silenced" },
            { 940, "eOther_Camera" },
            { 310, "eGun_Sphinx3000" },
            { 320, "eGun_Taurus24_7" },
            { 330, "eGun_Taurus24_7Silenced" },
            { 340, "eGun_Spotter" },
            { 350, "eOther_MG" },
            { 360, "eOther_RPG" },
            { 370, "eRevolver_PoliceRevolver" },
            { 380, "eRevolver_Snubnosed" },
            { 390, "eRevolver_Taurus" },
            { 922, "eCC_Katana" },
            { 400, "eRifle_HK416" },
            { 410, "eRifle_HK416Scope" },
            { 420, "eRifle_HK416Silenced" },
            { 932, "eSyringe_Sedative" },
            { 430, "eRifle_PumpRifle" },
            { 440, "eRifle_SG552" },
            { 450, "eRifle_SG552Scope" },
            { 460, "eShotgun_M500" },
            { 470, "eShotgun_M500New" },
            { 480, "eSMG_HK_UMP" },
            { 490, "eSMG_HK_UMPFlashlight" },
            { 500, "eSMG_HK_UMPScope" },
            { 510, "eSMG_HK_UMPSilenced" },
            { 580, "eCC_FiberWire" },
            { 590, "eOther_ZippoLighter" },
            { 600, "eOther_Keycard_A" },
            { 610, "eOther_SecurityTape" },
            { 620, "eCC_PowerCord" },
            { 630, "eCC_PoliceBaton" },
            { 700, "eCC_Rake" },
            { 710, "eCC_ToyCar" },
            { 760, "eRifle_Beretta_ARX160" },
            { 800, "eShotgun_Spas12" },
            { 810, "eShotgun_Spas12_Compact" },
            { 830, "eShotgun_Silenced" },
            { 860, "eSMG_AK74u" },
            { 880, "eSMG_STK" },
            { 902, "eDLC_TEST_Prop_01" },
            { 920, "eOther_GenericPickup" },
            { 924, "cCC_Book_A" },
            { 931, "eSyringe_Lethal" },
            { 933, "eSyringe_Emetic" },
            { 934, "eSuitcase" },
            { 1337, "eUnknownItemType" }
        };

        (*enum_map)["EDebugExclusionLayerState"] = std::map<uint32_t, std::string>{
            { 0, "DEBUGELSTATE_UNBLOCKED" },
            { 1, "DEBUGELSTATE_BLOCKED" },
            { 2, "DEBUGELSTATE_UNKNOWN" }
        };

        (*enum_map)["ILightEntity.ELightType"] = std::map<uint32_t, std::string>{
            { 0, "LT_DIRECTIONAL" },
            { 1, "LT_ENVIRONMENT" },
            { 2, "LT_OMNI" },
            { 3, "LT_SPOT" },
            { 4, "LT_SQUARESPOT" },
            { 5, "LT_CAPSULE" },
            { 6, "LT_AREA_QUAD" }
        };

        (*enum_map)["ZPhotoModeMenuDataProvider.EMenuEntryType"] = std::map<uint32_t, std::string>{
            { 1, "Toggle" },
            { 2, "Slider" },
            { 3, "List" }
        };

        (*enum_map)["EOnlinTest"] = std::map<uint32_t, std::string>{
            { 0, "eHELLO" },
            { 1, "eWORLD" }
        };

        (*enum_map)["eContractSessionStartError"] = std::map<uint32_t, std::string>{
            { 0, "ERROR_NONE" },
            { 1, "ERROR_MISSING_CONTENT" },
            { 2, "ERROR_SERVER_UNREACHABLE" },
            { 3, "ERROR_SERVER_ERROR" },
            { 4, "ERROR_CONTRACT_NOT_PLAYABLE_YET" },
            { 5, "ERROR_CONTRACT_NOT_PLAYABLE_ANYMORE" },
            { 6, "ERROR_CANCELED" },
            { 7, "ERROR_ALREADY_STARTING" },
            { 8, "ERROR_UNKNOWN" }
        };

        (*enum_map)["EParticleColorUsage"] = std::map<uint32_t, std::string>{
            { 0, "REPLACE_PARTICLE_COLOR" },
            { 1, "LEAVE_PARTICLE_COLOR" }
        };

        (*enum_map)["EGameModeId"] = std::map<uint32_t, std::string>{
            { -1, "GameMode_Invalid" },
            { 0, "GameMode_Normal" },
            { 1, "GameMode_Sniper" },
            { 2, "GameMode_Versus" },
            { 3, "GameMode_Max" }
        };

        (*enum_map)["ZActBehaviorEntity.EApproachAlignment"] = std::map<uint32_t, std::string>{
            { 0, "AA_STRICT" },
            { 1, "AA_LOOSE" }
        };

        (*enum_map)["ILightEntity.EFrameIndexStrategy"] = std::map<uint32_t, std::string>{
            { 0, "WRAP" },
            { 1, "CLAMP" }
        };

        (*enum_map)["EViewportLock"] = std::map<uint32_t, std::string>{
            { 0, "VPL_None" },
            { 1, "VPL_Rectangular" },
            { 2, "VPL_Rectangular_AvoidMinimapOverlap" }
        };

        (*enum_map)["ZInputListenerDpadEntity.eInputListenerDpad"] = std::map<uint32_t, std::string>{
            { 0, "EILDP_UP" },
            { 1, "EILDP_LEFT" },
            { 2, "EILDP_DOWN" },
            { 3, "EILDP_RIGHT" }
        };

        (*enum_map)["ZEscortSituation2Entity.ETargetState"] = std::map<uint32_t, std::string>{
            { 0, "ETS_Unknown" },
            { 1, "ETS_NoTarget" },
            { 2, "ETS_RunningActBehavior" },
            { 3, "ETS_RunningDummyBehavior" },
            { 4, "ETS_RunningOtherBehavior" },
            { 5, "ETS_Dead" },
            { 6, "ETS_TargetIsHitman" }
        };

        (*enum_map)["ZTrespassingSituation.ESituationState"] = std::map<uint32_t, std::string>{
            { 1, "SS_Main" },
            { 2, "SS_StandDown" },
            { 3, "SS_Escalate" }
        };

        (*enum_map)["EAIFormationMemberSpeed"] = std::map<uint32_t, std::string>{
            { 0, "eFMS_Slow" },
            { 1, "eFMS_Normal" },
            { 2, "eFMS_Fast" }
        };

        (*enum_map)["ERenderGlowTypes"] = std::map<uint32_t, std::string>{
            { 0, "ERENDERGLOWTYPE_NONE" },
            { 1, "ERENDERGLOWTYPE_ENEMIES" },
            { 2, "ERENDERGLOWTYPE_ALLIES" },
            { 3, "ERENDERGLOWTYPE_CIVILIAN" },
            { 4, "ERENDERGLOWTYPE_ITEMS" },
            { 5, "ERENDERGLOWTYPE_STASHED_ITEMS" },
            { 6, "ERENDERGLOWTYPE_SETPIECE" },
            { 7, "ERENDERGLOWTYPE_BACKGROUND" },
            { 8, "ERENDERGLOWTYPE_CONTRACT_TARGET" },
            { 9, "ERENDERGLOWTYPE_CONTRACT_TARGET_NON_CRITICAL" },
            { 10, "ERENDERGLOWTYPE_CONTRACT_TARGET_SPECIAL" },
            { 11, "ERENDERGLOWTYPE_OBJECTIVES" },
            { 12, "ERENDERGLOWTYPE_ENFORCER" },
            { 13, "ERENDERGLOWTYPE_LTMEMORY" },
            { 14, "ERENDERGLOWTYPE_TAGGED" },
            { 15, "ERENDERGLOWTYPE_TAGFOCUS_UNTAGGED" },
            { 16, "ERENDERGLOWTYPE_TAGFOCUS_TAGGED" },
            { 17, "ERENDERGLOWTYPE_BACKGROUNDUNMASKED" },
            { 18, "ERENDERGLOWTYPE_INTERACTION" },
            { 19, "ERENDERGLOWTYPE_INTERACTION_SELECTED" },
            { 20, "ERENDERGLOWTYPE_INTERACTION_DESELECTED" },
            { 21, "ERENDERGLOWTYPE_PLAYER_LVA" },
            { 22, "ERENDERGLOWTYPE_PLAYER_LVA_SEEN" },
            { 23, "ERENDERGLOWTYPE_VS_OPPONENT" },
            { 24, "ERENDERGLOWTYPE_TRAVERSAL" },
            { 25, "ERENDERGLOWTYPE_EMISSIVE_UI" },
            { 26, "ERENDERGLOWTYPE_EMISSIVE_UI_IGNORE_DEPTH" },
            { 27, "ERENDERGLOWTYPE_OPPONENT" },
            { 28, "ERENDERGLOWTYPE_CAMERA" }
        };

        (*enum_map)["EFocusBarState"] = std::map<uint32_t, std::string>{
            { 0, "FOCUS_BAR_NORMAL" },
            { 1, "FOCUS_BAR_INSTINCT" },
            { 2, "FOCUS_BAR_BURNING" }
        };

        (*enum_map)["ESoundCollisionObjectType"] = std::map<uint32_t, std::string>{
            { 0, "StaticRigidBody" },
            { 1, "DynamicRigidBody" },
            { 2, "Ragdoll" }
        };

        (*enum_map)["ZDistanceCondition.ECompareMethod"] = std::map<uint32_t, std::string>{
            { 0, "CM_GreaterOrEqualThan" },
            { 1, "CM_LessThan" }
        };

        (*enum_map)["TeleportActionType"] = std::map<uint32_t, std::string>{
            { 0, "STAY" },
            { 1, "TELEPORT" },
            { 2, "LEAVE_CROWD" }
        };

        (*enum_map)["EActorAIState"] = std::map<uint32_t, std::string>{
            { 0, "eAAIS_None" },
            { 1, "eAAIS_Distracted" },
            { 2, "eAAIS_PotentialThreat" },
            { 3, "eAAIS_PotentialThreatDistracted" },
            { 4, "eAAIS_PotentialThreatDisabled" },
            { 5, "eAAIS_Aggressive" },
            { 6, "eAAIS_EscortingOut" },
            { 7, "eAAIS_Fleeing" },
            { 8, "eAAIS_Unconscious" },
            { 9, "eAAIS_Stunned" },
            { 10, "eAAIS_Grenade" },
            { 11, "eAAIS_DisabledInCombat" },
            { 12, "eAAIS_Disabled" },
            { 13, "eAAIS_Max" }
        };

        (*enum_map)["EMorphemeEventId"] = std::map<uint32_t, std::string>{
            { 0, "eDE_None" },
            { 1700, "eDE_StandToMoveRotationToTranslation" },
            { 18, "eDE_UnholsterWeapon" },
            { 10051, "eDE_Act_FB_BlendIn_Start" },
            { 862, "eDE_RecoveryPowerRagdollEnd" },
            { 19, "eDE_HolsterWeapon" },
            { 100, "eDE_LegR" },
            { 20, "eDE_FireWeapon" },
            { 1801, "eDE_HeadIKDisable" },
            { 4001, "eDE_ActItemPickup" },
            { 101, "eDE_LegL" },
            { 900, "eDE_ImpactFinished" },
            { 21, "eDE_AttachWeapon" },
            { 22, "eDE_DetachWeapon" },
            { 200, "eDE_TransitionEnd_LegR" },
            { 201, "eDE_TransitionEnd_LegL" },
            { 255, "eDE_Finish" },
            { 701, "eDE_EnableNPCHitmanCollision" },
            { 2010, "eDE_HeadControlEnable" },
            { 300, "eDE_InterpolationStart" },
            { 3011, "eDE_CombatEnableLeftHandIK" },
            { 1600, "eDE_OffHandIKEnable" },
            { 301, "eDE_InterpolationEnd" },
            { 400, "eDE_TransitionSignal" },
            { 3010, "eDE_CombatEnableRightHandIK" },
            { 3030, "eDE_CombatEnableShoot" },
            { 500, "eDE_TransitionEnd" },
            { 10050, "eDE_Act_UB_BlendIn_End" },
            { 700, "eDE_DisableNPCHitmanCollision" },
            { 800, "eDE_EnableRagdollRangeStart" },
            { 801, "eDE_EnableRagdollRangeEnd" },
            { 850, "eDE_StartBlendingOut" },
            { 3050, "eDE_CombatEnableBlindShoot" },
            { 851, "eDE_FinishBlendingOut" },
            { 1090, "eDE_ActFullbodyStartMarker" },
            { 860, "eDE_StartOrientationBlend" },
            { 861, "eDE_FinishOrientationBlend" },
            { 3015, "eDE_CombatEndEnableIK" },
            { 863, "eDE_RecoveryBlendEnd" },
            { 1000, "eDE_ActFinished" },
            { 1001, "eDE_ActFinishedWhenMoving" },
            { 2001, "eDE_WindowLeaningDisable" },
            { 1005, "eDE_ActReached" },
            { 1050, "eDE_ActLoop" },
            { 1091, "eDE_ActFullbodyEndMarker" },
            { 1100, "eDE_ReactionFinished" },
            { 1500, "eDE_StandToCrouch" },
            { 1501, "eDE_CrouchToStand" },
            { 1601, "eDE_OffHandIKDisable" },
            { 4002, "eDE_ActItemDrop" },
            { 3070, "eDE_CombatUnholsterGrenade" },
            { 1701, "eDE_MoveToStandTranslationToRotation" },
            { 1800, "eDE_HeadIKEnable" },
            { 4000, "eDE_ActBehaviorEventStart" },
            { 2000, "eDE_WindowLeaningEnable" },
            { 4010, "eDE_AmbientItemUseMarker" },
            { 2002, "eDE_PreventDeathAnimations" },
            { 2003, "eDE_AllowDeathAnimations" },
            { 2011, "eDE_HeadControlDisable" },
            { 3000, "eDE_CombatActEndMarker" },
            { 3025, "eDE_CombatEndDisableIK" },
            { 3100, "eDE_CombatAim" },
            { 10301, "eDE_Act_LeadIn_Marker" },
            { 3020, "eDE_CombatStartDisableIK" },
            { 3040, "eDE_CombatDisableShoot" },
            { 3060, "eDE_CombatDisableBlindShoot" },
            { 3080, "eDE_CombatThrowGrenade" },
            { 4999, "eDE_ActBehaviorEventEnd" },
            { 10052, "eDE_Act_FB_BlendIn_End" },
            { 10053, "eDE_Act_FB_BlendOut_End" }
        };

        (*enum_map)["EAccidentScaleContext"] = std::map<uint32_t, std::string>{
            { 0, "eASC_Standard" },
            { 1, "eASC_Unsafe" },
            { 2, "eASC_Dangerous" }
        };

        (*enum_map)["EEngineModeId"] = std::map<uint32_t, std::string>{
            { -1, "EngineMode_Invalid" },
            { 0, "EngineMode_SinglePlayer" },
            { 1, "EngineMode_SplitScreen" },
            { 2, "EngineMode_Multiplayer" },
            { 3, "EngineMode_Max" }
        };

        (*enum_map)["ZActBehaviorEntity.EState"] = std::map<uint32_t, std::string>{
            { 0, "UNDEFINED" },
            { 1, "IDLE" },
            { 2, "STOPPING" },
            { 3, "PREPARING" },
            { 4, "MOVING" },
            { 5, "ENTERING" },
            { 6, "RUNNING" },
            { 7, "TIMEDOUT" },
            { 8, "COMPLETE" }
        };

        (*enum_map)["ZSoundAmbienceGroup.ESoundCollisionType"] = std::map<uint32_t, std::string>{
            { 0, "SCT_Box" },
            { 1, "SCT_Cylinder" },
            { 2, "SCT_Sphere" },
            { 3, "SCT_Volume" }
        };

        (*enum_map)["EDamageEvent"] = std::map<uint32_t, std::string>{
            { 128, "eDE_Shoot" },
            { 64, "eDE_DeadlyThrow" },
            { 0, "eDE_UNDEFINED" },
            { 16384, "eDE_Electric" },
            { 32768, "eDE_SuspendedObject" },
            { 32, "eDE_KickDownStairs" },
            { 8192, "eDE_Poison" },
            { 16, "eDE_PushOver" },
            { 4096, "eDE_Garotte" },
            { 1, "eDE_InCloset" },
            { 2, "eDE_Subdue" },
            { 4, "eDE_CloseCombat" },
            { 256, "eDE_Sedated" },
            { 512, "eDE_InstantTakeDown" },
            { 1024, "eDE_CoupDeGrace" },
            { 2048, "eDE_ContextKill" },
            { 65536, "eDE_Burn" },
            { 131072, "eDE_Drop" },
            { 262144, "eDE_Drown" }
        };

        (*enum_map)["ZDebugGizmoEntity.EDrawLayer"] = std::map<uint32_t, std::string>{
            { 0, "DL_DEFAULT" },
            { 1, "DL_LIGHT" },
            { 2, "DL_PARTICLES" },
            { 3, "DL_PARTITIONING" },
            { 4, "DL_DECALS" },
            { 5, "DL_CROWD" },
            { 6, "DL_PHYSICS" },
            { 7, "DL_HERO" },
            { 8, "DL_AI" },
            { 9, "DL_AI_GRID" },
            { 10, "DL_AI_SITUATION" },
            { 11, "DL_AI_AREA" },
            { 12, "DL_NPC_LOCOMOTION" },
            { 13, "DL_GAME" },
            { 14, "DL_ALIGNMENT" },
            { 15, "DL_ENGINE" },
            { 16, "DL_SOUND" },
            { 17, "DL_ANIMATION" },
            { 18, "DL_CLOTH" },
            { 19, "DL_SOUND_PARTITIONING" },
            { 20, "DL_UI" }
        };

        (*enum_map)["ZCoverPlane.ECoverSize"] = std::map<uint32_t, std::string>{
            { 0, "eLowCover" },
            { 1, "eMediumCover" },
            { 2, "eHighCover" }
        };

        (*enum_map)["EAnimSetType"] = std::map<uint32_t, std::string>{
            { 0, "EAST_UNDEFINED" },
            { 1, "EAST_DEFAULT" },
            { 2, "EAST_PISTOL" },
            { 3, "EAST_REVOLVER" },
            { 4, "EAST_SMG" },
            { 5, "EAST_SNIPER" },
            { 6, "EAST_ASSULT" },
            { 7, "EAST_SHOTGUN" },
            { 8, "EAST_BODYBAG" },
            { 9, "EAST_GLASS" },
            { 10, "EAST_WHISKYGLASS" },
            { 11, "EAST_WINEGLASS" },
            { 12, "EAST_CHAMPAGNEGLASS" },
            { 13, "EAST_PHONE" },
            { 14, "EAST_BOX" },
            { 15, "EAST_PLATE" },
            { 16, "EAST_SMALLOBJ" },
            { 17, "EAST_SUITCASE" },
            { 18, "EAST_TRAY" },
            { 19, "EAST_TROLLY" },
            { 20, "EAST_FLASHLIGHT" },
            { 21, "EAST_CARRY_1HANDEDWEAPON" },
            { 22, "EAST_CARRY_2HANDEDWEAPON" },
            { 23, "EAST_CARRY_RAKE" },
            { 24, "EAST_CARRY_WEAPON_ITEM" },
            { 25, "EAST_CARRY_BRICK_MINE" },
            { 26, "EAST_CARRY_PROXY_MINE" },
            { 27, "EAST_CARRY_LEAD_PIPES" },
            { 28, "EAST_CARRY_TUBE" },
            { 29, "EAST_CARRY_UMBRELLA" }
        };

        (*enum_map)["ZContextKillGuide.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["EMassImpulseType"] = std::map<uint32_t, std::string>{
            { 0, "eMIT_None" },
            { 1, "eMIT_Explosion" },
            { 2, "eMIT_ShockWave" },
            { 3, "eMIT_ShockWaveNoExplosion" }
        };

        (*enum_map)["ReverbFidelity"] = std::map<uint32_t, std::string>{
            { 0, "Low" },
            { 1, "Medium" },
            { 2, "High" }
        };

        (*enum_map)["EDifficultyLevel"] = std::map<uint32_t, std::string>{
            { 0, "eDL_NO_FLAGS" },
            { 8, "eDL_VERY_HARD" },
            { 1, "eDL_EASY" },
            { 2, "eDL_NORMAL" },
            { 4, "eDL_HARD" },
            { 15, "eDL_ALL_FLAGS" }
        };

        (*enum_map)["ECharacterCollidableLayer"] = std::map<uint32_t, std::string>{
            { 0, "CCL_DYNAMIC" },
            { 1, "CCL_DYNAMIC_TRANSPARENT" },
            { 2, "CCL_DYNAMIC_NO_COLLISION" },
            { 3, "CCL_DYNAMIC_NO_COLLISION_TRANSPARENT" },
            { 4, "CCL_UNUSED_LAST" }
        };

        (*enum_map)["ZHUDOutfitWidgetControllerEntity.EVisibilityPolicy"] = std::map<uint32_t, std::string>{
            { 0, "INITIALLY_HIDDEN_POPUP_ON_CHANGE" },
            { 1, "DONT_TOUCH" }
        };

        (*enum_map)["ESmartWaitCondition"] = std::map<uint32_t, std::string>{
            { 0, "SWC_Time" },
            { 1, "SWC_Distance" },
            { 2, "SWC_Executing" },
            { 3, "SWC_Executed" }
        };

        (*enum_map)["ZActorPickerEffectTrack.EEffectPhaseAction"] = std::map<uint32_t, std::string>{
            { 0, "EPA_ATTACH" },
            { 1, "EPA_DETACH" }
        };

        (*enum_map)["ERegionId"] = std::map<uint32_t, std::string>{
            { 0, "RegionId_Default" },
            { 1, "RegionId_Japan" },
            { 2, "RegionId_Asia" }
        };

        (*enum_map)["ZTestGroupPath.EModifierType"] = std::map<uint32_t, std::string>{
            { 0, "EMT_PushCorners" },
            { 1, "EMT_PushCornersAlt" }
        };

        (*enum_map)["ZCautiousSearchGroup.EAssistantState"] = std::map<uint32_t, std::string>{
            { 0, "AS_CalculateFields" },
            { 1, "AS_RequestNode" },
            { 2, "AS_Approach" },
            { 3, "AS_Approaching" },
            { 4, "AS_Act" },
            { 5, "AS_Acting" }
        };

        (*enum_map)["EAnimationPose"] = std::map<uint32_t, std::string>{
            { 0, "AP_Undefined" },
            { 1, "AP_Standing" },
            { 2, "AP_SittingChair" },
            { 3, "AP_SittingGround" },
            { 4, "AP_LyingGround" },
            { 5, "AP_Crouching" },
            { 6, "AP_SittingBench" },
            { 7, "AP_SittingChairTable" }
        };

        (*enum_map)["EActorLookAtStopReason"] = std::map<uint32_t, std::string>{
            { 0, "eSR_Unknown" },
            { 1, "eSR_Disabled" },
            { 2, "eSR_DurationExpired" },
            { 3, "eSR_OutOfRange" },
            { 4, "eSR_OutOfSight" },
            { 5, "eSR_OutOfRemainingPathDistance" },
            { 6, "eSR_Interrupted" },
            { 7, "eSR_TargetRemoved" },
            { 8, "eSR_HigherPriorityProfile" },
            { 9, "eSR_HideInPlainSight" }
        };

        (*enum_map)["ZHM5Item.EUseTypes"] = std::map<uint32_t, std::string>{
            { 0, "EUT_CantUse" },
            { 1, "EUT_Toggle" },
            { 2, "EUT_TurnOn" }
        };

        (*enum_map)["EHM5SoundFootwearType"] = std::map<uint32_t, std::string>{
            { 0, "EFWT_UNDEFINED" },
            { 1, "EFWT_BAREFOOT" },
            { 2, "EFWT_SLIPPERS" },
            { 3, "EFWT_DISPOSABLE_SLIPPERS" },
            { 4, "EFWT_SANDALS" },
            { 5, "EFWT_SNEAKERS" },
            { 6, "EFWT_SNEAKERS_METALLEG" },
            { 7, "EFWT_LEATHER" },
            { 8, "EFWT_FEMALE_LEATHER" },
            { 9, "EFWT_BOOTS" },
            { 10, "EFWT_COWBOY_BOOTS" },
            { 11, "EFWT_FEMALE_COWBOY_BOOTS" },
            { 12, "EFWT_RUBBER_BOOTS" },
            { 13, "EFWT_HIGH_HEELS" },
            { 14, "EFWT_LEATHER_KEYS" },
            { 15, "EFWT_SWAT_BOOTS" }
        };

        (*enum_map)["ECharacterIdleHandlerFullbodyType"] = std::map<uint32_t, std::string>{
            { 0, "ecIHFBT_DisguisedIdle" },
            { 1, "ecIHFBT_Event" }
        };

        (*enum_map)["ZValueBool_Operation_Signal.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "NOT_ALL" },
            { 4, "ALL_EQUAL" },
            { 5, "ONE_TRUE" }
        };

        (*enum_map)["ZCautiousInvestigateGroup.EInvestigateGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_FindAssistant" },
            { 1, "IGS_PreAcknowledge" },
            { 2, "IGS_Acknowledge" },
            { 3, "IGS_Approach" },
            { 4, "IGS_Approaching" },
            { 5, "IGS_Investigating" },
            { 6, "IGS_DeadBody" },
            { 7, "IGS_Completed" },
            { 8, "IGS_Max" }
        };

        (*enum_map)["IHM5Door.eCloseMode"] = std::map<uint32_t, std::string>{
            { 0, "CM_AUTOCLOSE" },
            { 1, "CM_MANUALCLOSE" },
            { 2, "CM_NEVERCLOSE" }
        };

        (*enum_map)["EParticleSpritePivotLocation"] = std::map<uint32_t, std::string>{
            { 0, "CENTER_PIVOT" },
            { 1, "BOTTOM_PIVOT" },
            { 2, "TOP_PIVOT" }
        };

        (*enum_map)["WebSocketUtils.EOpCode"] = std::map<uint32_t, std::string>{
            { 0, "eCONTINUATION" },
            { 8, "eCLOSE" },
            { 1, "eTEXT" },
            { 9, "ePING" },
            { 2, "eBINARY" },
            { 10, "ePONG" },
            { 15, "eNONE" }
        };

        (*enum_map)["EVRConfigAnimationMode"] = std::map<uint32_t, std::string>{
            { 0, "EVRCAM_FullAnimation" },
            { 1, "EVRCAM_FullAnimationFreeze" },
            { 2, "EVRCAM_HideAnimation" },
            { 3, "EVRCAM_HideUpperBodyAnimation" },
            { 4, "EVRCAM_RemoveGroundMotion" },
            { 5, "EVRCAM_RemoveFullBodyAndGroundMotion" },
            { 6, "EVRCAM_KeepCurrentMode" }
        };

        (*enum_map)["EAnimSetState"] = std::map<uint32_t, std::string>{
            { 0, "EASES_DEFAULT" },
            { 1, "EASES_AMBIENT_ARMED" },
            { 2, "EASES_AMBIENT_VIP_ESCORT" },
            { 3, "EASES_AMBIENT_HERO_ESCORT" },
            { 4, "EASES_COMBAT_EVACUATE" },
            { 5, "EASES_COMBAT_EVACUATE_VIP_SOLO" },
            { 6, "EASES_SICK" },
            { 7, "EASES_INFECTED" }
        };

        (*enum_map)["ZRenderPostfilterParametersEntity.EHDRToneMapType"] = std::map<uint32_t, std::string>{
            { 0, "eLinear" },
            { 1, "eReinhardRGB" },
            { 2, "eFilmicRGB" }
        };

        (*enum_map)["SFootIKEventData.EFoot"] = std::map<uint32_t, std::string>{
            { 0, "eLeft" },
            { 1, "eRight" },
            { 2, "eBoth" }
        };

        (*enum_map)["EHM5GameInputFlag"] = std::map<uint32_t, std::string>{
            { 0, "eGameInputActionPerform" },
            { 1, "eGameInputActionPickup" },
            { 2, "eGameInputActionAbort" },
            { 3, "eGameInputActionDoor" },
            { 4, "eGameInputWeaponZoomIn" },
            { 5, "eGameInputWeaponZoomOut" },
            { 6, "eGameInputActionShoot" },
            { 7, "eGameInputActionPrecisionShoot" },
            { 8, "eGameInputAimOTS" },
            { 9, "eGameInputCoverAction" },
            { 10, "eGameInputCoverAlternativeAction" },
            { 11, "eGameInputInventoryAccept" },
            { 12, "eGameInputEquipShortRange" },
            { 13, "eGameInputEquipLongRange" },
            { 14, "eGameInputHolsterItem" },
            { 15, "eGameInputDropItem" },
            { 16, "eGameInputTogglePlacement" },
            { 17, "eGameInputActivatePropQuick" },
            { 18, "eGameInputActivateProp" },
            { 19, "eGameInputActivatePropHold" },
            { 20, "eGameInputActivatePropSecondary" },
            { 21, "eGameInputActivatePropInterruptibleStart" },
            { 22, "eGameInputActivatePropInterruptibleEnd" },
            { 23, "eGameInputConcealRetrieve" },
            { 24, "eGameInputGuideAction" },
            { 25, "eGameInputGuideActionHold" },
            { 26, "eGameInputGuideActionSecondary" },
            { 27, "eGameInputGuideEnterWindow" },
            { 28, "eGameInputGuideSneakPastWindow" },
            { 29, "eGameInputGuideSneakPastWindowHold" },
            { 30, "eGameInputGuideKill" },
            { 31, "eGameInputSlideLadder" },
            { 32, "eGameInputCloseCombat" },
            { 33, "eGameInputFiberwire" },
            { 34, "eGameInputCloseCombatCounter" },
            { 35, "eGameInputCloseCombatTakeDown" },
            { 36, "eGameInputCloseCombatSnapNeck" },
            { 37, "eGameInputSurrender" },
            { 38, "eGameInputShowItem" },
            { 39, "eGameInputReload" },
            { 40, "eGameInputTakeDisguise" },
            { 41, "eGameInputDragDropBody" },
            { 42, "eGameInputDumpBody" },
            { 43, "eGameInputSneakToggle" },
            { 44, "eGameInputCamSwitch" },
            { 45, "eGameInputActivateRun" },
            { 46, "eGameInputActivateWalkSlow" },
            { 47, "eGameInputInstinctMode" },
            { 48, "eGameInputContractTargetTag" },
            { 49, "eGameInputThrow" },
            { 50, "eGameInputCloseCombatChainHigh" },
            { 51, "eGameInputCloseCombatChainLow" },
            { 52, "eGameInputCloseCombatChainCounter" },
            { 53, "eGameInputSkipCutSequence" },
            { 54, "eGameInputCloseCombatChokeStart" },
            { 55, "eGameInputCloseCombatChoke" },
            { 56, "eGameInputFocusedInteractionFaceUp" },
            { 57, "eGameInputFocusedInteractionFaceLeft" },
            { 58, "eGameInputFocusedInteractionFaceDown" },
            { 59, "eGameInputFocusedInteractionLeftStickAnalog" },
            { 60, "eGameInputRemoteAction" },
            { 61, "eGameInputChangeAmmoNext" },
            { 62, "eGameInputChangeAmmoPrevious" },
            { 63, "eGameInputDefault" },
            { 64, "eGameInputActionsNUM" }
        };

        (*enum_map)["EDramaStateFlag"] = std::map<uint32_t, std::string>{
            { 0, "eDSF_DEFAULT" },
            { 8, "eDSF_RUNNING" },
            { 16, "eDSF_DONE" },
            { 32, "eDSF_TERMINATED" },
            { 1, "eDSF_ENABLED" },
            { 2, "eDSF_CAST" },
            { 4, "eDSF_RESUMING" }
        };

        (*enum_map)["ZHeroEscortSituation2Entity.EEscortState"] = std::map<uint32_t, std::string>{
            { 0, "EES_Idle" },
            { 1, "EES_Evaluate" },
            { 2, "EES_Escorting" },
            { 3, "EES_Intermediate" },
            { 4, "EES_OutOfRange" }
        };

        (*enum_map)["EActorState"] = std::map<uint32_t, std::string>{
            { 0, "AS_DEACTIVATED" },
            { 1, "AS_ALIVE" },
            { 2, "AS_DYING" },
            { 3, "AS_DEAD" },
            { 4, "AS_DISABLED" }
        };

        (*enum_map)["SActorSoundDefs.EDefinition"] = std::map<uint32_t, std::string>{
            { 0, "_NoSound" }
        };

        (*enum_map)["ZKeywordCondition.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "ANY_TRUE_IF_NO_REQS" },
            { 4, "EQUAL" },
            { 5, "GREATER" },
            { 6, "LESS" }
        };

        (*enum_map)["EBehaviorTreeVariableType"] = std::map<uint32_t, std::string>{
            { -1, "BTVT_Invalid" },
            { 0, "BTVT_SceneReference" },
            { 1, "BTVT_Contextual" },
            { 2, "BTVT_Dynamic" },
            { 3, "BTVT_NumTypes" }
        };

        (*enum_map)["EActorPerceptionSensitivity"] = std::map<uint32_t, std::string>{
            { 0, "APS_AMBIENT_DEAFBLIND" },
            { 1, "APS_AMBIENT_SLEEPING" },
            { 2, "APS_AMBIENT_PREOCCUPIED" },
            { 3, "APS_AMBIENT" },
            { 4, "APS_ALERT_LOW" },
            { 5, "APS_ALERT" },
            { 6, "APS_COMBAT" },
            { 7, "APS_SPIDER_SENSE" }
        };

        (*enum_map)["ECharacterStateTransitionAttempt"] = std::map<uint32_t, std::string>{
            { 0, "eSM_TA_IfDestinationCan" },
            { 1, "eSM_TA_IfDestinationCanAndWants" },
            { 2, "eSM_TA_IfDestinationCanAndOriginDoesNotWant" },
            { 3, "eSM_TA_IfDestinationCanAndWantsOriginDoesNotWant" },
            { 4, "eSM_TA_IfDestinationCanAndOriginCanNot" }
        };

        (*enum_map)["ETakeDownState"] = std::map<uint32_t, std::string>{
            { 0, "eTD_Cover" },
            { 1, "eTD_Locomotion" },
            { 2, "eTD_ChokeLoop" },
            { 3, "eTD_ChokeFinish" },
            { 4, "eTD_Deactivating" },
            { 5, "eTD_Unknown" }
        };

        (*enum_map)["ESequencePreviewState"] = std::map<uint32_t, std::string>{
            { 0, "PREVIEW_STOPPED" },
            { 1, "PREVIEW_PLAYING" },
            { 2, "PREVIEW_PAUSED" }
        };

        (*enum_map)["ECameraState"] = std::map<uint32_t, std::string>{
            { 0, "eCamSneakStand" },
            { 1, "eCamSneakWalk" },
            { 2, "eCamSneakRoadyRun" },
            { 3, "eCamNormalStand" },
            { 4, "eCamNormalWalk" },
            { 5, "eCamNormalRun" },
            { 6, "eCamLocomotion" },
            { 7, "eCamCrowdStand" },
            { 8, "eCamCrowdWalk" },
            { 9, "eCamCrowdRun" },
            { 10, "eCamCrowdSneakStand" },
            { 11, "eCamCrowdSneakWalk" },
            { 12, "eCamCrowdSneakRoadyRun" },
            { 13, "eCamCrowdLocomotion" },
            { 14, "eCamDead" },
            { 15, "eCamLadder" },
            { 16, "eCamDrainPipe" },
            { 17, "eCamDrag" },
            { 18, "eCamSBTag" },
            { 19, "eCamSBTagOTS" },
            { 20, "eCamCloseCombat" },
            { 21, "eCamFiberWire" },
            { 22, "eCamWindowPull" },
            { 23, "eCamRailPush" },
            { 24, "eCamLedgeKick" },
            { 25, "eCamLedgePull" },
            { 26, "eCamLedgeHang" },
            { 27, "eCamLedgeWalk" },
            { 28, "eCamLedgeWalkOTS" },
            { 29, "eCamCoverLow" },
            { 30, "eCamCoverMedium" },
            { 31, "eCamCoverHigh" },
            { 32, "eCamCoverLowOTS" },
            { 33, "eCamCoverHighOTS" },
            { 34, "eCamCoverLowScope" },
            { 35, "eCamCoverHighScope" },
            { 36, "eCamCoverLowTakedownOver" },
            { 37, "eCamCoverLowTakedownCorner" },
            { 38, "eCamCoverHighTakedownCorner" },
            { 39, "eCamTakeDisguise" },
            { 40, "eCamOTS" },
            { 41, "eCamOTSHigh" },
            { 42, "eCamOTSLow" },
            { 43, "eCamUnAimedShooting" },
            { 44, "eCamUnAimedShootingHigh" },
            { 45, "eCamUnAimedShootingLow" },
            { 46, "eCamScope" },
            { 47, "eCamScopeHigh" },
            { 48, "eCamScopeLow" },
            { 49, "eCamVaultLow" },
            { 50, "eCamVaultHigh" },
            { 51, "eCamTakedown" },
            { 52, "eCamTakedownChair" },
            { 53, "eCamLockedSniping" },
            { 54, "eCamLockedSnipingScope" },
            { 55, "eCamLockedSnipingScopeHigh" },
            { 56, "eCamLockedSnipingScopeLow" },
            { 57, "eCamContainerAssemble" },
            { 58, "eCamNumProfiles" },
            { 59, "eCamAll" }
        };

        (*enum_map)["IContractObjective.ObjectiveType"] = std::map<uint32_t, std::string>{
            { 0, "KILL" },
            { 1, "SETPIECE" },
            { 2, "CUSTOMKILL" },
            { 3, "CUSTOM" }
        };

        (*enum_map)["ZAISoundEvent.ELoudness"] = std::map<uint32_t, std::string>{
            { 0, "EAISEL_Default" },
            { 1, "EAISEL_VeryLow" },
            { 2, "EAISEL_Low" },
            { 3, "EAISEL_Normal" },
            { 4, "EAISEL_Loud" },
            { 5, "EAISEL_VeryLoud" }
        };

        (*enum_map)["ZStandOffSituation.ESituationState"] = std::map<uint32_t, std::string>{
            { 0, "SS_Uncertain" },
            { 1, "SS_Main" },
            { 2, "SS_Arresting" },
            { 3, "SS_GetHelp" },
            { 4, "SS_Escalate" },
            { 5, "SS_Investigate" },
            { 6, "SS_GetHelpFromFleeingCivilian" }
        };

        (*enum_map)["ECppTypeFlags"] = std::map<uint32_t, std::string>{
            { 1, "EDITOR_ONLY" },
            { 2, "EDITOR_ONLY_CRITICAL" },
            { 4, "RESETABLE" },
            { 8, "RESETABLE_COND" },
            { 16, "RESETABLE_STATIC" }
        };

        (*enum_map)["ESynchronisedActionState"] = std::map<uint32_t, std::string>{
            { 0, "eSAS_INACTIVE" },
            { 1, "eSAS_WAITING_TO_EXECUTE" },
            { 2, "eSAS_COOLINGDOWN" }
        };

        (*enum_map)["EInventoryConfigItemHandling"] = std::map<uint32_t, std::string>{
            { 0, "EICIH_Allow" },
            { 1, "EICIH_Store" },
            { 2, "EICIH_InstaStore" },
            { 3, "EICIH_Drop" },
            { 4, "EICIH_DropOrStore" },
            { 5, "EICIH_DropAndPickup" },
            { 6, "EICIH_DoNothing" }
        };

        (*enum_map)["ZUISubtitleDataProvider.EAlignment"] = std::map<uint32_t, std::string>{
            { 0, "ALIGN_BOTTOM" },
            { 1, "ALIGN_TOP" }
        };

        (*enum_map)["ECollidableLayer"] = std::map<uint32_t, std::string>{
            { 0, "eCollLayer_COLLIDE_WITH_ALL" },
            { 1, "eCollLayer_STATIC_COLLIDABLES_ONLY" },
            { 2, "eCollLayer_DYNAMIC_COLLIDABLES_ONLY" },
            { 3, "eCollLayer_STAIRS" },
            { 4, "eCollLayer_SHOT_ONLY_COLLISION" },
            { 5, "eCollLayer_DYNAMIC_TRASH_COLLIDABLES" },
            { 6, "eCollLayer_KINEMATIC_COLLIDABLES_ONLY" },
            { 7, "eCollLayer_STATIC_COLLIDABLES_ONLY_TRANSPARENT" },
            { 8, "eCollLayer_DYNAMIC_COLLIDABLES_ONLY_TRANSPARENT" },
            { 9, "eCollLayer_KINEMATIC_COLLIDABLES_ONLY_TRANSPARENT" },
            { 10, "eCollLayer_STAIRS_STEPS" },
            { 11, "eCollLayer_STAIRS_SLOPE" },
            { 12, "eCollLayer_HERO_PROXY" },
            { 13, "eCollLayer_ACTOR_PROXY" },
            { 14, "eCollLayer_HERO_VR" },
            { 15, "eCollLayer_CLIP" },
            { 16, "eCollLayer_ACTOR_RAGDOLL" },
            { 17, "eCollLayer_CROWD_RAGDOLL" },
            { 18, "eCollLayer_LEDGE_ANCHOR" },
            { 19, "eCollLayer_ACTOR_DYN_BODY" },
            { 20, "eCollLayer_HERO_DYN_BODY" },
            { 21, "eCollLayer_ITEMS" },
            { 22, "eCollLayer_WEAPONS" },
            { 23, "eCollLayer_COLLISION_VOLUME_HITMAN_ON" },
            { 24, "eCollLayer_COLLISION_VOLUME_HITMAN_OFF" },
            { 25, "eCollLayer_DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER" },
            { 26, "eCollLayer_DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER_TRANSPARENT" },
            { 27, "eCollLayer_COLLIDE_WITH_STATIC_ONLY" },
            { 28, "eCollLayer_AI_VISION_BLOCKER" },
            { 29, "eCollLayer_AI_VISION_BLOCKER_AMBIENT_ONLY" },
            { 30, "eCollayer_UNUSED_LAST" }
        };

        (*enum_map)["EGSKillVictim"] = std::map<uint32_t, std::string>{
            { 0, "GSKILLVICTIM_UNKNOWN" },
            { 1, "GSKILLVICTIM_CIVILIAN" },
            { 2, "GSKILLVICTIM_GUARD" },
            { 3, "GSKILLVICTIM_TARGET" }
        };

        (*enum_map)["IContractEvaluationContextListener.EListenerType"] = std::map<uint32_t, std::string>{
            { 0, "eDEFAULT" },
            { 1, "eOBJECTIVECOUNTER" },
            { 2, "eFORCEUPDATE" },
            { 3, "eCUSTOM" },
            { 4, "eINTERNAL" },
            { 5, "eCHALLENGECOUNTER" },
            { 6, "eCHALLENGETREE" },
            { 7, "eTOGGLE" },
            { 8, "eMATCHARRAYS" }
        };

        (*enum_map)["IRenderCompositorEntity.EMode"] = std::map<uint32_t, std::string>{
            { 0, "eSourceA" },
            { 1, "eSourceB" },
            { 2, "eLayered" },
            { 3, "eAlphaLayered" }
        };

        (*enum_map)["ZActorKnowledgeCondition.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "ANY" }
        };

        (*enum_map)["IHM5Door.EOpenMode"] = std::map<uint32_t, std::string>{
            { 0, "OM_TWO_WAY" },
            { 1, "OM_OPEN_POS_SIDE_ONLY" },
            { 2, "OM_OPEN_NEG_SIDE_ONLY" },
            { 3, "OM_DISABLED" }
        };

        (*enum_map)["EHeroGameState"] = std::map<uint32_t, std::string>{
            { 0, "eHGS_Reserved_QueryALL" },
            { 1, "eHGS_Reserved_QueryAny" },
            { 2, "eHGS_InstinctModeActive" },
            { 3, "eHGS_IsHardToSee" },
            { 4, "eHGS_Sneaking" },
            { 5, "eHGS_InSequence" },
            { 6, "eHGS_InCloseCombat" },
            { 7, "eHGS_ItemHandlingActive" },
            { 8, "eHGS_CamSwitched" },
            { 9, "eHGS_SniperMode" },
            { 10, "eHGS_OTSMode" },
            { 11, "eHGS_DualWielding" },
            { 12, "eHGS_PreciseAim" },
            { 13, "eHGS_ActivatingAltFire" },
            { 14, "eHGS_FiredLastFrFocusBurnRate" },
            { 15, "eHGS_FiredLastFrFocusGainValue" },
            { 16, "eHGS_BurningFocus" },
            { 17, "eHGS_InsideCloset" },
            { 18, "eHGS_Peeking" },
            { 19, "eHGS_FocusedInteraction" },
            { 20, "eHGS_HoldsUtility" },
            { 21, "eHGS_FacingRight" },
            { 22, "eHGS_ThrowAiming" },
            { 23, "eHGS_PlaceItemAiming" },
            { 24, "eHGS_Reloading" },
            { 25, "eHGS_FiberWirePrimedAndReady" },
            { 26, "eHGS_OperatingDoorVRHandPush" },
            { 27, "eHGS_MvAgility" },
            { 28, "eHGS_MvCover" },
            { 29, "eHGS_MvDragging" },
            { 30, "eHGS_MvDrainPipe" },
            { 31, "eHGS_MvLedgeWalk" },
            { 32, "eHGS_MvLedgeHang" },
            { 33, "eHGS_MvLocomotion" },
            { 34, "eHGS_MvCloseCombat" },
            { 35, "eHGS_MvSurrendering" },
            { 36, "eHGS_MvFriskComplying" },
            { 37, "eHGS_MvShowItem" },
            { 38, "eHGS_MvThrow" },
            { 39, "eHGS_MvFiberWireKill" },
            { 40, "eHGS_MvAlign" },
            { 41, "eHGS_MvTakedown" },
            { 42, "eHGS_MvSubAction" },
            { 43, "eHGS_MvNewFullBody" },
            { 44, "eHGS_MvDisguiseSafeZone" },
            { 45, "eHGS_MvPlaceItem" },
            { 46, "eHGS_NUM" }
        };

        (*enum_map)["EMarkerInclusionMode"] = std::map<uint32_t, std::string>{
            { 0, "MIM_DEFAULT" },
            { 1, "MIM_IF_FORCE_INCLUDED_ONLY" }
        };

        (*enum_map)["ZHM5ItemInteractionEventConsumer.EHM5SoundItemInteractionEvent"] = std::map<uint32_t, std::string>{
            { 0, "EIIE_ItemImpactBack" },
            { 1, "EIIE_ItemImpactFront" },
            { 2, "EIIE_ImpactLow" },
            { 3, "EIIE_ItemPullOut" },
            { 4, "EIIE_ItemOnDeadlyThrow" },
            { 5, "EIIE_ItemSlitThroat" },
            { 6, "EIIE_ItemSoundGeneric01" },
            { 7, "EIIE_ItemSoundGeneric02" },
            { 8, "EIIE_ItemSoundGeneric03" }
        };

        (*enum_map)["ETargetType"] = std::map<uint32_t, std::string>{
            { 0, "eTDS_None" },
            { 1, "eTDS_LookAtTarget" },
            { 2, "eTDS_Spatial" },
            { 3, "eTDS_Linked" }
        };

        (*enum_map)["ETessellationFactorMode"] = std::map<uint32_t, std::string>{
            { 0, "TESSELLATIONFACTORMODE_FIXED" },
            { 1, "TESSELLATIONFACTORMODE_OBJECT" },
            { 2, "TESSELLATIONFACTORMODE_TRIANGLE" }
        };

        (*enum_map)["ZHM5GenericEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "eGeneric00" },
            { 1, "eGeneric01" },
            { 2, "eGeneric02" },
            { 3, "eGeneric03" },
            { 4, "eGeneric04" },
            { 5, "eGeneric05" },
            { 6, "eGeneric06" },
            { 7, "eGeneric07" }
        };

        (*enum_map)["EActorBumpType"] = std::map<uint32_t, std::string>{
            { 0, "EABT_NONE" },
            { 1, "EABT_UPPERBODY" },
            { 2, "EABT_HEAD" }
        };

        (*enum_map)["ETimerEntityCommandType"] = std::map<uint32_t, std::string>{
            { 0, "ETECT_Add" },
            { 1, "ETECT_Remove" },
            { 2, "ETECT_Reset" }
        };

        (*enum_map)["ZValueInt_Comparator_Poll.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "EQUAL" },
            { 1, "NOT_EQUAL" },
            { 2, "LESS" },
            { 3, "LESS_OR_EQUAL" },
            { 4, "HIGHER" },
            { 5, "HIGHER_OR_EQUAL" }
        };

        (*enum_map)["EHM5SoundFootstepStance"] = std::map<uint32_t, std::string>{
            { 0, "EFSS_SNEAK" },
            { 1, "EFSS_SNEAKRUN" },
            { 2, "EFSS_WALKSLOWLY" },
            { 3, "EFSS_WALK" },
            { 4, "EFSS_RUN" },
            { 5, "EFSS_ANY" }
        };

        (*enum_map)["EGestureCategory"] = std::map<uint32_t, std::string>{
            { 0, "EGC_None" },
            { 1, "EGC_AmbientDialog" },
            { 2, "EGC_TrespassWarn1" },
            { 3, "EGC_TrespassWarn2" },
            { 4, "EGC_TrespassWarn3" },
            { 5, "EGC_StopWarn" }
        };

        (*enum_map)["eWeaponOperation"] = std::map<uint32_t, std::string>{
            { 0, "WO_SEMI_AUTO" },
            { 1, "WO_FULL_AUTO" }
        };

        (*enum_map)["ERayDetailLevel"] = std::map<uint32_t, std::string>{
            { 0, "RAYDETAILS_NONE" },
            { 1, "RAYDETAILS_BONES" },
            { 2, "RAYDETAILS_MESH" }
        };

        (*enum_map)["EVictimMovementType"] = std::map<uint32_t, std::string>{
            { 4, "eVictimMovementRecoveryFinisher" },
            { -1, "eVictimMovementNone" },
            { 0, "eVictimMovementPullVictimFromWindow" },
            { 1, "eVictimMovementThrowBodyOverRail" },
            { 2, "eVictimMovementDumpBodyOverLedge" },
            { 3, "eVictimMovementTakeDown" },
            { 5, "eVictimMovementGrabVictim" },
            { 6, "eVictimMovementPushVictimThroughWindowAndRail" },
            { 7, "eVictimMovementContextKill" },
            { 8, "eVictimMovementKickVictimOverLedge" },
            { 9, "eVictimMovementCoupDeGrace" },
            { 10, "eVictimMovementCloseCombat" },
            { 11, "eVictimMovementLast" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioImpactType"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioImpactType_Normal" },
            { 1, "eWBC_AudioImpactType_Sweetener" }
        };

        (*enum_map)["ECameraShakerChannel"] = std::map<uint32_t, std::string>{
            { 0, "eCameraShakerChannel_Default" },
            { 1, "eCameraShakerChannle_Locomotion" },
            { 2, "eCameraShakerChannel_Ambient" },
            { 3, "eCameraShakerChannel_Weapon" },
            { 4, "eCameraShakerChannel_CloseCombat" },
            { 5, "eCameraShakerChannel_Num" }
        };

        (*enum_map)["CrowdUtil.ECrowdActorMood"] = std::map<uint32_t, std::string>{
            { 0, "CM_AMBIENT" },
            { 1, "CM_ALERTED" },
            { 2, "CM_SCARED" },
            { 3, "CM_PANICED" },
            { 4, "CM_DEAD" },
            { 5, "CM_DISABLED" },
            { 6, "NUM_ACTOR_MOOD_TYPES" }
        };

        (*enum_map)["EItemLocation"] = std::map<uint32_t, std::string>{
            { 0, "eIL_Anywhere" },
            { 1, "eIL_Inventory" },
            { 2, "eIL_RightHand" },
            { 3, "eIL_LeftHand" },
            { 4, "eIL_FreeBone" },
            { 5, "eIL_Count" }
        };

        (*enum_map)["ESituationType"] = std::map<uint32_t, std::string>{
            { 0, "AIST_Any" },
            { 1, "AIST_DetectedInPrivate" },
            { 2, "AIST_HandleDistraction" },
            { 3, "AIST_Spectator" },
            { 4, "AIST_InvestigateWeapon" },
            { 5, "AIST_GuardBody" },
            { 6, "AIST_Entourage" },
            { 7, "AIST_HandleTrespassing" },
            { 8, "AIST_DeadBody" },
            { 9, "AIST_InvestigateCautious" },
            { 10, "AIST_Sentry" },
            { 11, "AIST_HandleDisguise" },
            { 12, "AIST_SC_Curious" },
            { 13, "AIST_SC_Alerted" },
            { 14, "AIST_SC_Combat" },
            { 15, "AIST_SniperCombat" },
            { 16, "AIST_Arrest" },
            { 17, "AIST_StandOff" },
            { 18, "AIST_RecoverUnconscious" },
            { 19, "AIST_AvoidExplosion" },
            { 20, "AIST_Evacuate" },
            { 21, "AIST_ProtoCombat" },
            { 22, "AIST_DangerousArea" },
            { 23, "AIST_CloseCombat" },
            { 24, "AIST_Infected" },
            { 25, "ESituationType_Count" }
        };

        (*enum_map)["ZContextKillGuide.EContextKillType"] = std::map<uint32_t, std::string>{
            { 0, "eCKT_TOILET_DROWN" },
            { 1, "eCKT_SNAP_AND_TOSS" },
            { 2, "eCKT_PUSH" },
            { 3, "eCKT_NO_ANIMATION" },
            { 4, "eCKT_END" }
        };

        (*enum_map)["MenuWeaponUpgradeData.EUpgradeAction"] = std::map<uint32_t, std::string>{
            { 0, "EUPGRADE_ACTION_NONE" },
            { 1, "EUPGRADE_ACTION_INSTALL" },
            { 2, "EUPGRADE_ACTION_REMOVE" },
            { 3, "EUPGRADE_ACTION_UNLOCK_SLOT" },
            { 4, "EUPGRADE_ACTION_BUY_NEXT_LEVEL" },
            { 5, "EUPGRADE_ACTION_EQUIP_WEAPON" },
            { 6, "EUPGRADE_ACTION_UNEQUIP_WEAPON" },
            { 7, "EUPGRADE_ACTION_CLEAR_LEVELS" }
        };

        (*enum_map)["ZUIPerformanceTestMetricsCollectorEntity.EVsyncPolicy"] = std::map<uint32_t, std::string>{
            { -1, "None" },
            { 0, "ForceOff" },
            { 1, "Force60" },
            { 2, "Force30" }
        };

        (*enum_map)["EBoolStateRequirement"] = std::map<uint32_t, std::string>{
            { 0, "eBSR_Any" },
            { 1, "eBSR_RequireTrue" },
            { 2, "eBSR_RequireFalse" }
        };

        (*enum_map)["EIntelType"] = std::map<uint32_t, std::string>{
            { 0, "eIT_DATA" },
            { 1, "eIT_OPPORTUNITY" },
            { 2, "eIT_BACKGROUND" },
            { 3, "eIT_HANDLER" },
            { 4, "eIT_UNDEFINED" }
        };

        (*enum_map)["EMultiplayerLobbyRequestType"] = std::map<uint32_t, std::string>{
            { 0, "LOBBY_REQUEST_VOID" },
            { 1, "LOBBY_REQUEST_IDLE" },
            { 2, "LOBBY_REQUEST_CREATE" },
            { 3, "LOBBY_REQUEST_CREATE_LOCALHOST" },
            { 4, "LOBBY_REQUEST_JOIN" }
        };

        (*enum_map)["ERayLayer"] = std::map<uint32_t, std::string>{
            { 8, "eRayLayer_BLOODSPLATDECAL_COLI" },
            { 0, "eRayLayer_COLLIDE_WITH_ALL" },
            { 1, "eRayLayer_CAMERA_COLI" },
            { 2, "eRayLayer_CAMERA_COLI_CAMERA_OBSTACE_ONLY" },
            { 3, "eRayLayer_HERO_FIRE" },
            { 4, "eRayLayer_ACTOR_FIRE" },
            { 5, "eRayLayer_ACTOR_LINE_OF_FIRE" },
            { 7, "eRayLayer_FOOTSTEPS_COLI" },
            { 9, "eRayLayer_COLLIDE_WITH_STATIC" },
            { 10, "eRayLayer_COLLIDE_WITH_STATIC_EXCL_TRANSPARENT" },
            { 11, "eRayLayer_PARTICLES" },
            { 12, "eRayLayer_COVERSEARCH" },
            { 13, "eRayLayer_ACTOR_VISIBILITY" },
            { 14, "eRayLayer_ACTOR_VISIBILITY_AMBIENT" },
            { 16, "eRayLayer_PICKUP" },
            { 17, "eRayLayer_HERO_PROXY" },
            { 19, "eRayLayer_COLLIDE_WITH_STATIC_AND_SLOPE" },
            { 20, "eRayLayer_COLLIDE_WITH_STATIC_AND_SLOPE_EXCL_TRANSPARENT" },
            { 22, "eRayLayer_HERO_PROXY_NO_VOLUMES" },
            { 23, "eRayLayer_PHYSICS_MANIPULATOR" },
            { 24, "eRayLayer_UNUSED_LAST" }
        };

        (*enum_map)["EExclusionLayer"] = std::map<uint32_t, std::string>{
            { 0, "EL_NONE" },
            { 1, "EL_STORY" },
            { 2, "EL_AMBIENT" }
        };

        (*enum_map)["ZHeroEscortSituationEntity.EEscortState"] = std::map<uint32_t, std::string>{
            { 0, "EES_Idle" },
            { 1, "EES_Evaluate" },
            { 2, "EES_Escorting" },
            { 3, "EES_Intermediate" },
            { 4, "EES_OutOfRange" }
        };

        (*enum_map)["EConversationRole"] = std::map<uint32_t, std::string>{
            { 0, "eCR_Leader" },
            { 1, "eCR_Assistant" }
        };

        (*enum_map)["AudioEventCullingBehavior"] = std::map<uint32_t, std::string>{
            { 0, "AutoSendStop" },
            { 1, "AutoSendStopAfterTriggered" },
            { 2, "NoAutoSendStop" },
            { 3, "NoDistanceCulling" }
        };

        (*enum_map)["Network.PacketPriority"] = std::map<uint32_t, std::string>{
            { 0, "IMMEDIATE_PRIORITY" },
            { 1, "HIGH_PRIORITY" },
            { 2, "MEDIUM_PRIORITY" },
            { 3, "LOW_PRIORITY" },
            { 4, "NUMBER_OF_PRIORITIES" }
        };

        (*enum_map)["EReplicaMode"] = std::map<uint32_t, std::string>{
            { 0, "E_RM_STATIC_OBJECT" },
            { 1, "E_RM_DYNAMIC" }
        };

        (*enum_map)["ZValueInt_Comparator.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "EQUAL" },
            { 1, "NOT_EQUAL" },
            { 2, "LESS" },
            { 3, "LESS_OR_EQUAL" },
            { 4, "HIGHER" },
            { 5, "HIGHER_OR_EQUAL" }
        };

        (*enum_map)["ZSpatialEntity.ERoomBehaviour"] = std::map<uint32_t, std::string>{
            { 0, "ROOM_STATIC" },
            { 1, "ROOM_DYNAMIC" },
            { 2, "ROOM_STATIC_OUTSIDE_CLIENT" }
        };

        (*enum_map)["ZOnlineManager.EError"] = std::map<uint32_t, std::string>{
            { 1001, "eNETWORK_UNKNOWN" },
            { 0, "eNONE" },
            { 1000, "eGENERAL" },
            { 1002, "eNETWORK_SEND_FAILED" },
            { 1003, "eAPI_VERSION" },
            { 1004, "eAUTHENTICATION_UNAUTHORIZED" },
            { 1005, "eAUTHENTICATION_PROXY" },
            { 1024, "eGENERAL_SERVER_ERROR" },
            { 1006, "eAUTHENTICATION_SERVER_ERROR" },
            { 1007, "eAUTHENTICATION_USER_GONE" },
            { 1030, "eCANCELLED" },
            { 1008, "eAUTHENTICATION_STEAM_GENERAL" },
            { 1009, "eAUTHENTICATION_EPIC_GENERAL" },
            { 1010, "eAUTHENTICATION_IZUMO_GENERAL" },
            { 1027, "eDYNAMIC_RESOURCES_CONFIG_ERROR" },
            { 1011, "eAUTHENTICATION_PSN_GENERAL" },
            { 1012, "eAUTHENTICATION_PSN_NP_CHECK" },
            { 1013, "eAUTHENTICATION_PSN_ACCOUNT_COLLISION" },
            { 1014, "eAUTHENTICATION_NO_PRIMARY_USER" },
            { 1015, "eAUTHENTICATION_XBLIVE_GENERAL" },
            { 1016, "eAUTHENTICATION_XBLIVE_SIGNED_OUT" },
            { 1017, "eAUTHENTICATION_XBLIVE_USER_CHANGED" },
            { 1028, "eDYNAMIC_RESOURCES_UPDATEFAILED" },
            { 1018, "eAUTHENTICATION_STADIA_GENERAL" },
            { 1019, "ePLATFORM_USERINFO" },
            { 1020, "eCONFIGURATION" },
            { 1021, "eENTITLEMENTS" },
            { 1022, "ePROFILE" },
            { 1023, "eGAME_SESSION" },
            { 1025, "eGENERAL_SERVER_BUSY" },
            { 1026, "eGENERAL_CLIENT_ERROR" },
            { 1029, "eDYNAMIC_RESOURCES_OUTDATED" },
            { 1031, "eERROR_MAX" }
        };

        (*enum_map)["EDetachUsage"] = std::map<uint32_t, std::string>{
            { 0, "EDU_NEVER" },
            { 1, "EDU_ALWAYS" },
            { 2, "EDU_RAGDOLL_ONLY" }
        };

        (*enum_map)["ZActorPicker.EKeywordEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["EDestructibleInteractionType"] = std::map<uint32_t, std::string>{
            { 0, "eDIT_SHOT" },
            { 1, "eDIT_EXPLOSION" },
            { 2, "eDIT_COLLISION" },
            { 3, "eDIT_FRACTURE_AND_DETACH_ALL" },
            { 4, "eDIT_OUT_OF_WORLD" },
            { 5, "eDIT_COUNT" }
        };

        (*enum_map)["EGameEventType"] = std::map<uint32_t, std::string>{
            { 0, "GET_GameplayStart" },
            { 1, "GET_GameplayStop" },
            { 2, "GET_IntroCutStart" },
            { 3, "GET_IntroCutEnd" },
            { 4, "GET_ProfilingStart" },
            { 5, "GET_SavegameRestored" },
            { 6, "GET_PlayingAfterLoad" },
            { 7, "GET_COUNT" }
        };

        (*enum_map)["EPhysicsObjectType"] = std::map<uint32_t, std::string>{
            { 0, "EPHYSICSOBJECTTYPE_UNKNOWN" },
            { 1, "EPHYSICSOBJECTTYPE_DYNAMIC" },
            { 2, "EPHYSICSOBJECTTYPE_KINEMATIC" },
            { 3, "EPHYSICSOBJECTTYPE_STATIC" }
        };

        (*enum_map)["eParticleEmitterMeshEntity_SpawnModes"] = std::map<uint32_t, std::string>{
            { 0, "MESH_SPAWNMODE_VERTEX" },
            { 1, "MESH_SPAWNMODE_EDGE" },
            { 2, "MESH_SPAWNMODE_FACE" }
        };

        (*enum_map)["eWeaponType"] = std::map<uint32_t, std::string>{
            { 0, "WT_HANDGUN" },
            { 1, "WT_SLOWGUN" },
            { 2, "WT_ASSAULTRIFLE" },
            { 3, "WT_SMG" },
            { 4, "WT_SNIPER" },
            { 5, "WT_RPG" },
            { 6, "WT_KNIFE" },
            { 7, "WT_SHOTGUN" },
            { 8, "WT_SPOTTER" }
        };

        (*enum_map)["ERatingTitle"] = std::map<uint32_t, std::string>{
            { 0, "ERatingTitle_SilentAssassin" },
            { 1, "ERatingTitle_PerfectAssassin" },
            { 2, "ERatingTitle_Ninja" },
            { 3, "ERatingTitle_GloryBlazer" },
            { 4, "ERatingTitle_Eraser" },
            { 5, "ERatingTitle_Exhibitionist" },
            { 6, "ERatingTitle_Trickster" },
            { 7, "ERatingTitle_Chameleon" },
            { 8, "ERatingTitle_Spider" },
            { 9, "ERatingTitle_Angel" },
            { 10, "ERatingTitle_COUNT" }
        };

        (*enum_map)["ECameraControls"] = std::map<uint32_t, std::string>{
            { 0, "eCameraControlsNormal" },
            { 1, "eCameraControlsOTS" }
        };

        (*enum_map)["ECollidableShape"] = std::map<uint32_t, std::string>{
            { 0, "ECOLLIDABLESHAPE_SPHERE" },
            { 1, "ECOLLIDABLESHAPE_CAPSULE" },
            { 2, "ECOLLIDABLESHAPE_BOX" }
        };

        (*enum_map)["EHM5SoundRicochetType"] = std::map<uint32_t, std::string>{
            { 0, "ESRT_VARIATION1" },
            { 1, "ESRT_VARIATION2" }
        };

        (*enum_map)["ZUISplashHintDataProviderEntity.ESplashHintType"] = std::map<uint32_t, std::string>{
            { 0, "SHT_GlobalHint" },
            { 1, "SHT_TutorialHint" },
            { 2, "SHT_ControlHint" }
        };

        (*enum_map)["ZHM5GameTimeMultiplierEntity.eTMLDReason"] = std::map<uint32_t, std::string>{
            { 0, "eTMLDR_HintMessages" },
            { 1, "eTMLDR_Sequence" },
            { 2, "eTMLDR_ActionKillHeadShot" },
            { 3, "eTMLDR_ActionKillCCProp" },
            { 4, "eTMLDR_ActionKillExplosion" },
            { 5, "eTMLDR_ActionKillLastEnemyInEncounter" },
            { 6, "eTMLDR_ActionKillLastEnemyInCheckPoint" },
            { 7, "eTMLDR_NOTSET" }
        };

        (*enum_map)["ZInteractionEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "eActivate" },
            { 1, "eRH_Place" },
            { 2, "eRH_Retrieve" },
            { 3, "eRH_Swap" },
            { 4, "eRH_Show" },
            { 5, "eRH_Hide" },
            { 6, "eLH_Place" },
            { 7, "eLH_Retrieve" },
            { 8, "eLH_Swap" },
            { 9, "eLH_Show" },
            { 10, "eLH_Hide" },
            { 11, "eObjectEvent01" },
            { 12, "eObjectEvent02" },
            { 13, "eObjectEvent03" },
            { 14, "eObjectEvent04" },
            { 15, "eSoundEvent01" },
            { 16, "eSoundEvent02" },
            { 17, "eSoundEvent03" },
            { 18, "eSoundEvent04" }
        };

        (*enum_map)["ZHM5IndicatorManager.EIndicatorType"] = std::map<uint32_t, std::string>{
            { 0, "eActorAmbient" },
            { 1, "eActorAlertedLow" },
            { 2, "eActorAlertedHigh" },
            { 3, "eActorArrest" },
            { 4, "eActorCombat" },
            { 5, "eExplosion" },
            { 6, "eObjective" },
            { 7, "ePointOfInterest" },
            { 8, "eContractExit" },
            { 9, "eEasterActor" }
        };

        (*enum_map)["BoneId.Enum"] = std::map<uint32_t, std::string>{
            { 0, "GROUND" },
            { 1, "PELVIS" },
            { 2, "SPINE" },
            { 3, "SPINE_1" },
            { 4, "SPINE_2" },
            { 5, "NECK" },
            { 6, "NECK1" },
            { 7, "HEAD" },
            { 8, "LEFT_THIGH" },
            { 9, "LEFT_THIGH_TWIST" },
            { 10, "LEFT_THIGH_TWIST1" },
            { 11, "LEFT_THIGH_TWIST2" },
            { 12, "LEFT_CALF" },
            { 13, "LEFT_FOOT" },
            { 14, "LEFT_TOE" },
            { 15, "LEFT_TOENUB" },
            { 16, "RIGHT_THIGH" },
            { 17, "RIGHT_THIGH_TWIST" },
            { 18, "RIGHT_THIGH_TWIST1" },
            { 19, "RIGHT_THIGH_TWIST2" },
            { 20, "RIGHT_CALF" },
            { 21, "RIGHT_FOOT" },
            { 22, "RIGHT_TOE" },
            { 23, "RIGHT_TOENUB" },
            { 24, "LEFT_CLAVICLE" },
            { 25, "LEFT_UPPER_ARM" },
            { 26, "LEFT_UPPER_ARM_TWIST" },
            { 27, "LEFT_UPPER_ARM_TWIST1" },
            { 28, "LEFT_UPPER_ARM_TWIST2" },
            { 29, "LEFT_FOREARM" },
            { 30, "LEFT_FORETWIST" },
            { 31, "LEFT_FORETWIST1" },
            { 32, "LEFT_FORETWIST2" },
            { 33, "LEFT_HAND" },
            { 34, "LEFT_FINGER_0" },
            { 35, "LEFT_FINGER_01" },
            { 36, "LEFT_FINGER_02" },
            { 37, "LEFT_FINGER_1" },
            { 38, "LEFT_FINGER_11" },
            { 39, "LEFT_FINGER_12" },
            { 40, "LEFT_FINGER_2" },
            { 41, "LEFT_FINGER_21" },
            { 42, "LEFT_FINGER_22" },
            { 43, "LEFT_FINGER_3" },
            { 44, "LEFT_FINGER_31" },
            { 45, "LEFT_FINGER_32" },
            { 46, "LEFT_FINGER_4" },
            { 47, "LEFT_FINGER_41" },
            { 48, "LEFT_FINGER_42" },
            { 49, "RIGHT_CLAVICLE" },
            { 50, "RIGHT_UPPER_ARM" },
            { 51, "RIGHT_UPPER_ARM_TWIST" },
            { 52, "RIGHT_UPPER_ARM_TWIST1" },
            { 53, "RIGHT_UPPER_ARM_TWIST2" },
            { 54, "RIGHT_FOREARM" },
            { 55, "RIGHT_FORETWIST" },
            { 56, "RIGHT_FORETWIST1" },
            { 57, "RIGHT_FORETWIST2" },
            { 58, "RIGHT_HAND" },
            { 59, "RIGHT_FINGER_0" },
            { 60, "RIGHT_FINGER_01" },
            { 61, "RIGHT_FINGER_02" },
            { 62, "RIGHT_FINGER_1" },
            { 63, "RIGHT_FINGER_11" },
            { 64, "RIGHT_FINGER_12" },
            { 65, "RIGHT_FINGER_2" },
            { 66, "RIGHT_FINGER_21" },
            { 67, "RIGHT_FINGER_22" },
            { 68, "RIGHT_FINGER_3" },
            { 69, "RIGHT_FINGER_31" },
            { 70, "RIGHT_FINGER_32" },
            { 71, "RIGHT_FINGER_4" },
            { 72, "RIGHT_FINGER_41" },
            { 73, "RIGHT_FINGER_42" },
            { 74, "XTRA_MUS_R_HIP" },
            { 75, "XTRA_MUS_L_HIP" },
            { 76, "RIGHT_HAND_ATTACHER" },
            { 77, "LEFT_HAND_ATTACHER" },
            { 78, "HIP_ATTACHER" },
            { 79, "BACK_ATTACHER" },
            { 80, "FREE_ATTACHER" },
            { 81, "CAMERA_ATTACHER_01" },
            { 82, "CAMERA_ATTACHER_02" },
            { 83, "FREE_ATTACHER_PROP_01" },
            { 84, "FREE_ATTACHER_PROP_02" },
            { 85, "LEFT_EYE" },
            { 86, "LEFT_EYE_LID" },
            { 87, "RIGHT_EYE" },
            { 88, "RIGHT_EYE_LID" },
            { 89, "LOWER_LIP" },
            { 90, "JAW" },
            { 91, "HOLSTER_ATTACHER" },
            { 92, "LEFT_WRIST_EFFECTOR" },
            { 93, "LEFT_FINGER_0_EFFECTOR" },
            { 94, "LEFT_FINGER_1_EFFECTOR" },
            { 95, "LEFT_FINGER_2_EFFECTOR" },
            { 96, "LEFT_FINGER_3_EFFECTOR" },
            { 97, "LEFT_FINGER_4_EFFECTOR" },
            { 98, "RIGHT_WRIST_EFFECTOR" },
            { 99, "RIGHT_FINGER_0_EFFECTOR" },
            { 100, "RIGHT_FINGER_1_EFFECTOR" },
            { 101, "RIGHT_FINGER_2_EFFECTOR" },
            { 102, "RIGHT_FINGER_3_EFFECTOR" },
            { 103, "RIGHT_FINGER_4_EFFECTOR" },
            { 104, "XTRA_MUS_L_PEC" },
            { 105, "XTRA_MUS_R_PEC" },
            { 106, "XTRA_MUS_NECK" },
            { 107, "XTRA_MUS_L_ELBOW" },
            { 108, "XTRA_MUS_R_ELBOW" },
            { 109, "XTRA_MUS_L_ASS" },
            { 110, "XTRA_MUS_R_ASS" },
            { 111, "XTRA_MUS_L_TOE" },
            { 112, "XTRA_MUS_L_KNEE" },
            { 113, "XTRA_MUS_L_FOOT" },
            { 114, "XTRA_MUS_R_TOE" },
            { 115, "XTRA_MUS_R_KNEE" },
            { 116, "XTRA_MUS_R_FOOT" },
            { 117, "XTRA_TARGET_R_FOOT" },
            { 118, "XTRA_PIVOT_R_HEEL" },
            { 119, "XTRA_TARGET_R_KNEE" },
            { 120, "XTRA_ALIGN_R_KNEE" },
            { 121, "XTRA_TARGET_L_FOOT" },
            { 122, "XTRA_PIVOT_L_HEEL" },
            { 123, "XTRA_TARGET_L_KNEE" },
            { 124, "XTRA_ALIGN_L_KNEE" },
            { 125, "XTRA_TARGET_R_ASS" },
            { 126, "XTRA_TARGET_L_ASS" },
            { 127, "XTRA_ALIGN_L_ASS" },
            { 128, "XTRA_ALIGN_R_ASS" },
            { 129, "XTRA_UP_THIGH" },
            { 130, "XTRA_TARGET_R_PEC" },
            { 131, "XTRA_ALIGN_R_ELBOW" },
            { 132, "XTRA_TARGET_R_ELBOW" },
            { 133, "XTRA_TARGET_L_PEC" },
            { 134, "XTRA_ALIGN_L_ELBOW" },
            { 135, "XTRA_TARGET_L_ELBOW" },
            { 136, "XTRA_ALIGN_L_PEC" },
            { 137, "XTRA_UP_PEC" },
            { 138, "XTRA_ALIGN_R_PEC" },
            { 139, "XTRA_ALIGN_NECK" },
            { 140, "HEAD_ATTACHER" },
            { 141, "RIFLE_HOLSTER_ATTACHER" },
            { 142, "SPINE1_ATTACHER" },
            { 143, "PISTOL_HOLSTER_ATTACHER" },
            { 144, "BONEID_LAST" }
        };

        (*enum_map)["ZVsTargetPicker.ETargetSelectionType"] = std::map<uint32_t, std::string>{
            { 0, "eTST_Random" },
            { 1, "eTST_Fair" },
            { 2, "eTST_Max" }
        };

        (*enum_map)["ZAvoidDangerousAreaGroup.EAvoidDangerousAreaGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_SelectDestination" },
            { 1, "IGS_Move" },
            { 2, "IGS_Jump" },
            { 3, "IGS_Wait" },
            { 4, "IGS_Completed" },
            { 5, "IGS_Max" }
        };

        (*enum_map)["EActorLookAtTargetType"] = std::map<uint32_t, std::string>{
            { 0, "eALAT_Invalid" },
            { 1, "eALAT_PositionTarget" },
            { 2, "eALAT_KnownEntityType" },
            { 3, "eALAT_PointOfInterest" },
            { 4, "eALAT_Hero" },
            { 5, "eALAT_Screenplay" },
            { 6, "eALAPT_Max" }
        };

        (*enum_map)["ECustomSoundDefType"] = std::map<uint32_t, std::string>{
            { 0, "eCSDT_Distraction_InvestigationAck" },
            { 1, "eCSDT_Distraction_InvestigationInvestigate" },
            { 2, "eCSDT_Distraction_InvestigationStnd" }
        };

        (*enum_map)["EWeaponSpecialSituation"] = std::map<uint32_t, std::string>{
            { 0, "WSS_NORMAL" },
            { 1, "WSS_BAREHANDS" },
            { 2, "WSS_EXPLOSION" },
            { 3, "WSS_ACCIDENT" },
            { 4, "WSS_NPC_FRIENDLY_FIRE" },
            { 5, "WSS_EXECUTE_PISTOL" },
            { 6, "WSS_ANY_WEAPON" },
            { 7, "WSS_INVISIBLE" }
        };

        (*enum_map)["EAIModifierScope"] = std::map<uint32_t, std::string>{
            { 0, "AIMS_Volume" },
            { 1, "AIMS_Behavior" },
            { 2, "AIMS_Role" },
            { 3, "AIMS_Item" },
            { 4, "AIMS_Situation" },
            { 5, "AIMS_Outfit" },
            { 6, "AIMS_Override" },
            { 7, "AIMS_Status" }
        };

        (*enum_map)["ZHM5WeaponEventConsumer.EAnimWeapon"] = std::map<uint32_t, std::string>{
            { 0, "eHM5Weapon_HideClip" },
            { 1, "eHM5Weapon_ShowClip" },
            { 2, "eHM5Weapon_SpawnPhysicsClip" },
            { 3, "eHM5Weapon_EjectCartridge" }
        };

        (*enum_map)["EStashpointContainedEntityType"] = std::map<uint32_t, std::string>{
            { 0, "PICKUP_NONE" },
            { 1, "PICKUP_ITEMS" },
            { 2, "PICKUP_OUTFIT" },
            { 3, "PICKUP_PICKEDUP" }
        };

        (*enum_map)["ESoundCollisionType"] = std::map<uint32_t, std::string>{
            { 0, "Impact" },
            { 1, "Rolling" },
            { 2, "Sliding" }
        };

        (*enum_map)["ICloset.EClosetType"] = std::map<uint32_t, std::string>{
            { 0, "eTypeClosetNormal" },
            { 1, "eTypeClosetCake" },
            { 2, "eTypeClosetDumpster" },
            { 3, "eTypeClosetLast" }
        };

        (*enum_map)["EHitmanPermissionFlag"] = std::map<uint32_t, std::string>{
            { 0, "eHPFlag_InventorySelect" },
            { 1, "eHPFlag_CanHolsterItem" },
            { 2, "eHPFlag_CanDropItem" },
            { 3, "eHPFlag_CanDualWield" },
            { 4, "eHPFlag_CameraControl" },
            { 5, "eHPFlag_MovementControl" },
            { 6, "eHPFlag_AimControl" },
            { 7, "eHPFlag_CanOpenNotebook" },
            { 8, "eHPFlag_CanOpenPauseMenu" },
            { 9, "eHPFlag_PermissionsNUM" }
        };

        (*enum_map)["EActionRadialArcIconType"] = std::map<uint32_t, std::string>{
            { 0, "EARAIT_NoIcon" },
            { 1, "EARAIT_Locked" },
            { 2, "EARAIT_NeedTool" },
            { 3, "EARAIT_IsRunning" },
            { 4, "EARAIT_Crowbar" },
            { 5, "EARAIT_Wrench" },
            { 6, "EARAIT_Card" },
            { 7, "EARAIT_Lockpick" },
            { 8, "EARAIT_Screwdriver" },
            { 9, "EARAIT_AmmoBullet" },
            { 10, "EARAIT_Flower" },
            { 11, "EARAIT_ChemicalTube" },
            { 12, "EARAIT_Golfball" },
            { 13, "EARAIT_Keypad" },
            { 14, "EARAIT_Key" },
            { 15, "EARAIT_Coin" },
            { 16, "EARAIT_Poison" },
            { 17, "EARAIT_Exsplosive" }
        };

        (*enum_map)["EParticleDecalSpawnEntity_Constraints"] = std::map<uint32_t, std::string>{
            { 0, "FACTOR_AND_RANDOMIZE" },
            { 1, "SIZE_RANGE" }
        };

        (*enum_map)["ZActBehaviorEntity.ERotationAlignment"] = std::map<uint32_t, std::string>{
            { 0, "RA_NONE" },
            { 1, "RA_LOOSE" },
            { 2, "RA_EXACT" }
        };

        (*enum_map)["ZPatrolBehaviorEntity.ERotationAlignment"] = std::map<uint32_t, std::string>{
            { 0, "RA_NONE" },
            { 1, "RA_LOOSE" },
            { 2, "RA_EXACT" }
        };

        (*enum_map)["EAnimBlendMode"] = std::map<uint32_t, std::string>{
            { 0, "EAnimBlendMode_InterpAttInterpPos" },
            { 1, "EAnimBlendMode_InterpAttAddPos" },
            { 2, "EAnimBlendMode_AddAttLeavePos" },
            { 3, "EAnimBlendMode_AddAttAddPos" }
        };

        (*enum_map)["ETeamModeId"] = std::map<uint32_t, std::string>{
            { 0, "TeamMode_Coop" },
            { 1, "TeamMode_Versus" }
        };

        (*enum_map)["ZUIControlEntity.EScaleMode"] = std::map<uint32_t, std::string>{
            { 0, "Manual" },
            { 1, "ResolutionScale" },
            { 2, "ResolutionScaleAspect" },
            { 3, "ResolutionScaleAspectFill" }
        };

        (*enum_map)["ZInputListenerActorPool.eInputListenerButtons"] = std::map<uint32_t, std::string>{
            { 0, "EILB_UP" },
            { 1, "EILB_LEFT" },
            { 2, "EILB_DOWN" },
            { 3, "EILB_RIGHT" }
        };

        (*enum_map)["ERuntimeMemoryAllocationState"] = std::map<uint32_t, std::string>{
            { 0, "eRMAS_NeverAllocated" },
            { 1, "eRMAS_Allocated" },
            { 2, "eRMAS_Released" }
        };

        (*enum_map)["CrowdRegionType"] = std::map<uint32_t, std::string>{
            { 0, "CROWDSPHERE_NONE" },
            { 1, "CROWDSPHERE_SAFE" },
            { 2, "CROWDSPHERE_POI" },
            { 3, "CROWDSPHERE_AVOID" },
            { 4, "CROWDSPHERE_RELOCATE" },
            { 5, "CROWDSPHERE_STOP" },
            { 6, "CROWDSPHERE_ALERT" },
            { 7, "CROWDSPHERE_SCARE" },
            { 8, "CROWDSPHERE_GETDOWN" },
            { 9, "CROWDSPHERE_DIE" }
        };

        (*enum_map)["EDramaEventAction"] = std::map<uint32_t, std::string>{
            { 0, "eDEA_NONE" },
            { 8, "eDEA_DONT_STOP_SPEAK" },
            { 16, "eDEA_RESET_CASTING" },
            { 32, "eDEA_RESET_STATE" },
            { 1, "eDEA_TERMINATE_DRAMA" },
            { 2, "eDEA_DONE_DRAMA" },
            { 4, "eDEA_STOP_BEHAVIOR" }
        };

        (*enum_map)["ZRecoverUnconsciousGroup.EInvestigateGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_InitialState" },
            { 1, "IGS_WaitingForVictim" },
            { 2, "IGS_Recovering" },
            { 3, "IGS_Relocating" },
            { 4, "IGS_Reporting" },
            { 5, "IGS_Completed" },
            { 6, "IGS_Max" }
        };

        (*enum_map)["ECharacterActionRequests"] = std::map<uint32_t, std::string>{
            { 0, "eSM_AR_None" },
            { 1, "eSM_AR_ReloadR" },
            { 2, "eSM_AR_ReloadL" },
            { 3, "eSM_AR_ShootR" },
            { 4, "eSM_AR_ShootL" },
            { 5, "eSM_AR_SwapHands" },
            { 6, "eSM_AR_HolsterR" },
            { 7, "eSM_AR_HolsterL" },
            { 8, "eSM_AR_UnholsterR" },
            { 9, "eSM_AR_UnholsterL" },
            { 10, "eSM_AR_PickupItemR" },
            { 11, "eSM_AR_PickupItemL" },
            { 12, "eSM_AR_InteractionR" },
            { 13, "eSM_AR_InteractionL" },
            { 14, "eSM_AR_InteractionSwipe" },
            { 15, "eSM_AR_InteractionSwipeR" },
            { 16, "eSM_AR_InteractionSwipeL" },
            { 17, "eSM_AR_OpenDoor" },
            { 18, "eSM_AR_OpenDoorR" },
            { 19, "eSM_AR_OpenDoorL" },
            { 20, "eSM_AR_CloseCombat" },
            { 21, "eSM_AR_Movement" },
            { 22, "eSM_AR_Pretend" },
            { 23, "eSM_AR_Sniping" },
            { 24, "eSM_AR_EndSniping" },
            { 25, "eSM_AR_RetrieveFromSuitcase" },
            { 26, "eSM_AR_Marking" },
            { 27, "eSM_AR_ChangeAmmo" },
            { 28, "eSM_AR_Assemble" },
            { 29, "eSM_AR_Assemble_UnequippedContainer" },
            { 30, "eSM_AR_Disassemble" },
            { 31, "eSM_AR_Disassemble_PickingUp" },
            { 32, "eSM_AR_AssembleUB" },
            { 33, "eSM_AR_DisassembleUB" },
            { 34, "eSM_AR_Cancel" },
            { 35, "eSM_AR_Sprint" },
            { 36, "eSM_AR_CloseHandR" },
            { 37, "eSM_AR_CloseHandL" }
        };

        (*enum_map)["ZHM5HIKEventConsumer.EBlend"] = std::map<uint32_t, std::string>{
            { 0, "eBlendIn" },
            { 1, "eBlendOut" },
            { 2, "eInstantOn" },
            { 3, "eInstantOff" }
        };

        (*enum_map)["EWeaponUpgradeIcon"] = std::map<uint32_t, std::string>{
            { 0, "WEAPON_UPGRADE_ICON_NONE" },
            { 1, "WEAPON_UPGRADE_ICON_SILENCER" },
            { 2, "WEAPON_UPGRADE_ICON_RED_DOT_SIGHT" },
            { 3, "WEAPON_UPGRADE_ICON_AMMUNITION" },
            { 4, "WEAPON_UPGRADE_ICON_MAGAZINE" },
            { 5, "WEAPON_UPGRADE_ICON_EXTCLIP" },
            { 6, "WEAPON_UPGRADE_ICON_SHELLS" },
            { 7, "WEAPON_UPGRADE_ICON_SLIDE" },
            { 8, "WEAPON_UPGRADE_ICON_BOLT" },
            { 9, "WEAPON_UPGRADE_ICON_BARREL" },
            { 10, "WEAPON_UPGRADE_ICON_HAMMER" },
            { 11, "WEAPON_UPGRADE_ICON_CHUTE" },
            { 12, "WEAPON_UPGRADE_ICON_DUALWIELD" },
            { 13, "WEAPON_UPGRADE_ICON_MAGNUM" },
            { 14, "WEAPON_UPGRADE_ICON_STOCK" }
        };

        (*enum_map)["ECollisionResponse"] = std::map<uint32_t, std::string>{
            { 0, "REFLECT_VELOCITY" },
            { 1, "SLIDE_ON" },
            { 2, "STAY_AT_COLLISION_POINT" },
            { 3, "CUSTOM" }
        };

        (*enum_map)["ZDramaSituationEntity.EReentryBehavior"] = std::map<uint32_t, std::string>{
            { 0, "RB_CONTINUE" },
            { 1, "RB_TERMINATE" },
            { 2, "RB_RESTART" },
            { 3, "RB_RECASTANDRESTART" },
            { 4, "RB_RECASTANDCONTINUE" }
        };

        (*enum_map)["IScatterContainerEntity.EResizeMode"] = std::map<uint32_t, std::string>{
            { 0, "MODE_CLEAR" },
            { 1, "MODE_KEEPASCENTER" },
            { 2, "MODE_SCALE" }
        };

        (*enum_map)["ZBoneWeightRotationEntity.eRotationAxises"] = std::map<uint32_t, std::string>{
            { 0, "AXIS_X" },
            { 1, "AXIS_Y" },
            { 2, "AXIS_Z" }
        };

        (*enum_map)["EGameTension"] = std::map<uint32_t, std::string>{
            { 0, "EGT_Undefined" },
            { 1, "EGT_Ambient" },
            { 2, "EGT_Agitated" },
            { 3, "EGT_Searching" },
            { 4, "EGT_AlertedLow" },
            { 5, "EGT_AlertedHigh" },
            { 6, "EGT_Hunting" },
            { 7, "EGT_Arrest" },
            { 8, "EGT_Combat" }
        };

        (*enum_map)["ERegionMask"] = std::map<uint32_t, std::string>{
            { 1, "eRM_None" },
            { 2, "eRM_LowCover" },
            { 4, "eRM_HighCover" },
            { 128, "eRM_User_3" },
            { 64, "eRM_User_2" },
            { 32, "eRM_User_1" },
            { 16, "eRM_Separator" },
            { 4096, "eRM_User_8" },
            { 8, "eRM_Stairs" },
            { 256, "eRM_User_4" },
            { 512, "eRM_User_5" },
            { 1024, "eRM_User_6" },
            { 2048, "eRM_User_7" }
        };

        (*enum_map)["ECharacterAnimEvent"] = std::map<uint32_t, std::string>{
            { 699, "eCAE_ReloadStarted" },
            { 700, "eCAE_ReloadedClip" },
            { 9051, "eCAE_OpenDoorActivate" },
            { 701, "eCAE_ReloadCompleted" },
            { 703, "eCAE_ReloadSwapRHandAttacher" },
            { 9040, "eCAE_SwapHandsRtoL" },
            { 711, "eCAE_ReloadedOneBullet" },
            { 702, "eCAE_ReloadUpdate" },
            { 712, "eCAE_ChangeAmmoCompleted" },
            { 704, "eCAE_ReloadResetRHandSwap" },
            { 9071, "eCAE_AssembleAttach" },
            { 9010, "eCAE_UnholsterAttachL" },
            { 9074, "eCAE_AssembleContainerAttach" },
            { 9011, "eCAE_UnholsterCompletedL" },
            { 9075, "eCAE_AssembleEarlyOut" },
            { 9012, "eCAE_UnholsterAttachR" },
            { 9013, "eCAE_UnholsterCompletedR" },
            { 9020, "eCAE_HolsterDetachL" },
            { 9021, "eCAE_HolsterCompletedL" },
            { 9022, "eCAE_HolsterDetachR" },
            { 9023, "eCAE_HolsterCompletedR" },
            { 9030, "eCAE_SwipeActivate" },
            { 9031, "eCAE_SwipeCompleted" },
            { 10003, "eCAE_HandIKDisableRight" },
            { 5001, "eCAE_InteractionActivate" },
            { 5000, "eCAE_InteractionCompleted" },
            { 9041, "eCAE_SwapHandsRtoLCompleted" },
            { 9042, "eCAE_SwapHandsLtoR" },
            { 9043, "eCAE_SwapHandsLtoRCompleted" },
            { 9050, "eCAE_OpenDoorCompleted" },
            { 9060, "eCAE_FlavorIdleCompleted" },
            { 9070, "eCAE_AssemblePlaceAlignCompleted" },
            { 10002, "eCAE_HandIKEnableRight" },
            { 9073, "eCAE_AssembleToSneak" },
            { 9072, "eCAE_AssembleCompleted" },
            { 10000, "eCAE_HandIKEnableLeft" },
            { 10001, "eCAE_HandIKDisableLeft" },
            { 50, "eCAE_EarlyExit" }
        };

        (*enum_map)["EMatchOverCondition"] = std::map<uint32_t, std::string>{
            { 0, "CompletedNormally" },
            { 1, "OpponentsDisconnected" },
            { 2, "OpponentsAbandoned" }
        };

        (*enum_map)["CrowdUtil.ECrowdStandingPush"] = std::map<uint32_t, std::string>{
            { 0, "PUSH_FRONT" },
            { 1, "PUSH_FRONT_LEFT" },
            { 2, "PUSH_LEFT" },
            { 3, "PUSH_BACK_LEFT" },
            { 4, "PUSH_BACK" },
            { 5, "PUSH_BACK_RIGHT" },
            { 6, "PUSH_RIGHT" },
            { 7, "PUSH_FRONT_RIGHT" }
        };

        (*enum_map)["EDialogueLine"] = std::map<uint32_t, std::string>{
            { 0, "Hello" },
            { 1, "MoveAlong" },
            { 2, "WhatsGoingOn" },
            { 3, "DropWeapon01" },
            { 4, "DropWeapon02" },
            { 5, "DropWeapon03" },
            { 6, "DropWeapon04" },
            { 7, "DropWeaponSecondWarning" },
            { 8, "Comply01" },
            { 9, "Comply02" },
            { 10, "YouAskedForIt" },
            { 11, "DropWeapon_Single01" },
            { 12, "DropWeapon_Double01" },
            { 13, "DropWeapon_Multiple01" },
            { 14, "DropWeapon_Comply_Single01" },
            { 15, "DropWeapon_Comply_Double01" },
            { 16, "DropWeapon_Comply_Multiple01" },
            { 17, "DropWeapon_RunAway01" },
            { 18, "DropWeapon_RunAway_Comply01" },
            { 19, "DropWeapon_Escalate_Single01" },
            { 20, "DropWeapon_Escalate_Double01" },
            { 21, "DropWeapon_Escalate_Multiple01" }
        };

        (*enum_map)["EPostProcessorComponentType"] = std::map<uint32_t, std::string>{
            { 1, "MRP_LOOKAT" },
            { 2, "MRP_AIM" },
            { 16, "MRP_LEDGEFEETLOCK" },
            { 32, "MRP_LEDGEHANG" },
            { 2048, "MRP_BLINDFIRE" },
            { 1024, "MRP_IMPACT" }
        };

        (*enum_map)["WebSocketUtils.ECloseStatus"] = std::map<uint32_t, std::string>{
            { 1000, "eCS_NORMAL" },
            { 1001, "eCS_GOINGAWAY" },
            { 1002, "eCS_PROTOCOLERROR" },
            { 1003, "eCS_INVALIDDATA" },
            { 1004, "eCS_1004" },
            { 1005, "eCS_NOSTATUSRCVD" },
            { 1006, "eCS_ABNORMALCLOSURE" },
            { 1007, "eCS_INVALIDPAYLOAD" },
            { 1008, "eCS_POLICYVIOLATION" },
            { 1009, "eCS_MSGTOOBIG" },
            { 1010, "eCS_MANDATORYEXT" },
            { 1011, "eCS_INTERNALSRVERR" },
            { 1015, "eCS_TLSHANDSHAKE" }
        };

        (*enum_map)["EControlButtonName"] = std::map<uint32_t, std::string>{
            { 0, "eCN_ABORT" },
            { 1, "eCN_ACTION" },
            { 2, "eCN_ACTIVATE_PROP" },
            { 3, "eCN_AGILITY_DOWN" },
            { 4, "eCN_AGILITY_ENTERWINDOW" },
            { 5, "eCN_AGILITY_SNEAKPASTWINDOW" },
            { 6, "eCN_AGILITY_THROWOVERRAIL" },
            { 7, "eCN_AGILITY_UP" },
            { 8, "eCN_AIM" },
            { 9, "eCN_COVER_ENTER" },
            { 10, "eCN_COVER_TAKEDOWN" },
            { 11, "eCN_COVER_TO_COVER" },
            { 12, "eCN_CROUCH" },
            { 13, "eCN_DRAGBODY" },
            { 14, "eCN_DUMPBODY" },
            { 15, "eCN_FIBERWIRE" },
            { 16, "eCN_INSTINCT" },
            { 17, "eCN_INVENTORY_HOLSTER" },
            { 18, "eCN_INVENTORY_LONGRANGE" },
            { 19, "eCN_MARK_TARGET" },
            { 20, "eCN_CHANGE_AMMO_NEXT" },
            { 21, "eCN_CHANGE_AMMO_PREVIOUS" },
            { 22, "eCN_INVENTORY_PROP" },
            { 23, "eCN_INVENTORY_SHORTRANGE" },
            { 24, "eCN_ITEM_DROP" },
            { 25, "eCN_ITEM_THROW" },
            { 26, "eCN_MELEE_HIT" },
            { 27, "eCN_MELEE_TAKEDOWN" },
            { 28, "eCN_PICKUP" },
            { 29, "eCN_RUN" },
            { 30, "eCN_WALK_SLOW" },
            { 31, "eCN_CONCEAL_RETRIEVE" },
            { 32, "eCN_SB_ACTIVATE" },
            { 33, "eCN_SB_CANCEL" },
            { 34, "eCN_SB_EXECUTE" },
            { 35, "eCN_SB_REMOVETAG" },
            { 36, "eCN_SHOOT" },
            { 37, "eCN_NOTEBOOK" },
            { 38, "eCN_PAUSE" },
            { 39, "eCN_NO_ICON" }
        };

        (*enum_map)["ZInvestigateDisguiseGroup.EState"] = std::map<uint32_t, std::string>{
            { 0, "eInit" },
            { 1, "eGuardBanter" },
            { 2, "eAcknowledge" },
            { 3, "eMain" },
            { 4, "eSearch" },
            { 5, "eMoveback" },
            { 6, "eStandDown" },
            { 7, "eEscalate" },
            { 8, "eDone" }
        };

        (*enum_map)["ECOMUsage"] = std::map<uint32_t, std::string>{
            { 0, "ECOMUSAGE_AUTOCOMPUTE" },
            { 1, "ECOMUSAGE_PIVOT" }
        };

        (*enum_map)["EAIEventType"] = std::map<uint32_t, std::string>{
            { 0, "AIET_None" },
            { 1, "AIET_Ignore" },
            { 2, "AIET_PreventAutoPurge" },
            { 3, "AIET_ShootTarget" },
            { 4, "AIET_Hunting" },
            { 5, "AIET_Order_Target" },
            { 6, "AIET_END_BOOLEAN_EVENTS" },
            { 7, "AIET_LookedAt" },
            { 8, "AIET_Killer" },
            { 9, "AIET_Strangler" },
            { 10, "AIET_Suspicious" },
            { 11, "AIET_KnownBody" },
            { 12, "AIET_KnownBodyIsFoundOutsideNavmeshAndIgnored" },
            { 13, "AIET_KnownBodyIsHandled" },
            { 14, "AIET_KnownBodybag" },
            { 15, "AIET_KnownBodybagIsHandled" },
            { 16, "AIET_Dead" },
            { 17, "AIET_DisguiseSuspicious" },
            { 18, "AIET_DamagingMe" },
            { 19, "AIET_KnownInCloset" },
            { 20, "AIET_KnownInNpcUnreachableArea" },
            { 21, "AIET_Sense" },
            { 22, "AIET_HasBeenInCombat" },
            { 23, "AIET_ReportedByCamera" },
            { 24, "AIET_SOUNDEVENTS_LOW_LOUDNESS_BEGIN" },
            { 25, "AIET_SoundFootsteps" },
            { 26, "AIET_SoundThrownItemCollision" },
            { 27, "AIET_SoundGunSilenced" },
            { 28, "AIET_SoundHitmanGunSilenced" },
            { 29, "AIET_SoundLegalGunSilenced" },
            { 30, "AIET_SoundLegalHitmanGunSilenced" },
            { 31, "AIET_SoundEmptyGun" },
            { 32, "AIET_SoundCCBreakNeck" },
            { 33, "AIET_SoundCCChoke" },
            { 34, "AIET_SoundCCFiberwire" },
            { 35, "AIET_SoundBulletImpact" },
            { 36, "AIET_SoundLegalBulletImpact" },
            { 37, "AIET_SoundSpeakWarning" },
            { 38, "AIET_SoundSpeakAngry" },
            { 39, "AIET_SOUNDEVENTS_NORMAL_LOUDNESS_BEGIN" },
            { 40, "AIET_SoundRadio" },
            { 41, "AIET_SoundAlarm" },
            { 42, "AIET_SoundSpeakPainBehind" },
            { 43, "AIET_SoundCloseCombat" },
            { 44, "AIET_SoundSpeakPain" },
            { 45, "AIET_SoundSetpiece" },
            { 46, "AIET_SOUNDEVENTS_LOUD_LOUDNESS_BEGIN" },
            { 47, "AIET_SoundSpeakHelp" },
            { 48, "AIET_SoundSpeakHelpLoud" },
            { 49, "AIET_SoundGunShot" },
            { 50, "AIET_SoundHitmanGunShot" },
            { 51, "AIET_SoundLegalGunShot" },
            { 52, "AIET_SoundLegalHitmanGunShot" },
            { 53, "AIET_SoundExplosion" },
            { 54, "AIET_SoundAccident" },
            { 55, "AIET_SoundCurious" },
            { 56, "AIET_SoundCuriousItem" },
            { 57, "AIET_SoundInteresting" },
            { 58, "AIET_SoundCrowdCurious" },
            { 59, "AIET_SoundCrowdAlerted" },
            { 60, "AIET_SoundCrowdWantsArrest" },
            { 61, "AIET_SoundCrowdWantsArrestKiller" },
            { 62, "AIET_SOUNDEVENTS_END" },
            { 63, "AIET_SightReaction" },
            { 64, "AIET_SightInvestigation" },
            { 65, "AIET_PerceptibleEntityAware" },
            { 66, "AIET_InSight" },
            { 67, "AIET_SlowMovementInVegetationInSight" },
            { 68, "AIET_MediumMovementInVegetationInSight" },
            { 69, "AIET_FastMovementInVegetationInSight" },
            { 70, "AIET_VisualSetpiece" },
            { 71, "AIET_DirectSetpiece" },
            { 72, "AIET_Sick" },
            { 73, "AIET_MyFlashGrenadeExploded" },
            { 74, "AIET_StunnedByFlashGrenade" },
            { 75, "AIET_WasSlapped" },
            { 76, "AIET_Attention_Stage_1" },
            { 77, "AIET_Attention_Stage_2" },
            { 78, "AIET_AttentionFading" },
            { 79, "AIET_FullAttention" },
            { 80, "AIET_FullDisguiseAttention" },
            { 81, "AIET_ReactedTo" },
            { 82, "AIET_ReactTarget" },
            { 83, "AIET_ReactSuppressed" },
            { 84, "AIET_StanddownHolster" },
            { 85, "AIET_ForcedIllegal" },
            { 86, "AIET_HitByItem" },
            { 87, "AIET_HitByCoin" },
            { 88, "AIET_LTHitByItemOrCoin" },
            { 89, "AIET_HitByItemOrCoinTwice" },
            { 90, "AIET_HitByThis" },
            { 91, "AIET_SeenInAccident" },
            { 92, "AIET_Trespassing_Strike_1" },
            { 93, "AIET_Trespassing_Strike_2" },
            { 94, "AIET_Trespassing_Strike_3" },
            { 95, "AIET_DistractedByMovementInVegetation_Once" },
            { 96, "AIET_DistractedByMovementInVegetation_Twice" },
            { 97, "AIET_Investigate_Strike_1" },
            { 98, "AIET_Investigate_Strike_2" },
            { 99, "AIET_DisguiseEnforcer" },
            { 100, "AIET_Suspect" },
            { 101, "AIET_InSentryZone" },
            { 102, "AIET_MySentryZone" },
            { 103, "AIET_TriggerAlarm" },
            { 104, "AIET_LockdownIntruder" },
            { 105, "AIET_AccuseUnconscious" },
            { 106, "AIET_HasKnockMeDown" },
            { 107, "AIET_HasKnockSomeoneDown" },
            { 108, "AIET_InMyPrivateArea" },
            { 109, "AIET_KnownNaked" },
            { 110, "AIET_KnownDraggingBodybag" },
            { 111, "AIET_InvestigateOrder" },
            { 112, "AIET_ReactedSurprised" },
            { 113, "AIET_ReportGotShot" },
            { 114, "AIET_HasReportedDeadBodySituation" },
            { 115, "AIET_Social_TooClose" },
            { 116, "AIET_Social_Privacy" },
            { 117, "AIET_Social_Silly" },
            { 118, "AIET_ReactedToSituation" },
            { 119, "AIET_SeeAttached" },
            { 120, "AIET_WasInvisible" },
            { 121, "AIET_BlameHitman" },
            { 122, "AIET_BlamedByUnconscious" },
            { 123, "AIET_DontCareAboutBlaming" },
            { 124, "AIET_Combat_PropagatedBy" },
            { 125, "AIET_Combat_WithinPropagationField" },
            { 126, "AIET_Combat_MarkedAsDiscardable" },
            { 127, "AIET_Combat_AttackWitness" },
            { 128, "AIET_Combat_HomingAttackOrigin" },
            { 129, "AIET_Combat_VIPWasAttackedFromPosition" },
            { 130, "AIET_ReactedToSniperKnowledge" },
            { 131, "AIET_SniperCombatTarget" },
            { 132, "AIET_SniperCombatAimTarget" },
            { 133, "AIET_CantReachTarget" },
            { 134, "AIET_HuntingSniper" },
            { 135, "AIET_NeedCover" },
            { 136, "AIET_WaitingForSniper" },
            { 137, "AIET_HeardSniperShooting" },
            { 138, "AIET_KnownHidden" },
            { 139, "AIET_SituationTarget" },
            { 140, "AIET_TalkingToHitman" },
            { 141, "AIET_SuspiciousAction" },
            { 142, "AIET_RadioCallHelpingActor" },
            { 143, "AIET_WeaponContextKnown" },
            { 144, "AIET_IdleStandDown" },
            { 145, "AIET_IgnoreInvestigation" },
            { 146, "AIET_NewDisguiseTarget" },
            { 147, "AIET_SocialTarget" },
            { 148, "AIET_SecurityCamBroken" },
            { 149, "AIET_HeWoundedMeLongDistance" },
            { 150, "AIET_HeWoundedMeShortDistance" },
            { 151, "AIET_HeWoundedMeCC" },
            { 152, "AIET_HeAimedAtMe" },
            { 153, "AIET_BumpedIntoMe" },
            { 154, "AIET_ReactToDistraction" },
            { 155, "AIET_MovingToTarget" },
            { 156, "AIET_ReachedTarget" },
            { 157, "AIET_DistractionChainPart" },
            { 158, "AIET_NewDistractionChainPart" },
            { 159, "AIET_ChainFixed" },
            { 160, "AIET_SurprisePlayed" },
            { 161, "AIET_NewDistraction" },
            { 162, "AIET_WasSilly" },
            { 163, "AIET_Distraction_My_Leader" },
            { 164, "AIET_Distraction_My_Assistant" },
            { 165, "AIET_Distraction_Was_My_Assistant" },
            { 166, "AIET_AssitingWithSuitcase" },
            { 167, "AIET_SuitcaseAssistanceFaceToFace" },
            { 168, "AIET_SuitcaseAssistanceRadio" },
            { 169, "AIET_SC_CombatPosition" },
            { 170, "AIET_SC_CombatPositionMove" },
            { 171, "AIET_SC_CombatPositionFight" },
            { 172, "AIET_SC_PreventMovingThere" },
            { 173, "AIET_SC_JumpyReaction" },
            { 174, "AIET_DISTRACTION_TYPES_START" },
            { 175, "AIET_SeeInCoverDistraction" },
            { 176, "AIET_SeeSneakingDistraction" },
            { 177, "AIET_SeeStunnedDistraction" },
            { 178, "AIET_SeeSickDistraction" },
            { 179, "AIET_LegalItemDroppedDistraction" },
            { 180, "AIET_IllegalItemDroppedDistraction" },
            { 181, "AIET_BumpedDistraction" },
            { 182, "AIET_SightReactionDistraction" },
            { 183, "AIET_SightInvestigationDistraction" },
            { 184, "AIET_HearItemImpactDistraction" },
            { 185, "AIET_SeeCuriousPerceptibleDistraction" },
            { 186, "AIET_SeeCuriousItemDistraction" },
            { 187, "AIET_SeeFlyingItemDistraction" },
            { 188, "AIET_SeeThrowingDistraction" },
            { 189, "AIET_HitBySomethingDistraction" },
            { 190, "AIET_HearFootstepsDistraction" },
            { 191, "AIET_SeeAgility" },
            { 192, "AIET_SeeAgility_Vault" },
            { 193, "AIET_SeeAgility_Window" },
            { 194, "AIET_SeeHidding" },
            { 195, "AIET_HearStrangeSound" },
            { 196, "AIET_HearAlarm" },
            { 197, "AIET_HearRadio" },
            { 198, "AIET_HearAngry" },
            { 199, "AIET_HearHelp" },
            { 200, "AIET_HearAccident" },
            { 201, "AIET_HearCuriousCrowd" },
            { 202, "AIET_HearBulletImpact" },
            { 203, "AIET_HearPain" },
            { 204, "AIET_HearCuriousSound" },
            { 205, "AIET_HearCuriousItemSound" },
            { 206, "AIET_SecurityCameraBroken" },
            { 207, "AIET_SetpieceDistractionDefault" },
            { 208, "AIET_SeeItemTossDistraction" },
            { 209, "AIET_SetpieceDistractionDoor" },
            { 210, "AIET_SetpieceDistractionExperimental1" },
            { 211, "AIET_SetpieceDistractionExperimental2" },
            { 212, "AIET_SeeSuitcaseDistraction" },
            { 213, "AIET_Distraction_Silly" },
            { 214, "AIET_Distraction_TooClose" },
            { 215, "AIET_Distraction_Privacy" },
            { 216, "AIET_Distraction_MovementInVegetationBrief" },
            { 217, "AIET_Distraction_MovementInVegetationCurious" },
            { 218, "AIET_DISTRACTION_TYPES_END" },
            { 219, "AIET_ARREST_REASONS_START" },
            { 220, "AIET_ArrestReason_IllegalItem" },
            { 221, "AIET_ArrestReason_DragBody" },
            { 222, "AIET_ArrestReason_KnockDown" },
            { 223, "AIET_ARREST_REASONS_END" },
            { 224, "AIET_END_NORMAL_EVENTS" },
            { 225, "AIET_DEBUG_EVENTS_START" },
            { 226, "AIET_Debug_ThrowFlashbangHere" },
            { 227, "AIET_DEBUG_EVENTS_END" }
        };

        (*enum_map)["EHM5SoundCloseCombatEvent"] = std::map<uint32_t, std::string>{
            { 0, "ECCE_PUNCH_FACE" },
            { 1, "ECCE_PUNCH_BODY" },
            { 2, "ECCE_ATTACK_ELBOW" },
            { 3, "ECCE_ATTACK_KNEE" },
            { 4, "ECCE_ATTACK_KICK" },
            { 5, "ECCE_SWOOSH_SHORT" },
            { 6, "ECCE_SWOOSH_LONG" },
            { 7, "ECCE_CLOTH_SWOOSH" },
            { 8, "ECCE_CLOTH_RUSTLE" },
            { 9, "ECCE_CLOTH_STRETCH" },
            { 10, "ECCE_GRAB" },
            { 11, "ECCE_BONE_BREAK" },
            { 12, "ECCE_NECK_SNAP" },
            { 13, "ECCE_BODY_FALL" },
            { 14, "ECCE_SKULL_CRACK" },
            { 15, "ECCE_ANY" }
        };

        (*enum_map)["EVirtualPlatformID"] = std::map<uint32_t, std::string>{
            { 769, "XBONE_X" },
            { 0, "DEFAULT" },
            { 513, "PS4_PRO" },
            { 255, "__SUB" },
            { 256, "PC" },
            { 512, "PS4" },
            { 257, "PC_ENHANCED" },
            { 768, "XBONE" },
            { 1024, "STADIA" },
            { 1280, "PS5" },
            { 1536, "XBSCARLETT" },
            { 65792, "VR_PC" },
            { 1537, "XBSCARLETT_ANACONDA" },
            { 65536, "VR" },
            { 66048, "VR_PS4" },
            { 66049, "VR_PS4_PRO" },
            { 66816, "VR_PS5" },
            { 65280, "__BASE" },
            { 16711680, "__SYSTEM" }
        };

        (*enum_map)["eAmmoType"] = std::map<uint32_t, std::string>{
            { 0, "eAmmoGun" },
            { 1, "eAmmoRevolver" },
            { 2, "eAmmoSMG" },
            { 3, "eAmmoRifle" },
            { 4, "eAmmoShotgun" },
            { 5, "eAmmoSniper" },
            { 6, "eAmmoMG" },
            { 7, "eAmmoRPG" },
            { 8, "eAmmoFake" },
            { 9, "eAmmoHarmless" },
            { 10, "eAmmoLightPistol" },
            { 11, "eAmmoDartTranquilizer" },
            { 12, "eAmmoShotgunBeanbag" },
            { 13, "eUnknownAmmoType" }
        };

        (*enum_map)["ZEntityPropertyReplica.EReplicationStrategy"] = std::map<uint32_t, std::string>{
            { 0, "REP_ALL_PROPERTIES" },
            { 1, "REP_ALL_EXCEPT" },
            { 2, "REP_ONLY_SPECIFIC" }
        };

        (*enum_map)["ZApproachOrder.EApproachStyle"] = std::map<uint32_t, std::string>{
            { 0, "AS_AUTO" },
            { 1, "AS_SPRINT" },
            { 2, "AS_JOG" },
            { 3, "AS_WALK" }
        };

        (*enum_map)["ZInvestigateCautiousSituation.EBystanderState"] = std::map<uint32_t, std::string>{
            { 0, "BS_SelectCandidate" },
            { 1, "BS_SelectPoint" },
            { 2, "BS_SearchDangerField" }
        };

        (*enum_map)["ZSmuggleSituationEntity.ESmuggleState"] = std::map<uint32_t, std::string>{
            { 0, "ESS_Idle" },
            { 1, "ESS_Evaluate" },
            { 2, "ESS_FindJob" },
            { 3, "ESS_OnRouteToLocation" },
            { 4, "ESS_PickingUp" },
            { 5, "ESS_PuttingDown" },
            { 6, "ESS_OnABreak" }
        };

        (*enum_map)["ZSniperCombatSituation.ESituationState"] = std::map<uint32_t, std::string>{
            { 0, "SS_Main" },
            { 1, "SS_StandDown" },
            { 2, "SS_InvalidSituation" }
        };

        (*enum_map)["EOnlineEventInvestigationType"] = std::map<uint32_t, std::string>{
            { 0, "OEIT_Generic" },
            { 1, "OEIT_HearBulletImpact" },
            { 2, "OEIT_HearFootsteps" },
            { 3, "OEIT_CarAlarm" },
            { 4, "OEIT_HearBark" },
            { 5, "OEIT_HearPain" },
            { 6, "OEIT_HearRadio" },
            { 7, "OEIT_HearItemToss" },
            { 8, "OEIT_SeeItemToss" },
            { 9, "OEIT_HearAccident" },
            { 10, "OEIT_CuriousSound" },
            { 11, "OEIT_CuriousItemSound" },
            { 12, "OEIT_SeeWeapon" },
            { 13, "OEIT_SeeItem" },
            { 14, "OEIT_Perceptible" },
            { 15, "OEIT_Toss" },
            { 16, "OEIT_CrowdCurious" },
            { 17, "OEIT_CameraDefect" }
        };

        (*enum_map)["IItemWeapon.EDeadlyThrowType"] = std::map<uint32_t, std::string>{
            { 0, "DEADLYTHROW_NONE" },
            { 1, "DEADLYTHROW_LIGHT" },
            { 2, "DEADLYTHROW_HEAVY" }
        };

        (*enum_map)["EValueUpdateStrategy"] = std::map<uint32_t, std::string>{
            { 0, "UPDATE_STRATEGY_REPLACE" },
            { 1, "UPDATE_STRATEGY_ADD_TO" },
            { 2, "UPDATE_STRATEGY_MULTIPLY" }
        };

        (*enum_map)["ZEscortSituationEntity.ETargetState"] = std::map<uint32_t, std::string>{
            { 0, "ETS_Unknown" },
            { 1, "ETS_NoTarget" },
            { 2, "ETS_RunningActBehavior" },
            { 3, "ETS_RunningDummyBehavior" },
            { 4, "ETS_RunningOtherBehavior" },
            { 5, "ETS_Dead" },
            { 6, "ETS_TargetIsHitman" }
        };

        (*enum_map)["IRenderCompositorEntity.EViewportAnchor"] = std::map<uint32_t, std::string>{
            { 0, "eFill" },
            { 1, "eTopLeft" },
            { 2, "eTopRight" },
            { 3, "eBottomLeft" },
            { 4, "eBottomRight" }
        };

        (*enum_map)["ECoverPosition"] = std::map<uint32_t, std::string>{
            { 0, "COVER_POSITION_NONE" },
            { 1, "COVER_POSITION_LEFT" },
            { 2, "COVER_POSITION_RIGHT" },
            { 3, "COVER_POSITION_MIDDLE" }
        };

        (*enum_map)["EAgilityState"] = std::map<uint32_t, std::string>{
            { 0, "eAgState_JumpToHangFromLocomotion" },
            { 1, "eAgState_ClimbOnToLedgeFromLocomotion" },
            { 2, "eAgState_ClimbToHangFromLocomotion" },
            { 3, "eAgState_WalkOnFromLocomotion" },
            { 4, "eAgState_WalkOffToLocomoton" },
            { 5, "eAgState_PullUpToLedgeWalk" },
            { 6, "eAgState_PullUpToLocomotion" },
            { 7, "eAgState_LedgeJumpOffToLocomotion" },
            { 8, "eAgState_VaultOverLedgeFromCover" },
            { 9, "eAgState_ClimbOnToLedgeFromCover" },
            { 10, "eAgState_LedgeHangDefault" },
            { 11, "eAgState_LedgeWalkDefault" },
            { 12, "eAgState_TransLedgeHangDownToWalk" },
            { 13, "eAgState_TransLedgeHangUpToWalk" },
            { 14, "eAgState_TransLedgeWalkDownToHang" },
            { 15, "eAgState_TransLedgeWalkUpToHang" },
            { 16, "eAgState_TransLedgeHangUpToRail" },
            { 17, "eAgState_TransLedgeHangToLocomotion" },
            { 18, "eAgState_TransLedgeWalkToLocomotion" },
            { 19, "eAgState_LedgeHangJumpLeft" },
            { 20, "eAgState_LedgeHangJumpRight" },
            { 21, "eAgState_LedgeWalkJumpLeft" },
            { 22, "eAgState_LedgeWalkJumpRight" },
            { 23, "eAgState_SneakPastWindowLeftToRight" },
            { 24, "eAgState_SneakPastWindowRightToLeft" },
            { 25, "eAgState_ClimbWindowFromLedgeToLocomotion" },
            { 26, "eAgState_ClimbWindowFromLocomotionToLedge" },
            { 27, "eAgState_ClimbWindowFromLocomotionToLocomotion" },
            { 28, "eAgState_ClimbWindowFromCoverToLocomotion" },
            { 29, "eAgState_ClimbWindowFromCoverToLedge" },
            { 30, "eAgState_ClimbOnToLedgeFromDrainPipe" },
            { 31, "eAgState_ClimbToHangFromDrainPipe" },
            { 32, "eAgState_ClimbToWalkFromDrainPipeLeft" },
            { 33, "eAgState_ClimbToWalkFromDrainPipeRight" },
            { 34, "eAgState_Deactivate" },
            { 35, "eAgState_Unknown" }
        };

        (*enum_map)["SUITestData.ETestEnum"] = std::map<uint32_t, std::string>{
            { 1, "ETestEnum_Value_One" },
            { 2, "ETestEnum_Value_Two" },
            { 3, "ETestEnum_Value_Three" }
        };

        (*enum_map)["EVRConfigCinemaMode"] = std::map<uint32_t, std::string>{
            { 0, "EVRCCM_CinemaModeEnabled60FPS" },
            { 1, "EVRCCM_CinemaModeEnabled" },
            { 2, "EVRCCM_CinemaModeDisabled" },
            { 3, "EVRCCM_KeepCurrent" }
        };

        (*enum_map)["EOutfitType"] = std::map<uint32_t, std::string>{
            { 0, "eOT_None" },
            { 1, "eOT_Suit" },
            { 2, "eOT_Guard" },
            { 3, "eOT_Worker" },
            { 4, "eOT_Waiter" },
            { 5, "eOT_LucasGrey" }
        };

        (*enum_map)["ZActorTensionEntity.ETensionCheckMode"] = std::map<uint32_t, std::string>{
            { 0, "ETCM_INSIDE_VOLUME_OR_LIST" },
            { 1, "ETCM_INSIDE_VOLUME_AND_LIST" }
        };

        (*enum_map)["EUbitusRequest"] = std::map<uint32_t, std::string>{
            { 128, "EUR_FRIEND_LIST" },
            { 64, "EUR_ACCOUNT_AUTHORIZATION_CODE" },
            { 0, "EUR_DEVICE_LANGUAGE" },
            { 192, "EUR_FREE_COMMUNICATION_AVAILABLE" },
            { 112, "EUR_NICKNAME" },
            { 48, "EUR_NETWORK_SERVICE_ACCOUNT_ID" },
            { 176, "EUR_ESHOP_INFO" },
            { 96, "EUR_NETWORK_SERVICE_ACCOUNT_TOKEN" },
            { 160, "EUR_PROFILE_LIST" },
            { 32800, "EUR_PAUSE_GAME" },
            { 80, "EUR_ACCOUNT_AUTHORIZATION_TOKEN" },
            { 16, "EUR_DEVICE_MODE" },
            { 144, "EUR_BLOCKED_USER_LIST" },
            { 208, "EUR_DLC_PURCHASE_STATUS" },
            { 32801, "EUR_RESUME_GAME" },
            { 1226, "EUR_ESHOP_CLOSED" }
        };

        (*enum_map)["ZScreenplay.EState"] = std::map<uint32_t, std::string>{
            { 0, "State_Idle" },
            { 1, "State_Running" },
            { 2, "State_Paused" },
            { 3, "State_Failed" },
            { 4, "State_Done" }
        };

        (*enum_map)["EDifficultyParameter"] = std::map<uint32_t, std::string>{
            { 0, "EDP_Invalid" },
            { 1, "EDP_AIAttentionGainMultiplier" },
            { 2, "EDP_AIAudioPerceptionAlwaysEmitFootstepSoundEvents" },
            { 3, "EDP_AICombat_ActiveCombatants_MinimumNumber" },
            { 4, "EDP_AICombat_ActiveCombatants_MinimumNumberWhenAllVIPsAreDead" },
            { 5, "EDP_AICombat_ActiveCombatants_MaximumNumber" },
            { 6, "EDP_AICombat_ActiveCombatants_MinimumAmountForMedium" },
            { 7, "EDP_AICombat_ActiveCombatants_MinimumAmountForMany" },
            { 8, "EDP_AICombat_ShootingAccuracyResetPeriod_Minimum" },
            { 9, "EDP_AICombat_ShootingAccuracyResetPeriod_Maximum_FewCombatants" },
            { 10, "EDP_AICombat_ShootingAccuracyResetPeriod_Maximum_MediumCombatants" },
            { 11, "EDP_AICombat_ShootingAccuracyResetPeriod_Maximum_ManyCombatants" },
            { 12, "EDP_AICombat_Leaders_NumberOfActiveCombatantsNeededForOneLeader" },
            { 13, "EDP_AICombat_Leaders_NumberOfActiveCombatantsNeededForTwoLeaders" },
            { 14, "EDP_AICombat_MaximumInterestingCombatLifetime" },
            { 15, "EDP_AICombat_MaximumInterestingTimeSinceLastCombat" },
            { 16, "EDP_AICombat_ShootingAccuracyInitialBuildUp_MinimumDuration" },
            { 17, "EDP_AICombat_ShootingAccuracyInitialBuildUp_MaximumDuration" },
            { 18, "EDP_AICombat_ShootingAccuracy_MinimumDistance" },
            { 19, "EDP_AICombat_FlashGrenadeThrow_GlobalCooldown" },
            { 20, "EDP_AICombat_FlashGrenadeThrow_AggressionTriggerCooldown" },
            { 21, "EDP_AICombat_FlashGrenadeThrow_TargetCampingTriggerCooldown" },
            { 22, "EDP_AICombat_FlashGrenadeThrow_LargeRelocationTriggerCooldown" },
            { 23, "EDP_AIDisguise_FollowWarningsDelay" },
            { 24, "EDP_AIDisguise_BreakOnVeryCloseDistance" },
            { 25, "EDP_AIEscortOut_MaxForbiddenMovement" },
            { 26, "EDP_AIEscortOut_EscortEscalateDistance" },
            { 27, "EDP_AIEscortOut_MaxTimeToHonorWarning" },
            { 28, "EDP_AIEscortOut_EscortHardWarningDistance" },
            { 29, "EDP_AIIllegalActionAttentionModifier" },
            { 30, "EDP_AIInvestigateCautious_Disturbance_Radius" },
            { 31, "EDP_AIInvestigateCautious_Search_ClearDisturbanceBackRadius" },
            { 32, "EDP_AIInvestigateCautious_Search_ClearDisturbanceFrontRadius" },
            { 33, "EDP_AIInvestigateCautious_Search_ClearDisturbanceRespectSoftReservationThreshold" },
            { 34, "EDP_AIInvestigateCautiousAttentionToPersonEnabled" },
            { 35, "EDP_AIInvestigateDisguiseMembersFollowDistance" },
            { 36, "EDP_AILongTermMemory_DisableArrest" },
            { 37, "EDP_AIRunningAttentionModifier" },
            { 38, "EDP_AISentry_WarningDelay" },
            { 39, "EDP_AISuspiciousMovementDisguiseAttentionModifier" },
            { 40, "EDP_AISuspiciousMovementSensorEnabled" },
            { 41, "EDP_AISuspiciousLostTargetTime" },
            { 42, "EDP_AllowInstinctGlow" },
            { 43, "EDP_ArrestEscalateIllegalWeapon" },
            { 44, "EDP_ArrestEscalateMaximum" },
            { 45, "EDP_ArrestEscalateMoving" },
            { 46, "EDP_ArrestEscalateRunning" },
            { 47, "EDP_ArrestTargetLostTime" },
            { 48, "EDP_CloseCombatChainFailDamage" },
            { 49, "EDP_CloseCombatChoiceWindowLength" },
            { 50, "EDP_CloseCombatChokePressCount_Civilian" },
            { 51, "EDP_CloseCombatChokePressCount_Elite" },
            { 52, "EDP_CloseCombatChokePressCount_Guard" },
            { 53, "EDP_CloseCombatCounterEndLength" },
            { 54, "EDP_CloseCombatCounterFailDamage" },
            { 55, "EDP_CloseCombatShowSnapNeckPressCount_Civilian" },
            { 56, "EDP_CloseCombatShowSnapNeckPressCount_Elite" },
            { 57, "EDP_CloseCombatShowSnapNeckPressCount_Guard" },
            { 58, "EDP_DamageMultiplierHandgun" },
            { 59, "EDP_DamageMultiplierSlowgun" },
            { 60, "EDP_DamageMultiplierAssaultRifle" },
            { 61, "EDP_DamageMultiplierSMG" },
            { 62, "EDP_DamageMultiplierSniper" },
            { 63, "EDP_DamageMultiplierRPG" },
            { 64, "EDP_DamageMultiplierKnife" },
            { 65, "EDP_DamageMultiplierShotgun" },
            { 66, "EDP_DamageMultiplierSpotter" },
            { 67, "EDP_DamageMultiplierHandgun_NPC" },
            { 68, "EDP_DamageMultiplierSlowgun_NPC" },
            { 69, "EDP_DamageMultiplierAssaultRifle_NPC" },
            { 70, "EDP_DamageMultiplierSMG_NPC" },
            { 71, "EDP_DamageMultiplierSniper_NPC" },
            { 72, "EDP_DamageMultiplierRPG_NPC" },
            { 73, "EDP_DamageMultiplierKnife_NPC" },
            { 74, "EDP_DamageMultiplierShotgun_NPC" },
            { 75, "EDP_DamageMultiplierSpotter_NPC" },
            { 76, "EDP_DisguiseRuiningEnabled" },
            { 77, "EDP_GameMissionSaveLimit" },
            { 78, "EDP_GameOpportunitiesEnabled" },
            { 79, "EDP_HearRangeBulletImpact" },
            { 80, "EDP_HearRangeBulletImpactOnBody" },
            { 81, "EDP_HearRangeCloseCombatBreakNeck" },
            { 82, "EDP_HearRangeCloseCombatChoke" },
            { 83, "EDP_HearRangeCloseCombatFiberWire" },
            { 84, "EDP_HearRangeCloseCombatHitFromBehind" },
            { 85, "EDP_HearRangeCloseCombatL1" },
            { 86, "EDP_HearRangeCloseCombatL2" },
            { 87, "EDP_HearRangeCloseCombatL3" },
            { 88, "EDP_HearRangeEventAccident" },
            { 89, "EDP_HearRangeEventAlarm" },
            { 90, "EDP_HearRangeEventExplosion" },
            { 91, "EDP_HearRangeEventRadio" },
            { 92, "EDP_HearRangeEventStrange" },
            { 93, "EDP_HearRangeEventThrownItem" },
            { 94, "EDP_HearRangeFootstepWalkSlow" },
            { 95, "EDP_HearRangeFootstepWalk" },
            { 96, "EDP_HearRangeFootstepRun" },
            { 97, "EDP_HearRangeFootstepCrouchWalkSlow" },
            { 98, "EDP_HearRangeFootstepCrouchWalk" },
            { 99, "EDP_HearRangeFootstepCrouchRun" },
            { 100, "EDP_HearRangeFootstepVegetationLVAWalkSlow" },
            { 101, "EDP_HearRangeFootstepVegetationLVAWalk" },
            { 102, "EDP_HearRangeFootstepVegetationLVARun" },
            { 103, "EDP_HearRangeFootstepVegetationLVACrouchWalkSlow" },
            { 104, "EDP_HearRangeFootstepVegetationLVACrouchWalk" },
            { 105, "EDP_HearRangeFootstepVegetationLVACrouchRun" },
            { 106, "EDP_HearRangeGunEmpty" },
            { 107, "EDP_HearRangeGunShot" },
            { 108, "EDP_HearRangeGunSilenced" },
            { 109, "EDP_HearRangeGunSuperSilenced" },
            { 110, "EDP_HearRangeSpeakAngry" },
            { 111, "EDP_HearRangeSpeakHelp" },
            { 112, "EDP_HearRangeSpeakHelpLoud" },
            { 113, "EDP_HearRangeSpeakPain" },
            { 114, "EDP_HearRangeSpeakPainLow" },
            { 115, "EDP_HearRangeSpeakWarning" },
            { 116, "EDP_HitmanDamageReceivedMultiplier" },
            { 117, "EDP_InstinctModeBurnRate" },
            { 118, "EDP_InstinctRegenCap" },
            { 119, "EDP_InstinctRegenRate" },
            { 120, "EDP_ManhuntGroupMaxSearchTime" },
            { 121, "EDP_ManhuntGroupSearchAreaHackMaxDistance" },
            { 122, "EDP_ManhuntGroupSearchAreaRadius" },
            { 123, "EDP_MaxCoverShootActorCountMultiplier" },
            { 124, "EDP_MinAttentionToForceLookAt" },
            { 125, "EDP_MinAttentionToForceLookAtIfIncreasing" },
            { 126, "EDP_NPCDamageReceivedMultiplier" },
            { 127, "EDP_PlayerDamagePerSecondDistance" },
            { 128, "EDP_PlayerDamagePerSecondLimitFar" },
            { 129, "EDP_PlayerDamagePerSecondLimitNear" },
            { 130, "EDP_PlayerDamagePerSecondThreshold" },
            { 131, "EDP_PlayerHitPointsMax" },
            { 132, "EDP_PlayerHitPointsRegenerationDelay" },
            { 133, "EDP_PlayerHitPointsRegenerationPerSecond" },
            { 134, "EDP_SecurityCameras" },
            { 135, "EDP_SightReactionMinAttention" },
            { 136, "EDP_SightInvestigationMinAttention" },
            { 137, "EDP_ShootSpreadMax" },
            { 138, "EDP_ShootSpreadMin" },
            { 139, "EDP_ShootSpreadRange" },
            { 140, "EDP_SocialGreetingAngle" },
            { 141, "EDP_SocialGreetingCooldown" },
            { 142, "EDP_SocialGreetingCooldownAlertLevel" },
            { 143, "EDP_SocialGreetingCooldownLastSpeak" },
            { 144, "EDP_SocialGreetingCooldownPlayer" },
            { 145, "EDP_SocialGreetingCooldownSequence" },
            { 146, "EDP_SocialGreetingDistance" },
            { 147, "EDP_SocialLookAtTimeMax" },
            { 148, "EDP_SocialLookAtTimeMin" },
            { 149, "EDP_SocialSillyCooldown" },
            { 150, "EDP_SocialSillyDistance" },
            { 151, "EDP_SocialSillyTriggerTime" },
            { 152, "EDP_SocialStandingCooldown" },
            { 153, "EDP_SocialStandingDistance" },
            { 154, "EDP_SocialStandingDistanceImmediate" },
            { 155, "EDP_SocialStandingTriggerTime" },
            { 156, "EDP_SpotHitmanShootingDistance" },
            { 157, "EDP_StrictItemRules" },
            { 158, "EDP_COUNT" }
        };

        (*enum_map)["ZInputListenerButtonEntity.eInputListenerButtons"] = std::map<uint32_t, std::string>{
            { 0, "EILB_UP" },
            { 1, "EILB_LEFT" },
            { 2, "EILB_DOWN" },
            { 3, "EILB_RIGHT" },
            { 4, "EILB_AIM" },
            { 5, "EILB_TRIGGER" }
        };

        (*enum_map)["EEquipAbility"] = std::map<uint32_t, std::string>{
            { 0, "EA_EQUIP_IN_HANDS" },
            { 1, "EA_CANNOT_BE_EQUIPPED" }
        };

        (*enum_map)["EAISituationEvent"] = std::map<uint32_t, std::string>{
            { 0, "AISE_Any" },
            { 1, "AISE_ActorJoined" },
            { 2, "AISE_ActorLeft" },
            { 3, "AISE_Data" }
        };

        (*enum_map)["ZLeadEscortSituationEntity.EEscortState"] = std::map<uint32_t, std::string>{
            { 0, "EES_Idle" },
            { 1, "EES_Evaluate" },
            { 2, "EES_Escorting" },
            { 3, "EES_Intermediate" }
        };

        (*enum_map)["ZSniperCombatGroup.EGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_WaitingForActors" },
            { 1, "IGS_Move" },
            { 2, "IGS_Siege" },
            { 3, "IGS_Completed" },
            { 4, "IGS_Max" }
        };

        (*enum_map)["EVolumeTriggerEntityTypeFlags"] = std::map<uint32_t, std::string>{
            { 1, "VTET_Hitman" },
            { 2, "VTET_ActorAlive" },
            { 4, "VTET_ActorDead" },
            { 128, "VTET_ExplodeableItem" },
            { 64, "VTET_SoundSweetener" },
            { 32, "VTET_Spatial" },
            { 16, "VTET_ThrownItem" },
            { 8, "VTET_ActorPacified" },
            { 256, "VTET_Item" },
            { 512, "VTET_SpatialVisible" }
        };

        (*enum_map)["ZSetUIControlDisplayInfoEntity.EVisibilityOp"] = std::map<uint32_t, std::string>{
            { 0, "NoOp" },
            { 1, "SetTrue" },
            { 2, "SetFalse" }
        };

        (*enum_map)["EActionPromptState"] = std::map<uint32_t, std::string>{
            { 0, "eActionPromptState_Disabled" },
            { 1, "eActionPromptState_Enabled" },
            { 2, "eActionPromptState_Activated" },
            { 3, "eActionPromptState_Held" }
        };

        (*enum_map)["ZMoveToPositionBehaviorEntity.ERotationAlignment"] = std::map<uint32_t, std::string>{
            { 0, "RA_NONE" },
            { 1, "RA_LOOSE" },
            { 2, "RA_EXACT" }
        };

        (*enum_map)["ESaveType"] = std::map<uint32_t, std::string>{
            { 0, "ESaveType_AutoSave" },
            { 1, "ESaveType_QuickSave" },
            { 2, "ESaveType_SystemData" },
            { 3, "ESaveType_LocalProfile" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioFireType"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioFireType_Full_Automatic" },
            { 1, "eWBC_AudioFireType_Single" }
        };

        (*enum_map)["EButtonPressType"] = std::map<uint32_t, std::string>{
            { 0, "BUTTON_PRESS" },
            { 1, "BUTTON_HOLD" },
            { 2, "BUTTON_REPEAT" },
            { 3, "BUTTON_TAP" },
            { 4, "BUTTON_ROTATE_CCW" },
            { 5, "BUTTON_PRESS_TYPE_MAX" }
        };

        (*enum_map)["EImpactEffectsOptions"] = std::map<uint32_t, std::string>{
            { 0, "eIE_None" },
            { 8, "eIE_AmmoImpactEffect" },
            { 16, "eIE_All" },
            { 1, "eIE_BloodSplatter" },
            { 2, "eIE_Decals" },
            { 4, "eIE_MaterialImpactEffects" }
        };

        (*enum_map)["EBooleanOption"] = std::map<uint32_t, std::string>{
            { 0, "BO_Off" },
            { 1, "BO_On" },
            { 2, "BO_Default" }
        };

        (*enum_map)["EStandRepositionStrategy"] = std::map<uint32_t, std::string>{
            { 0, "SRS_InPlaceRepositioning" },
            { 1, "SRS_NoRepositioning" }
        };

        (*enum_map)["ZLookAtLogic.EAxisSelect"] = std::map<uint32_t, std::string>{
            { 0, "eX" },
            { 1, "eX_NEG" },
            { 2, "eY" },
            { 3, "eY_NEG" },
            { 4, "eZ" },
            { 5, "eZ_NEG" }
        };

        (*enum_map)["ESituationStateChangeReason"] = std::map<uint32_t, std::string>{
            { 128, "eSSCR_C_CLOSE_COMBAT" },
            { 64, "eSSCR_C_CONTROLLED" },
            { 0, "eSSCR_NONE" },
            { 16384, "eSSCR_S_DONE" },
            { 32768, "eSSCR_S_TERMINATED" },
            { 32, "eSSCR_C_DEACTIVATED" },
            { 8192, "eSSCR_S_DISABLED" },
            { 2097152, "eSSCR_PAUSED_FROM_RESUMING" },
            { 16, "eSSCR_C_DIED" },
            { 4096, "eSSCR_CL_SYSTEMIC" },
            { 1048576, "eSSCR_PAUSED_FROM_PLAYING" },
            { 256, "eSSCR_C_HERO_ACTION" },
            { 512, "eSSCR_C_PACIFIED" },
            { 1024, "eSSCR_CL_CHILD_DRAMA" },
            { 2048, "eSSCR_CL_OTHER_DRAMA" },
            { 65536, "eSSCR_S_STARTED" },
            { 131072, "eSSCR_S_RESUMING" },
            { 262144, "eSSCR_S_UNPAUSE" },
            { 524288, "eSSCR_R_RESOURCE_UNAVAILABLE" }
        };

        (*enum_map)["EMaterialRegistrationState"] = std::map<uint32_t, std::string>{
            { 0, "eMRS_None" },
            { 1, "eMRS_Pending" },
            { 2, "eMRS_Success" },
            { 3, "eMRS_Failed" }
        };

        (*enum_map)["ZRenderPostfilterParametersEntity.EDOFBlurType"] = std::map<uint32_t, std::string>{
            { 0, "eFastest" },
            { 1, "eLowRezLensBlur" },
            { 2, "eLowRezLensBlurSoft" },
            { 3, "eHighRezLensBlur" }
        };

        (*enum_map)["CrowdReactionAIEventType"] = std::map<uint32_t, std::string>{
            { 0, "CROWDAIEVENT_NONE" },
            { 1, "CROWDAIEVENT_CURIOUS" },
            { 2, "CROWDAIEVENT_ALERTED" },
            { 3, "CROWDAIEVENT_BLAME_FOR_WEAPON" },
            { 4, "CROWDAIEVENT_BLAME_FOR_KILL" }
        };

        (*enum_map)["ZHeroEscortSituationEntity.ETargetState"] = std::map<uint32_t, std::string>{
            { 0, "ETS_Unknown" },
            { 1, "ETS_NoTarget" },
            { 2, "ETS_RunningActBehavior" },
            { 3, "ETS_RunningDummyBehavior" },
            { 4, "ETS_RunningOtherBehavior" },
            { 5, "ETS_Dead" },
            { 6, "ETS_TargetIsHitman" }
        };

        (*enum_map)["ZHitmanLocomotionQuery.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "ANY" }
        };

        (*enum_map)["CrowdUtil.EDefaultStateTypes"] = std::map<uint32_t, std::string>{
            { 0, "STATE_IDLE" },
            { 1, "STATE_WALK" },
            { 2, "STATE_PENDINGWALK" },
            { 3, "STATE_DEAD" },
            { 4, "STATE_SCARED" },
            { 5, "STATE_PENDINGSCARED" },
            { 6, "STATE_POSSESSED" },
            { 7, "STATE_ALERT" },
            { 8, "STATE_PRONE" },
            { 9, "STATE_RELOCATE" },
            { 10, "STATE_PENDINGRELOCATE" },
            { 11, "NUM_DEFAULT_STATES" }
        };

        (*enum_map)["ZAIPerceptibleEntity.EReactionBehaviorMode"] = std::map<uint32_t, std::string>{
            { 0, "FullBehavior" },
            { 1, "StopOnInvestigate" },
            { 2, "StopOnReacted" }
        };

        (*enum_map)["EGameEventArgType"] = std::map<uint32_t, std::string>{
            { 0, "GE_ARGS_KILL" },
            { 1, "GE_ARGS_PRIORITYKILL" },
            { 2, "GE_ARGS_SITUATION" },
            { 3, "GE_ARGS_INVENTORY" },
            { 4, "GE_ARGS_EXPLODINGPROP" },
            { 5, "GE_ARGS_EVENT" },
            { 6, "GE_ARGS_CHECKPOINTEVENT" },
            { 7, "GE_ARGS_AI_SITUATION" }
        };

        (*enum_map)["ZCautiousSearchGroup.ESearchGroupState"] = std::map<uint32_t, std::string>{
            { 0, "SGS_Acknowledge" },
            { 1, "SGS_Approach" },
            { 2, "SGS_Approaching" },
            { 3, "SGS_Waiting" },
            { 4, "SGS_Completed" }
        };

        (*enum_map)["EVRConfigRotationType"] = std::map<uint32_t, std::string>{
            { 0, "EVRCRT_None" },
            { 1, "EVRCRT_Lock" },
            { 2, "EVRCRT_Offset" }
        };

        (*enum_map)["IRoomEntity.EReasoningGridImportance"] = std::map<uint32_t, std::string>{
            { 0, "RGI_NoGrid" },
            { 1, "RGI_Low" },
            { 2, "RGI_Normal" },
            { 3, "RGI_High" },
            { 4, "RGI_Extreme" }
        };

        (*enum_map)["EFSMInternalEvent"] = std::map<uint32_t, std::string>{
            { 0, "eFSMEvent_Completed" },
            { 1, "eFSMEvent_Failed" },
            { 2, "eFSMEvent_Choice" },
            { 3, "eFSMEvent_Max" }
        };

        (*enum_map)["ZOnlineManager.EState"] = std::map<uint32_t, std::string>{
            { 1, "eTRIGGER_FLOW" },
            { 2, "eOFFLINE" },
            { 3, "eSUSPEND" },
            { 4, "eDISCONNECTED" },
            { 5, "eBEGIN_CONNECTION_FLOW" },
            { 6, "ePREAUTHENTICATE" },
            { 7, "eFETCH_API_VERSION" },
            { 8, "eFETCHING_API_VERSION" },
            { 9, "eAPI_VERSION_RECEIVED" },
            { 10, "eFETCHING_CONFIGURATION" },
            { 11, "eCONFIGURATION_RECEIVED" },
            { 12, "eONLINE_CONSENT_CONFIRMATION" },
            { 13, "ePREAUTHENTICATION_ENTITLEMENTS_SYNCHRONIZE_INPROGRESS" },
            { 14, "ePREAUTHENTICATION_ENTITLEMENTS_SYNCHRONIZE_DONE" },
            { 15, "eAUTHENTICATING" },
            { 16, "eAUTHENTICATION_RECEIVED" },
            { 17, "eENTITLEMENTS_SYNCHRONIZE_INPROGRESS" },
            { 18, "eENTITLEMENTS_SYNCHRONIZE_DONE" },
            { 19, "eAUTHENTICATING_GAMESERVICE" },
            { 20, "eAUTHENTICATION_GAMESERVICE_RECEIVED" },
            { 21, "eFETCHING_OFFLINE_CACHE_DB_DIFF" },
            { 22, "eFETCHING_OFFLINE_CACHE_DB_DIFF_RECEIVED" },
            { 23, "eFETCHING_DYNAMIC_RESOURCES" },
            { 24, "eDYNAMIC_RESOURCES_MOUNTED" },
            { 25, "eCHECK_FOR_DLC_UPDATES" },
            { 26, "eFETCHING_PLATFORM_USERINFO" },
            { 27, "ePLATFORM_USERINFO_RECEIVED" },
            { 28, "eFETCHING_USER_CONFIGURATION" },
            { 29, "eUSER_CONFIGURATION_RECEIEVED" },
            { 30, "eFETCHING_PROFILE" },
            { 31, "ePROFILE_RECEIVED" },
            { 32, "eSYNCHRONIZING_PROFILE" },
            { 33, "ePROFILE_SYNCHRONIZED" },
            { 34, "eRETRIEVING_EVENTS" },
            { 35, "eEVENTS_RETRIEVED" },
            { 36, "eWAITING_FOR_SYNCHRONIZING_EVENTS" },
            { 37, "eCONNECTED" },
            { 38, "eCONNECT_TO_GAME_SESSION" },
            { 39, "eCONNECTING_TO_GAME_SESSION" },
            { 40, "eRETRY_SAVE_EVENTS" },
            { 41, "eRETRY_SAVE_EVENTS_INTERNAL" },
            { 42, "eREAUTHENTICATE" },
            { 43, "ePLATFORM_SESSION_RECONNECT" },
            { 44, "eSTATE_MAX" }
        };

        (*enum_map)["ZDebugHM5GameTimeMultiplierEnt.eTMLDReason"] = std::map<uint32_t, std::string>{
            { 0, "eTMLDR_HintMessages" },
            { 1, "eTMLDR_Sequence" },
            { 2, "eTMLDR_ActionKillHeadShot" },
            { 3, "eTMLDR_ActionKillCCProp" },
            { 4, "eTMLDR_ActionKillExplosion" },
            { 5, "eTMLDR_ActionKillLastEnemyInEncounter" },
            { 6, "eTMLDR_ActionKillLastEnemyInCheckPoint" },
            { 7, "eTMLDR_NOTSET" }
        };

        (*enum_map)["ECautiousDisturbanceState"] = std::map<uint32_t, std::string>{
            { 0, "eCDS_Pending" },
            { 1, "eCDS_Expired" },
            { 2, "eCDS_Handled" },
            { 3, "eCDS_GetHelpGroup" },
            { 4, "eCDS_InvestigationGroup" },
            { 5, "eCDS_InvestigateAfterExplosion" },
            { 6, "eCDS_Ignored" },
            { 7, "eCDS_NoInvestigator" },
            { 8, "eCDS_Similar" },
            { 9, "eCDS_ManHunt" },
            { 10, "eCDS_CombatSuppressGunshots" }
        };

        (*enum_map)["ZValueBool_Operation.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "NOT_ALL" },
            { 4, "ALL_EQUAL" },
            { 5, "ONE_TRUE" }
        };

        (*enum_map)["IContractObjective.SCounterData.ECounterType"] = std::map<uint32_t, std::string>{
            { 0, "eDEFAULT" },
            { 1, "ePERCENTAGE" }
        };

        (*enum_map)["ECCNodeType"] = std::map<uint32_t, std::string>{
            { 0, "eCCNodeType_Counter" },
            { 1, "eCCNodeType_ExecutePistol" },
            { 2, "eCCNodeType_React" },
            { 3, "eCCNodeType_SilentTakeDownEnd" },
            { 4, "eCCNodeType_SilentKill" },
            { 5, "eCCNodeType_SlitThroat1H" },
            { 6, "eCCNodeType_Swing1H" },
            { 7, "eCCNodeType_Bash1H" },
            { 8, "eCCNodeType_Stab1H" },
            { 9, "eCCNodeType_Syringe1H" },
            { 10, "eCCNodeType_Strangle1H" },
            { 11, "eCCNodeType_Smash1H" },
            { 12, "eCCNodeType_Swing2H" },
            { 13, "eCCNodeType_Stab2H" },
            { 14, "eCCNodeType_Bash2H" },
            { 15, "eCCNodeType_Strangle2H" },
            { 16, "eCCNodeType_Axe" },
            { 17, "eCCNodeType_Sword1H" },
            { 18, "eCCNodeType_AttackChain" },
            { 19, "eCCNodeType_AttackChainCivilian" },
            { 20, "eCCNodeType_AttackChainHeavy" },
            { 21, "eCCNodeType_FailChain" },
            { 22, "eCCNodeType_FinishChainHigh" },
            { 23, "eCCNodeType_FinishChainLow" },
            { 24, "eCCNodeType_FinishChainKill" },
            { 25, "eCCNodeType_StairsChain" },
            { 26, "eCCNodeType_CounterChain" },
            { 27, "eCCNodeType_AttackIdle" },
            { 28, "eCCNodeType_AttackPassify" },
            { 29, "eCCNodeType_FailChainPush" },
            { 30, "eCCNodeType_Count" }
        };

        (*enum_map)["ZHM5CrowdGenericEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "eFootstep" },
            { 1, "eClothRustle" },
            { 2, "ePushReaction" },
            { 3, "ePanic" },
            { 4, "eGeneric00" },
            { 5, "eGeneric01" },
            { 6, "eGeneric02" },
            { 7, "eGeneric03" }
        };

        (*enum_map)["EFontFlags"] = std::map<uint32_t, std::string>{
            { 8, "FV_FauxBold" },
            { 0, "FV_Normal" },
            { 32, "FV_NoAutoFit" },
            { 16, "FV_Original" },
            { 1, "FV_Italic" },
            { 2, "FV_Bold" },
            { 3, "FV_BoldItalic" },
            { 4, "FV_FauxItalic" },
            { 12, "FV_FauxBoldItalic" }
        };

        (*enum_map)["EDeathBehavior"] = std::map<uint32_t, std::string>{
            { 0, "eDB_IMPACT_ANIM" },
            { 1, "eDB_NO_IMPACT_ANIM" },
            { 2, "eDB_NO_VELOCITY" },
            { 3, "eDB_NO_RAGDOLL" }
        };

        (*enum_map)["CrowdMapImpl.CellFlags"] = std::map<uint32_t, std::string>{
            { 1, "CELLFLAG_RESTRICTED_A" },
            { 2, "CELLFLAG_RESTRICTED_B" },
            { 4, "CELLFLAG_RESTRICTED_C" },
            { 8, "CELLFLAG_WALKABLE" },
            { 16, "CELLFLAG_TELEPORT_IN" },
            { 32, "CELLFLAG_TELEPORT_OUT" },
            { 64, "CELLFLAG_UNUSED" },
            { 128, "CELLFLAG_PANIC_ONLY" }
        };

        (*enum_map)["ZBoneAttachEntity.EResetMode"] = std::map<uint32_t, std::string>{
            { 0, "eAtBoneOrigin" },
            { 1, "eKeepOffset" }
        };

        (*enum_map)["EAIModifiers"] = std::map<uint32_t, std::string>{
            { 32, "AIM_PreferredInvestigator" },
            { 8192, "AIM_IgnoreWeapons" },
            { 2097152, "AIM_SuppressSocialGreeting" },
            { 16, "AIM_IgnoreAgileHitman" },
            { 4096, "AIM_IgnoreTrespassing" },
            { 1048576, "AIM_BlockDeadlyThrow" },
            { 8, "AIM_IgnoreSneakyHitman" },
            { 0, "AIM_Invalid" },
            { 64, "AIM_AccidentShy" },
            { 128, "AIM_PreferredAccidentInvestigator" },
            { 16384, "AIM_IgnoreLockdown" },
            { 32768, "AIM_DisableHelpCivilian" },
            { 4194304, "AIM_NeverInvestigateAccidents" },
            { 8388608, "AIM_BlockDeath" },
            { 1, "AIM_IgnoreLowNoise" },
            { 2, "AIM_IgnoreHitmanPropToss" },
            { 4, "AIM_IgnoreAnnoyingHitman" },
            { 30, "AIM_IgnoreSillyHitman" },
            { 256, "AIM_BlockFiberWireInteraction" },
            { 512, "AIM_BlockCloseCombatInteraction" },
            { 1024, "AIM_BlockDragBodyInteraction" },
            { 1792, "AIM_BlockAllInteractions" },
            { 2048, "AIM_IgnoreDistractions" },
            { 65536, "AIM_WantsPrivacy" },
            { 131072, "AIM_ConversationHelper" },
            { 262144, "AIM_ConversationHelperFast" },
            { 524288, "AIM_OneHitpoint" },
            { 16777216, "AIM_IgnoreDeadBody" },
            { 33554432, "AIM_NeverSpectate" },
            { 67108864, "AIM_DisableDeadBodySensor" },
            { 134217728, "AIM_DeafAndBlind" }
        };

        (*enum_map)["ESoundGateType"] = std::map<uint32_t, std::string>{
            { 0, "SOUNDGATE_TYPE_ROUND" },
            { 1, "SOUNDGATE_TYPE_SHARPCORNERS" }
        };

        (*enum_map)["ZVRCameraEntity.ETurnMode"] = std::map<uint32_t, std::string>{
            { 0, "eSmooth" },
            { 1, "eSnap" },
            { 2, "eSmoothSnap" }
        };

        (*enum_map)["ZChannelKeywordCondition.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "ANY_TRUE_IF_NO_REQS" }
        };

        (*enum_map)["ZUIFlowLayoutEntity.EFlowType"] = std::map<uint32_t, std::string>{
            { 0, "E_FLOW_TYPE_HORIZONTAL" },
            { 1, "E_FLOW_TYPE_VERTICAL" }
        };

        (*enum_map)["ZTrackDollyControllerQueryEntity.ETrackPointQueryType"] = std::map<uint32_t, std::string>{
            { 0, "Closest" },
            { 1, "ClosestNext" },
            { 2, "ClosestPrevious" }
        };

        (*enum_map)["ESubcontrollerInventorySlot"] = std::map<uint32_t, std::string>{
            { 0, "eSIS_Invalid" },
            { 1, "eSIS_LeftHand" },
            { 2, "eSIS_RightHand" }
        };

        (*enum_map)["IHM5Door.EInitialState"] = std::map<uint32_t, std::string>{
            { 0, "IS_CLOSED" },
            { 1, "IS_OPEN" },
            { 2, "IS_OPEN_IN" },
            { 3, "IS_OPEN_OUT" }
        };

        (*enum_map)["EActionType"] = std::map<uint32_t, std::string>{
            { 64, "AT_CLOTHBUNDLE" },
            { 0, "AT_INVALID" },
            { 128, "AT_DRAGBODY" },
            { 16384, "AT_INTERACTION_GROUP" },
            { 32768, "AT_USE_NORMAL" },
            { 4194304, "AT_CONTAINERFLUSHBODY" },
            { 1073741824, "AT_USE_QUICK" },
            { -2147483648, "AT_ITEM_INTERACTION" },
            { 32, "AT_OUTFIT" },
            { 8192, "AT_COUP_DE_GRACE" },
            { 2097152, "AT_CANCEL" },
            { 536870912, "AT_ITEMCONTAINER" },
            { 16, "AT_REMOTE" },
            { 4096, "AT_DUMPBODY" },
            { 1048576, "AT_EXITSAFEZONE" },
            { 268435456, "AT_INTERRUPTIBLE_TAP" },
            { 8, "AT_GENERIC" },
            { 1, "AT_OPENDOOR" },
            { 2, "AT_CLOSEDOOR" },
            { 4, "AT_PICKUP" },
            { 256, "AT_RETRIEVE_ITEM_FROM_CONTAINER" },
            { 1024, "AT_HERO_MOVEMENT" },
            { 2048, "AT_SWAPITEM" },
            { 65536, "AT_STASHPOINT_PICKUP" },
            { 131072, "AT_ENTERCLOSET" },
            { 262144, "AT_EXITCLOSET" },
            { 524288, "AT_ENTERSAFEZONE" },
            { 16777216, "AT_PLACE_ITEM_IN_CONTAINER" },
            { 33554432, "AT_USE_DOWN" },
            { 67108864, "AT_USE_HOLD" },
            { 134217728, "AT_CONTEXTACTION" }
        };

        (*enum_map)["ERANDOM_DISTRIBUTION"] = std::map<uint32_t, std::string>{
            { 0, "RND_UNIFORM" },
            { 1, "RND_SAWTOOTH" }
        };

        (*enum_map)["InputControlNamesp.eHM5InputActionType"] = std::map<uint32_t, std::string>{
            { 0, "eTypeGet" },
            { 1, "eTypeHold" },
            { 2, "eTypeRepeat" },
            { 3, "eTypeTap" },
            { 4, "eTypeRelease" },
            { 5, "eTypeDownedge" },
            { 6, "eTypeFastTap" },
            { 7, "eTypeHoldDown" },
            { 8, "eTypeFireOnceHoldDown" },
            { 9, "eTypeClickHold" },
            { 10, "eTypePress" },
            { 11, "eTypeANALOG" },
            { 12, "eTypeANALOGRAW" },
            { 13, "eTypeRELATIVE" },
            { 14, "eTYPE_INVALID" }
        };

        (*enum_map)["ERatingCategory"] = std::map<uint32_t, std::string>{
            { 0, "ERatingCategory_Invalid" },
            { 1, "ERatingCategory_Flawless" },
            { 2, "ERatingCategory_Violence" },
            { 3, "ERatingCategory_Cunning" },
            { 4, "ERatingCategory_Noise" },
            { 5, "ERatingCategory_COUNT" }
        };

        (*enum_map)["EDebugSpatialInfoVerbosity"] = std::map<uint32_t, std::string>{
            { 0, "EDSIV_Overview" },
            { 1, "EDSIV_HierachicalView" },
            { 2, "EDSIV_Full" }
        };

        (*enum_map)["EPlayMode"] = std::map<uint32_t, std::string>{
            { 1, "PLAYMODE_STOPPED" },
            { 2, "PLAYMODE_PAUSED" },
            { 3, "PLAYMODE_PLAYING" }
        };

        (*enum_map)["ZHM5FaceFXSpecificEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "eNone" },
            { 1, "eGetInCombatCoverExhalation" },
            { 2, "eLeaveCombatCoverExhalation" },
            { 3, "eAgilityStrainedInhaleExhale" },
            { 4, "eAgilityGroan" },
            { 5, "eCloseCombatAttackSnarl" },
            { 6, "eCloseCombatStainedGroan" },
            { 7, "eCloseCombatSilentKillShush" }
        };

        (*enum_map)["EIKEndEffector"] = std::map<uint32_t, std::string>{
            { 0, "BEE_Pelvis" },
            { 1, "BEE_SpineOrigin" },
            { 2, "BEE_SpineEnd" },
            { 3, "BEE_Head" },
            { 4, "BEE_LeftUpperArm" },
            { 5, "BEE_RightUpperArm" },
            { 6, "BEE_LeftForeArm" },
            { 7, "BEE_RightForeArm" },
            { 8, "BEE_LeftHand" },
            { 9, "BEE_LeftHandThumb" },
            { 10, "BEE_LeftHandIndex" },
            { 11, "BEE_LeftHandMiddle" },
            { 12, "BEE_LeftHandRing" },
            { 13, "BEE_LeftHandPinky" },
            { 14, "BEE_RightHand" },
            { 15, "BEE_RightHandThumb" },
            { 16, "BEE_RightHandIndex" },
            { 17, "BEE_RightHandMiddle" },
            { 18, "BEE_RightHandRing" },
            { 19, "BEE_RightHandPinky" },
            { 20, "BEE_LeftFoot" },
            { 21, "BEE_RightFoot" },
            { 22, "BEE_LeftToe" },
            { 23, "BEE_RightToe" },
            { 24, "BEE_Count" }
        };

        (*enum_map)["EHintMessageType"] = std::map<uint32_t, std::string>{
            { 0, "EHintMessageType_Hint" },
            { 1, "EHintMessageType_InstinctHint" },
            { 2, "EHintMessageType_InstinctPrompt" },
            { 3, "EHintMessageType_Tutorial" }
        };

        (*enum_map)["EHUDMessageStatus"] = std::map<uint32_t, std::string>{
            { 0, "HUD_MESSAGE_UNUSED" },
            { 1, "HUD_MESSAGE_ATTACH" },
            { 2, "HUD_MESSAGE_FADEIN" },
            { 3, "HUD_MESSAGE_DISPLAY" },
            { 4, "HUD_MESSAGE_FADEOUT" }
        };

        (*enum_map)["ERequirementId"] = std::map<uint32_t, std::string>{
            { 0, "EREQUIREMENT_INVALID" },
            { 1, "EREQUIREMENT_TOKEN_OUTFIT_LEGACY_HERO_REQUIEMSUIT" },
            { 2, "EREQUIREMENT_FIREARMS_HERO_PISTOL_TACTICAL_015_SU_SKIN05" },
            { 3, "EREQUIREMENT_PROP_DEVICE_SONYPREORDER_WHITE_RUBBERDUCK_REMOTE_EXPLOSIVE" },
            { 4, "EREQUIREMENT_GOTY_PATIENT_ZERO" },
            { 5, "EREQUIREMENT_GOTY_TOKEN_OUTFIT_PARIS_CLOWN" },
            { 6, "EREQUIREMENT_GOTY_TOKEN_OUTFIT_HOKKAIDO_COWBOY" },
            { 7, "EREQUIREMENT_GOTY_TOKEN_OUTFIT_MARRAKESH_DARK_SNIPER" },
            { 8, "EREQUIREMENT_ANNIVERSARY_OUTFITS" },
            { 9, "EREQUIREMENT_LOCATION_NEWZEALAND" },
            { 10, "EREQUIREMENT_S2_EXECUTIVE_PACK" },
            { 11, "EREQUIREMENT_S2_COLLECTORS_PACK" },
            { 12, "EREQUIREMENT_S2_EXPANSION_VANITY_ITEM1" },
            { 13, "EREQUIREMENT_S2_EXPANSION_VANITY_ITEM2" },
            { 14, "EREQUIREMENT_PROP_CONTAINER_SUITCASE_ICA_STA_STADIA" },
            { 15, "EREQUIREMENT_PROP_DEVICE_ICA_RUBBERDUCK_REMOTE_EXPLOSIVE_STA_STADIA" },
            { 16, "EREQUIREMENT_H1_LEGACY_STANDARD" },
            { 17, "EREQUIREMENT_H1_LEGACY_EXPANSION" },
            { 18, "EREQUIREMENT_H2_LEGACY_STANDARD" },
            { 19, "EREQUIREMENT_H2_LEGACY_EXPANSION" },
            { 20, "EREQUIREMENT_H3_EXPANSION" },
            { 21, "EREQUIREMENT_LOCATION_GOLDEN" },
            { 22, "EREQUIREMENT_LOCATION_ANCESTRAL" },
            { 23, "EREQUIREMENT_LOCATION_EDGY" },
            { 24, "EREQUIREMENT_LOCATION_WET" },
            { 25, "EREQUIREMENT_LOCATION_ELEGANT" },
            { 26, "EREQUIREMENT_LOCATION_TRAPPED" },
            { 27, "EREQUIREMENT_PLATFORM_ORBIS" },
            { 28, "EREQUIREMENT_PLATFORM_PS5" },
            { 29, "EREQUIREMENT_PLATFORM_GDK" },
            { 30, "EREQUIREMENT_PLATFORM_EPIC" },
            { 31, "EREQUIREMENT_PLATFORM_STEAM" },
            { 32, "EREQUIREMENT_PLATFORM_GGP" },
            { 33, "EREQUIREMENT_H3_PREORDER" },
            { 34, "EREQUIREMENT_PLATFORM_IZUMO" }
        };

        (*enum_map)["ZSecuritySystemCameraConfiguration.ECameraEscalationSituations"] = std::map<uint32_t, std::string>{
            { 0, "eCES_Arrest" },
            { 1, "eCES_Combat" }
        };

        (*enum_map)["EConstraintType"] = std::map<uint32_t, std::string>{
            { 0, "ECONSTRAINTTYPE_UNKNOWN" },
            { 1, "ECONSTRAINTTYPE_BALL_AND_SOCKET" },
            { 2, "ECONSTRAINTTYPE_HINGE" },
            { 3, "ECONSTRAINTTYPE_FIXED" },
            { 4, "ECONSTRAINTTYPE_DISTANCE" },
            { 5, "ECONSTRAINTTYPE_D6" }
        };

        (*enum_map)["EVRIKElementMode"] = std::map<uint32_t, std::string>{
            { 0, "EVRIKEM_Enabled" },
            { 1, "EVRIKEM_Disabled" },
            { 2, "EVRIKEM_KeepCurrent" }
        };

        (*enum_map)["EObjectiveType"] = std::map<uint32_t, std::string>{
            { 0, "OBJECTIVE_PRIMARY" },
            { 1, "OBJECTIVE_SECONDARY" },
            { 2, "OBJECTIVE_TERTIARY" }
        };

        (*enum_map)["EWeaponUpgradeUse"] = std::map<uint32_t, std::string>{
            { 0, "eWUU_PrimaryOnly" },
            { 1, "eWUU_SecondaryOnly" },
            { 2, "eWUU_AllFireModes" }
        };

        (*enum_map)["EActorSoundDefs"] = std::map<uint32_t, std::string>{
            { 0, "_NoSound" },
            { 399, "InCa_XplnLOS" },
            { 1, "Dth_BrkNck" },
            { 2, "Dth_Fll" },
            { 393, "Gen_PhnPckUP" },
            { 3, "Dth_GnSht" },
            { 4, "Dth_HdSht" },
            { 387, "VIP_MssgnD_Rsp" },
            { 5, "Dth_Mpct" },
            { 6, "Dth_SltThrt" },
            { 397, "InCa_FrskHeadsUpLdr" },
            { 7, "Dth_Strngl" },
            { 8, "Dth_Xplo" },
            { 407, "Exp_Cough" },
            { 9, "Dth_PrpF" },
            { 10, "Dth_Electro" },
            { 401, "InDedBdy_CivCmntPhone" },
            { 11, "Dth_Burn" },
            { 12, "Dth_Crush" },
            { 395, "Cmbt_FlushOutLdr" },
            { 13, "Dth_Scrm" },
            { 14, "Dth_Hrt" },
            { 405, "Exp_Carry" },
            { 15, "Dth_SrpsGrab" },
            { 16, "Dth_HumShldStrain" },
            { 415, "Exp_Think" },
            { 17, "Dth_Snore" },
            { 18, "Dth_Groan" },
            { 409, "Exp_Exhale" },
            { 19, "Dth_Dump" },
            { 20, "Dth_PrpTssFrntAck" },
            { 403, "InDsg_FllwWrn1Nkd" },
            { 21, "Dth_Headlock" },
            { 22, "Dth_Blinded" },
            { 413, "Exp_Sniff" },
            { 23, "Dth_BeeSting" },
            { 24, "Dth_Grab" },
            { 25, "Gen_Grt47" },
            { 26, "Gen_GrtGrd47WGun" },
            { 27, "Gen_GrtTrgt" },
            { 28, "Gen_GrtTrgtRsp" },
            { 411, "Exp_Inhale" },
            { 29, "Gen_NPC2NPCGrt" },
            { 30, "Gen_NPC2NPCRsp" },
            { 31, "Gen_GtHlp" },
            { 32, "Gen_GtHlpLd" },
            { 33, "Gen_GtHlp47Knwn" },
            { 34, "Gen_Mssng" },
            { 35, "Gen_HMHere" },
            { 36, "Gen_HMThere" },
            { 419, "Exp_InPain" },
            { 37, "Gen_SrpsLow" },
            { 38, "Gen_SrpsLowShort" },
            { 39, "Gen_Srps" },
            { 40, "Gen_SrpsLd" },
            { 41, "Gen_StndRsp" },
            { 42, "Gen_Stop" },
            { 43, "Gen_StopLd" },
            { 44, "Gen_Reveal" },
            { 45, "Gen_ThumbsUp" },
            { 46, "Gen_BrknAck" },
            { 47, "Gen_Ack" },
            { 48, "Gen_AckLd" },
            { 49, "Gen_AckNtnse" },
            { 50, "Gen_BumpAck" },
            { 51, "Gen_Curse" },
            { 52, "Gen_CurseLow" },
            { 53, "Gen_DrpGun" },
            { 54, "Gen_DrpCase" },
            { 55, "Gen_CoinCurse" },
            { 56, "Gen_TransportGreet" },
            { 391, "Gen_Avoid" },
            { 57, "Gen_Thanks" },
            { 58, "Gen_ReturnItem2Guard" },
            { 385, "VIP_MssgnB_Rsp" },
            { 59, "Gen_NoWay1" },
            { 60, "Gen_NoWay2Kidding" },
            { 61, "Gen_NoWay3Joke" },
            { 62, "Gen_NoWay44Real" },
            { 389, "Dth_Sick" },
            { 63, "Gen_NoWay5DntBeliv" },
            { 64, "Gen_NoWay6Serious" },
            { 65, "Gen_NoWay7Horrible" },
            { 66, "Gen_Way1" },
            { 67, "Gen_Way2Kidding" },
            { 68, "Gen_Way3Joke" },
            { 69, "Gen_Way44Real" },
            { 70, "Gen_Way5DntBeliv" },
            { 71, "Gen_Way6Serious" },
            { 72, "Gen_Way7Horrible" },
            { 73, "Gen_NkdRunAck" },
            { 74, "Gen_Grasp" },
            { 75, "Gen_Amused" },
            { 76, "Gen_Annoyed" },
            { 77, "Gen_BdygrdArrive" },
            { 78, "Gen_BdygrdMovOut" },
            { 79, "Gen_GiveUp" },
            { 80, "Gen_Off" },
            { 81, "Gen_On" },
            { 82, "Gen_PanicLow" },
            { 83, "Gen_Sick" },
            { 84, "Gen_SmellAck" },
            { 85, "Gen_SmrtPhnAct" },
            { 86, "Gen_PhoneAct" },
            { 87, "Gen_OutbreakInfect" },
            { 88, "Gen_OutbreakSick" },
            { 89, "Gen_OutbreakWhine" },
            { 90, "Gtag" },
            { 91, "ClsCmbt_Ack" },
            { 92, "ClsCmbt_Tnt" },
            { 93, "Cmbt_BackupCll" },
            { 94, "Cmbt_BadDsg" },
            { 95, "Cmbt_Beg" },
            { 96, "Cmbt_ClsAck" },
            { 97, "Cmbt_Fire" },
            { 98, "Cmbt_FireLdr" },
            { 99, "Cmbt_GtHit" },
            { 100, "Cmbt_HitHM" },
            { 101, "Cmbt_HMClsCmbtAck" },
            { 102, "Cmbt_HMCvr" },
            { 103, "Cmbt_HMFire" },
            { 104, "Cmbt_HMFlnk" },
            { 105, "Cmbt_HMHeadPopr" },
            { 106, "Cmbt_HMKll" },
            { 107, "Cmbt_HMKllCiv" },
            { 108, "Cmbt_HMKllName" },
            { 109, "Cmbt_HMKllPrpTss" },
            { 110, "Cmbt_HMMssTnt" },
            { 111, "Cmbt_HMShrpShtr" },
            { 112, "Cmbt_HMSpttd" },
            { 113, "Cmbt_HMVnshd" },
            { 114, "Cmbt_Hold" },
            { 115, "Cmbt_HoldLdr" },
            { 116, "Cmbt_HumShldRls1" },
            { 117, "Cmbt_HumShldRls2" },
            { 118, "Cmbt_HumShldRls3" },
            { 119, "Cmbt_HumShldRlsFem1" },
            { 120, "Cmbt_HumShldRlsFem2" },
            { 121, "Cmbt_HumShldRlsFem3" },
            { 122, "Cmbt_HumShldVctm" },
            { 123, "Cmbt_HumShldLdr" },
            { 124, "Cmbt_LngLst" },
            { 125, "Cmbt_LngLstRsp" },
            { 126, "Cmbt_LstMnStn" },
            { 127, "Cmbt_LstSght" },
            { 128, "Cmbt_LstSghtRsp" },
            { 129, "Cmbt_NdrAttck" },
            { 314, "Trspss_Wrn3" },
            { 130, "Cmbt_Relod" },
            { 131, "Cmbt_Scrm" },
            { 260, "InCa_XplnDsg" },
            { 132, "Cmbt_ThrowFlash" },
            { 133, "Cmbt_ThrowFlashMiss" },
            { 318, "Trspss_EscortRequest" },
            { 134, "Cmbt_ThrowFlashMiss2" },
            { 135, "Cmbt_ThrowFlashWin" },
            { 312, "Trspss_Wrn1" },
            { 136, "Cmbt_ThrowFlashWin2" },
            { 137, "Cmbt_TkDwnLdr" },
            { 258, "InCa_XplnX" },
            { 138, "Cmbt_VntAck" },
            { 139, "Cmbt_Whmp" },
            { 268, "InCa_XplnSeeStrngl" },
            { 140, "Cmbt_StalemateHold" },
            { 141, "Cmbt_StalemateTnt" },
            { 262, "InCa_XplnRecurSvr" },
            { 142, "Cmbt_TriggerTheAlarm" },
            { 143, "Cmbt_47Mpty" },
            { 256, "InCa_XplnSeeDth" },
            { 144, "Cmbt_47SuperSize" },
            { 145, "Evac_PrtTrgtSolo" },
            { 266, "InCa_XplnKnckDwnVctm" },
            { 146, "Evac_PrtTrgtAck" },
            { 147, "Evac_PrtTrgtAckLdr" },
            { 276, "InDedBdy_NkdAck" },
            { 148, "Evac_PrtTrgtEscrt" },
            { 149, "Evac_PrtTrgtStop" },
            { 270, "InCa_XplnHuntTargetFail" },
            { 150, "Evac_PrtTrgtStnd" },
            { 151, "Evac_PrtTrgtStndRsp" },
            { 264, "InCa_XplnAckRdo" },
            { 152, "Evac_Cornered" },
            { 153, "Evac_MovOut" },
            { 274, "InDedBdy_BloodPllAck" },
            { 154, "Evac_PathChange" },
            { 155, "Evac_PeelOff" },
            { 284, "InDedBdy_CivCmnt" },
            { 156, "Evac_LastPeelOff" },
            { 157, "Evac_ShltrArrv" },
            { 278, "InDedBdy_BllPllRpt" },
            { 158, "Evac_ShltrBad" },
            { 159, "Evac_ShltrLdr" },
            { 272, "InCa_VIPKillAck" },
            { 160, "Evac_ShltrRsp" },
            { 161, "Evac_TrgtHitRsp" },
            { 162, "AvoidXplo_Ack" },
            { 163, "AvoidXplo_Stnd" },
            { 292, "InDedBdy_WakeNkdLdr" },
            { 164, "Ar_47BadAction" },
            { 165, "Ar_47X" },
            { 166, "Ar_BadDsg" },
            { 167, "Ar_BlmeKll" },
            { 168, "Ar_BlameKnckDwn" },
            { 169, "Ar_BlameKnckDwnPT" },
            { 290, "InDedBdy_WakerWake" },
            { 170, "Ar_BlameKllPT" },
            { 171, "Ar_47BadActionPT" },
            { 300, "InDsg_Pzzl" },
            { 172, "Ar_DrgBody" },
            { 173, "Ar_FkeSrrdrTnt" },
            { 294, "Rcvr_Xpln47" },
            { 174, "Ar_HMDoor" },
            { 175, "Ar_Strangle" },
            { 288, "InDedBdy_BodyGone" },
            { 176, "Ar_Trspss" },
            { 177, "Ar_WeapWrn1" },
            { 298, "InDsg_FllwWrn2" },
            { 178, "Ar_WeapWrn2" },
            { 179, "Ar_Wrn1" },
            { 308, "InDsg_FllwWrn1BadSound" },
            { 180, "Ar_Wrn2" },
            { 181, "Ar_Wrn3" },
            { 302, "InDsg_StndDistance" },
            { 182, "Ar_VictimAck" },
            { 183, "Ar_Thief" },
            { 296, "Rcvr_XplnKnckDwn" },
            { 184, "Ar_Rsp" },
            { 185, "Sniper_Ack" },
            { 306, "InDsg_FllwWrn1BadAction" },
            { 186, "InCa_BackupCll" },
            { 187, "InCa_ChckCvr" },
            { 316, "Trspss_SrchAckLegal47" },
            { 188, "InCa_CivRptFail" },
            { 189, "InCa_CivUpset" },
            { 310, "InDsg_FllwWrn1ShadyItem" },
            { 190, "InCa_ClstTnt" },
            { 191, "InCa_HMTnt" },
            { 304, "InDsg_HdNPlnSght" },
            { 192, "InCa_Idle" },
            { 193, "InCa_NitiateHMKnwn" },
            { 378, "Sentry_Accepted" },
            { 194, "InCa_SrchLdr" },
            { 195, "InCa_Stnd" },
            { 324, "Trspss_EscortExit" },
            { 196, "InCa_StndAgtd" },
            { 197, "InCa_StndAgtdLdr" },
            { 382, "Sentry_PostCommentLdr" },
            { 198, "InCa_StndAgtdHMKnwn" },
            { 199, "InCa_StndAgtdHMKnwnLdr" },
            { 376, "Sentry_FrskRequest" },
            { 200, "InCa_StndHMKnwn" },
            { 201, "InCa_StndHMKnwnLdr" },
            { 322, "Trspss_EscortStnd" },
            { 202, "InCa_StndLdr" },
            { 203, "InCa_StndRsp" },
            { 332, "InCu_CrAlrmLdr" },
            { 204, "InCa_StndLckDwnFlsAlrm" },
            { 205, "InCa_VntTnt" },
            { 326, "InCu_Brk2Rdo" },
            { 206, "InCa_Brk2Civ" },
            { 207, "InCa_Brk2Grd" },
            { 320, "Trspss_EscortStayClose" },
            { 208, "InCa_Brk2Rdo" },
            { 209, "InCa_BrkAsk" },
            { 330, "InCu_BackupRqst" },
            { 210, "InCa_GhostAsk" },
            { 211, "InCa_TriggerTheAlarm" },
            { 340, "InCu_PrpTssSeeAck" },
            { 212, "InCa_Xpln47Thief" },
            { 213, "InCa_DstrssInv" },
            { 334, "InCu_FtStpsAck" },
            { 214, "InCa_DstrssLdr" },
            { 215, "InCa_DstrssInvLdr" },
            { 328, "InCu_Stnd" },
            { 216, "InCa_WakeAsk" },
            { 217, "InCa_47Rcall" },
            { 338, "InCu_PrpTssHearLdr" },
            { 218, "InCa_WakerStnd" },
            { 219, "InCa_ClsCmbtAck" },
            { 348, "InCu_WpnInv" },
            { 220, "InCa_SeeDthInv" },
            { 221, "InCa_SeeDthInvLdr" },
            { 342, "InCu_PrpTssSeeLdr" },
            { 222, "InCa_SeeDthLdr" },
            { 223, "InCa_XploInv" },
            { 336, "InCu_PrpTssHearAck" },
            { 224, "InCa_XploInvLdr" },
            { 225, "InCa_XploLdr" },
            { 282, "InDedBdy_Stnd" },
            { 226, "InCa_AlarmAck" },
            { 227, "InCa_GnShtInv" },
            { 356, "InCu_EscrtTrgtGreenLight" },
            { 228, "InCa_GnShtInvLdr" },
            { 229, "InCa_GnShtLdr" },
            { 286, "InDedBdy_PrmtrBrchWrn2" },
            { 230, "InCa_RecurSvrInv" },
            { 231, "InCa_RecurSvrInvLdr" },
            { 280, "InDedBdy_PcfdInv" },
            { 232, "InCa_RecurSvrInvRsp" },
            { 233, "InCa_RecurSvrLdr" },
            { 354, "InCu_CrAlrmStndStndRsp" },
            { 234, "InCa_RecurSvrRsp" },
            { 235, "InCa_LckDwnGtOutLdr" },
            { 364, "InSt_Stnd" },
            { 236, "InCa_LckDwnGtOutRsp" },
            { 237, "InCa_LckDwnWrn1" },
            { 358, "InSt_HMBz" },
            { 238, "InCa_LckDwnWrn2" },
            { 239, "InCa_LckDwnWrn3" },
            { 352, "InCu_RecurRsp" },
            { 240, "InCa_LckDwnCivCmnt" },
            { 241, "InCa_FrskAck" },
            { 362, "InSt_HMSnkng" },
            { 242, "InCa_Frsk" },
            { 243, "InCa_FrskCln" },
            { 372, "FseBx_GoFix" },
            { 244, "InCa_FrskWpn" },
            { 245, "InCa_Xpln47Wpn" },
            { 366, "InSt_HM2Cls" },
            { 246, "InCa_XplnAccdnt" },
            { 247, "InCa_XplnDedBdy" },
            { 360, "InSt_HMEntXit" },
            { 248, "InCa_XplnDedBdyMassive" },
            { 249, "InCa_XplnDrgBdy" },
            { 370, "FseBx_Fixed" },
            { 250, "InCa_XplnDstrss" },
            { 251, "InCa_XplnExplo" },
            { 380, "Sentry_47LoiterAck" },
            { 252, "InCa_XplnGhost" },
            { 253, "InCa_XplnGnsht" },
            { 374, "Sentry_DenyEntry" },
            { 254, "InCa_XplnNkdBdy" },
            { 255, "InCa_XplnPcfdBdy" },
            { 368, "InSt_AdiosRequest" },
            { 257, "InCa_XplnTrspss" },
            { 259, "InCa_XplnWpn" },
            { 261, "InCa_XplnImposter" },
            { 263, "InCa_XplnRsp" },
            { 265, "InCa_XplnKnckDwn" },
            { 267, "InCa_XplnKnckDwnGhost" },
            { 269, "InCa_XplnHuntTargetWin" },
            { 271, "InCa_VIPDownAck" },
            { 273, "Accdnt_Inv" },
            { 275, "InDedBdy_Ack" },
            { 277, "InDedBdy_Inv" },
            { 279, "InDedBdy_Massive" },
            { 281, "InDedBdy_CntnAck" },
            { 283, "InDedBdy_CircleBdy" },
            { 285, "InDedBdy_PrmtrBrchWrn1" },
            { 287, "InDedBdy_47AsGrdAck" },
            { 289, "InDedBdy_VctmRcvr" },
            { 291, "InDedBdy_WakeRsp" },
            { 293, "InDedBdy_WakeNkdRsp" },
            { 295, "Rcvr_XplnDsg" },
            { 297, "InDsg_FllwWrn1" },
            { 299, "InDsg_FllwWrn3" },
            { 301, "InDsg_Stnd" },
            { 303, "InDsg_StndHidden" },
            { 305, "InDsg_FllwWrn1Ack" },
            { 307, "InDsg_FllwWrn1Wpn" },
            { 309, "InDsg_FllwWrnJoinr" },
            { 311, "Trspss_Stnd" },
            { 313, "Trspss_Wrn2" },
            { 315, "Trspss_Rsp" },
            { 317, "Trspss_EscortAck" },
            { 319, "Trspss_EscortRequestRepeat" },
            { 321, "Trspss_EscortOk" },
            { 323, "Trspss_EscortArrest" },
            { 325, "Trspss_EscortStayRequest" },
            { 327, "InCu_CivCmnd" },
            { 329, "InCu_CivRsp" },
            { 331, "InCu_CrAlrmAck" },
            { 333, "InCu_CrAlrmStndRsp" },
            { 335, "InCu_FtStpsStnd" },
            { 337, "InCu_PrpTssHearInv" },
            { 339, "InCu_PrpTssHearStndRsp" },
            { 341, "InCu_PrpTssSeeInv" },
            { 343, "InCu_PrpTssSeeStndRsp" },
            { 344, "InCu_RdoAck" },
            { 345, "InCu_RdoInv" },
            { 346, "InCu_RdoLdr" },
            { 347, "InCu_RdoStndRsp" },
            { 349, "InCu_RecurAck" },
            { 350, "InCu_RecurInv" },
            { 351, "InCu_RecurLdr" },
            { 353, "InCu_ItemAckLdr" },
            { 355, "InCu_EscrtTrgtRedLight" },
            { 357, "InSt_HMAglty" },
            { 359, "InSt_HMBzStnd" },
            { 361, "InSt_HMInCvr" },
            { 363, "InSt_PrpTssSee" },
            { 365, "InSt_Wrn" },
            { 367, "InSt_SickAck" },
            { 369, "InSt_PQ" },
            { 371, "FseBx_Fixing" },
            { 373, "FseBx_SabAck" },
            { 375, "Sentry_Frsk" },
            { 377, "Sentry_ItemRequest" },
            { 379, "Sentry_FrskWpnAck" },
            { 381, "Sentry_DenyDsg" },
            { 383, "Sentry_PostCommentRsp" },
            { 384, "VIP_MssgnA_Ldr" },
            { 386, "VIP_MssgnC_Ldr" },
            { 388, "VIP_MssngCallOut" },
            { 390, "Dth_Poison" },
            { 392, "Gen_CloseCall" },
            { 394, "Gen_PhoneActLockdown" },
            { 396, "Cmbt_HMPrptssKnckOut" },
            { 398, "InCa_FrskHeadsUpRdo" },
            { 400, "InCa_XplnGotShot" },
            { 402, "InDedBdy_NoFaulVctmXpln" },
            { 404, "Ar_BlameKnckDwnMelee" },
            { 406, "Exp_ClearThroat" },
            { 408, "Exp_Drink" },
            { 410, "Exp_Idle" },
            { 412, "Exp_InhaleFast" },
            { 414, "Exp_Swallow" },
            { 416, "Exp_Scared" },
            { 417, "Exp_Gld" },
            { 418, "Exp_Dsppntd" },
            { 420, "InCa_AckBdy" },
            { 421, "InCa_AckBdyLdr" },
            { 422, "InDedBdy_CivCmntPcfd" },
            { 423, "InDedBdy_CivCmntPhonePcfd" },
            { 424, "Gen_SocialAck" }
        };

        (*enum_map)["Network.OrderingChannel"] = std::map<uint32_t, std::string>{
            { 0, "SYSTEM_INTERNAL" },
            { 1, "PLAYER_INPUT" },
            { 2, "SYNCH_POINT" },
            { 3, "CAMERA" },
            { 4, "PROJECTILE" },
            { 5, "PIN_SIGNAL" },
            { 6, "GAME_STATE" },
            { 7, "HERO_STATE" },
            { 8, "NPC_STATE" },
            { 9, "WEAPONS" },
            { 10, "INTERACTIONS" },
            { 11, "ACTS" },
            { 12, "ANIMATION" },
            { 13, "MISC" },
            { 16, "MAX_CHANNELS" }
        };

        (*enum_map)["ZRenderPostfilterParametersEntity.EHDRAdaptationType"] = std::map<uint32_t, std::string>{
            { 0, "eMedian" },
            { 1, "eGeometricMean" }
        };

        (*enum_map)["ECheatGroup"] = std::map<uint32_t, std::string>{
            { 0, "eCGNone" },
            { 1, "eCGPistols" },
            { 2, "eCGSMGs" },
            { 3, "eCGAssaultRifles" },
            { 4, "eCGShotguns" },
            { 5, "eCGSniper" },
            { 6, "eCGExotics" },
            { 7, "eCGDevices" }
        };

        (*enum_map)["EReplicaOperation"] = std::map<uint32_t, std::string>{
            { 0, "E_Created" },
            { 1, "E_Destroyed" }
        };

        (*enum_map)["ECCEmitterEffect"] = std::map<uint32_t, std::string>{
            { 0, "eCCEmitterEffect_None" },
            { 1, "eCCEmitterEffect_Stab" },
            { 2, "eCCEmitterEffect_Blunt1H" },
            { 3, "eCCEmitterEffect_Blunt2H" },
            { 4, "eCCEmitterEffect_Slash1H" },
            { 5, "eCCEmitterEffect_Shatter" },
            { 6, "eCCEmitterEffect_AxeCleave" },
            { 7, "eCCEmitterEffect_AxePull" },
            { 8, "eCCEmitterEffect_AxeSlitThroat" },
            { 9, "eCCEmitterEffect_SlitThroat" },
            { 10, "eCCEmitterEffect_FacePunch" },
            { 11, "eCCEmitterEffect_Blood" }
        };

        (*enum_map)["ECommunicationBarState"] = std::map<uint32_t, std::string>{
            { 0, "COMMUNICATION_BAR_NONE" },
            { 1, "COMMUNICATION_BAR_CLEAR" },
            { 2, "COMMUNICATION_BAR_SEARCHING" },
            { 3, "COMMUNICATION_BAR_SUSPICIOUS" },
            { 4, "COMMUNICATION_BAR_HUNTING" },
            { 5, "COMMUNICATION_BAR_HOSTILE" },
            { 6, "COMMUNICATION_BAR_BLOOD_FOUND" },
            { 7, "COMMUNICATION_BAR_BODY_FOUND" },
            { 8, "COMMUNICATION_BAR_GUARDS_ALERTED" },
            { 9, "COMMUNICATION_BAR_BACKUP_ARRIVED" }
        };

        (*enum_map)["EButtonDisplay"] = std::map<uint32_t, std::string>{
            { 0, "BUTTON_DISPLAY_AT_OPENDOOR" },
            { 1, "BUTTON_DISPLAY_AT_PICKUP" },
            { 2, "BUTTON_DISPLAY_AT_STARTDIALOG" },
            { 3, "BUTTON_DISPLAY_AT_RESET" },
            { 4, "BUTTON_DISPLAY_AT_OUTFIT" },
            { 5, "BUTTON_DISPLAY_AT_CLOTHBUNDLE" },
            { 6, "BUTTON_DISPLAY_AT_DRAGBODY" },
            { 7, "BUTTON_DISPLAY_AT_SWITCHBOX" },
            { 8, "BUTTON_DISPLAY_AT_SWAPITEM" },
            { 9, "BUTTON_DISPLAY_AT_DUMPBODY" },
            { 10, "BUTTON_DISPLAY_AT_OPENLID" },
            { 11, "BUTTON_DISPLAY_AT_CHECKPOINTDOOR" },
            { 12, "BUTTON_DISPLAY_AT_SIMPLE" },
            { 13, "BUTTON_DISPLAY_AT_ENTERCLOSET" },
            { 14, "BUTTON_DISPLAY_AT_EXITCLOSET" },
            { 15, "BUTTON_DISPLAY_AT_ENTERSAFEZONE" },
            { 16, "BUTTON_DISPLAY_AT_EXITSAFEZONE" },
            { 17, "BUTTON_DISPLAY_AT_DISABLEFUSEBOX" },
            { 18, "BUTTON_DISPLAY_AT_CONTAINERFLUSHBODY" },
            { 19, "BUTTON_DISPLAY_AT_CONTEXTACTION" },
            { 20, "BUTTON_DISPLAY_AT_HEALTHSTATION" },
            { 21, "BUTTON_DISPLAY_AT_KEYCARDREADER" },
            { 22, "BUTTON_DISPLAY_AT_ITEMCONTAINER" },
            { 23, "BUTTON_DISPLAY_AT_USE_QUICK" },
            { 24, "BUTTON_DISPLAY_AT_USE" },
            { 25, "BUTTON_DISPLAY_RELOAD" },
            { 26, "BUTTON_DISPLAY_HOLSTER" },
            { 27, "BUTTON_DISPLAY_CHANGE_WEAPON_MODE" },
            { 28, "BUTTON_DISPLAY_REMOTE_DETONATE" },
            { 29, "BUTTON_DISPLAY_INVENTORY_ACCEPT" },
            { 30, "BUTTON_DISPLAY_INVENTORY_CANCEL" },
            { 31, "BUTTON_DISPLAY_HINTMESSAGE_CONTINUE" },
            { 32, "BUTTON_DISPLAY_CLOSECOMBAT_TAKEDOWN" },
            { 33, "BUTTON_DISPLAY_CLOSECOMBAT_COUNTERATTACK" },
            { 34, "BUTTON_DISPLAY_CLOSECOMBAT_SILENT" },
            { 35, "BUTTON_DISPLAY_CLOSECOMBAT_SNAPNECK" },
            { 36, "BUTTON_DISPLAY_CLOSECOMBAT_CHOKE" },
            { 37, "BUTTON_DISPLAY_CLOSECOMBAT_CHOKE_TARGET" },
            { 38, "BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_HIT" },
            { 39, "BUTTON_DISPLAY_PISTOL_FINISHER" },
            { 40, "BUTTON_DISPLAY_PEEK_VENT" },
            { 41, "BUTTON_DISPLAY_PEEK_CLOSET" },
            { 42, "BUTTON_DISPLAY_CLIMB_LADDER" },
            { 43, "BUTTON_DISPLAY_SLIDE_LADDER" },
            { 44, "BUTTON_DISPLAY_LEDGE" },
            { 45, "BUTTON_DISPLAY_LEDGE_STEPON" },
            { 46, "BUTTON_DISPLAY_LEDGEJUMP" },
            { 47, "BUTTON_DISPLAY_LEDGE_PULL_UP" },
            { 48, "BUTTON_DISPLAY_LEDGE_CLIMB_DOWN" },
            { 49, "BUTTON_DISPLAY_LEDGE_DROP_DOWN" },
            { 50, "BUTTON_DISPLAY_WINDOW" },
            { 51, "BUTTON_DISPLAY_WINDOWSNEAK" },
            { 52, "BUTTON_DISPLAY_PULLVICTIM" },
            { 53, "BUTTON_DISPLAY_PUSHVICTIM" },
            { 54, "BUTTON_DISPLAY_SURRENDER" },
            { 55, "BUTTON_DISPLAY_COVERTOCOVER" },
            { 56, "BUTTON_DISPLAY_COVERCORNER" },
            { 57, "BUTTON_DISPLAY_COVEREXITFORWARD" },
            { 58, "BUTTON_DISPLAY_COVERSCALEEXIT" },
            { 59, "BUTTON_DISPLAY_TAKEDISGUISE" },
            { 60, "BUTTON_DISPLAY_DROPBODY" },
            { 61, "BUTTON_DISPLAY_ABORT_CHECKPOINTDOOR" },
            { 62, "BUTTON_DISPLAY_QUICKSWAP" },
            { 63, "BUTTON_DISPLAY_OPERATE_LEVER" },
            { 64, "BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_HIGH" },
            { 65, "BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_LOW" },
            { 66, "BUTTON_DISPLAY_CLOSECOMBAT_CHAIN_COUNTER" },
            { 67, "BUTTON_DISPLAY_FIBERWIRE_KILL" },
            { 68, "BUTTON_DISPLAY_DROP_ITEM" },
            { 69, "BUTTON_DISPLAY_ABORT_MINIGAME" },
            { 70, "BUTTON_DISPLAY_CONTRACT_MARK" },
            { 71, "BUTTON_DISPLAY_CONTRACT_UNMARK" },
            { 72, "BUTTON_DISPLAY_COMPLY" },
            { 73, "BUTTON_DISPLAY_MAX" }
        };

        (*enum_map)["ZValueBool_OperationRecalc.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "NOT_ALL" },
            { 4, "ALL_EQUAL" },
            { 5, "ONE_TRUE" }
        };

        (*enum_map)["EHM5SoundBulletType"] = std::map<uint32_t, std::string>{
            { 0, "ESBT_SMALL" },
            { 1, "ESBT_MEDIUM" },
            { 2, "ESBT_LARGE" },
            { 3, "ESBT_GORGEOUS" },
            { 4, "ESBT_ANY" }
        };

        (*enum_map)["EContinuity"] = std::map<uint32_t, std::string>{
            { 0, "C0" },
            { 1, "C1" },
            { 2, "C2" }
        };

        (*enum_map)["EVRRenderingMode"] = std::map<uint32_t, std::string>{
            { 0, "VR_RENDER_MODE_2D_SCREEN" },
            { 1, "VR_RENDER_MODE_STEREO_3D" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioHeadTailType"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioHeadTailType_DryFire" },
            { 1, "eWBC_AudioHeadTailType_Normal" },
            { 2, "eWBC_AudioHeadTailType_NormalSweetener" },
            { 3, "eWBC_AudioHeadTailType_Silenced" },
            { 4, "eWBC_AudioHeadTailType_SilencedSweetener" }
        };

        (*enum_map)["ECharacterFullBodyStateType"] = std::map<uint32_t, std::string>{
            { 0, "eSM_FB_OldMovementPlaceholder" },
            { 1, "eSM_FB_Slave" },
            { 2, "eSM_FB_Locomotion" },
            { 3, "eSM_FB_CloseCombat" },
            { 4, "eSM_FB_FlavorIdle" },
            { 5, "eSM_FB_Sniping" },
            { 6, "eSM_FB_AssemblePutOnTheFloor" },
            { 7, "eSM_FB_AssembleAlignContainer" },
            { 8, "eSM_FB_AssembleRetrieve" },
            { 11, "eSM_FB_AssembleStore" }
        };

        (*enum_map)["EControllerButton"] = std::map<uint32_t, std::string>{
            { 0, "BUTTON_NONE" },
            { 1, "BUTTON_CROSS" },
            { 2, "BUTTON_TRIANGLE" },
            { 3, "BUTTON_SQUARE" },
            { 4, "BUTTON_CIRCLE" },
            { 5, "BUTTON_UP" },
            { 6, "BUTTON_RIGHT" },
            { 7, "BUTTON_DOWN" },
            { 8, "BUTTON_LEFT" },
            { 9, "BUTTON_R1" },
            { 10, "BUTTON_R2" },
            { 11, "BUTTON_R3" },
            { 12, "BUTTON_R_STICK" },
            { 13, "BUTTON_L1" },
            { 14, "BUTTON_L2" },
            { 15, "BUTTON_L3" },
            { 16, "BUTTON_L_STICK" },
            { 17, "BUTTON_START" },
            { 18, "BUTTON_SELECT" },
            { 19, "BUTTON_UP_DOWN" },
            { 20, "BUTTON_LEFT_RIGHT" },
            { 21, "BUTTON_LEFT_RIGHT_UP_DOWN" },
            { 22, "BUTTON_L1_R1" },
            { 23, "BUTTON_L2_R2" },
            { 24, "BUTTON_KEYBOARD" },
            { 25, "BUTTON_ESCAPE" }
        };

        (*enum_map)["ZCautiousBackupGroup.EGroupState"] = std::map<uint32_t, std::string>{
            { 0, "GS_DistanceField" },
            { 1, "GS_SendBackup" },
            { 2, "GS_Waiting" },
            { 3, "GS_Completed" }
        };

        (*enum_map)["ZHM5HitmanHealthModifier.EHealthDrainType"] = std::map<uint32_t, std::string>{
            { 0, "eRemoveHealthInstantly" },
            { 1, "eDecreaseHealthToValue" }
        };

        (*enum_map)["ERayType"] = std::map<uint32_t, std::string>{
            { 0, "ERAY_CLOSESTHIT_SIMPLE" },
            { 1, "ERAY_CLOSESTHIT_DETAILED" }
        };

        (*enum_map)["ZHM5BodyContainer.EBCBodyMode"] = std::map<uint32_t, std::string>{
            { 0, "BC_1_VICTIM" },
            { 1, "BC_2_VICTIMS" },
            { 2, "BC_1_VICTIM_FLUSHABLE" },
            { 3, "BC_1_VICTIM_AUTOFLUSHABLE" }
        };

        (*enum_map)["ESoundMaterialType"] = std::map<uint32_t, std::string>{
            { 0, "eGeneric" },
            { 1, "eFlesh" },
            { 2, "eSilent" }
        };

        (*enum_map)["EScreenAnchor"] = std::map<uint32_t, std::string>{
            { 0, "SCREEN_ANCHOR_TOP_LEFT" },
            { 1, "SCREEN_ANCHOR_TOP_CENTER" },
            { 2, "SCREEN_ANCHOR_TOP_RIGHT" },
            { 3, "SCREEN_ANCHOR_MIDDLE_LEFT" },
            { 4, "SCREEN_ANCHOR_MIDDLE_CENTER" },
            { 5, "SCREEN_ANCHOR_MIDDLE_RIGHT" },
            { 6, "SCREEN_ANCHOR_BOTTOM_LEFT" },
            { 7, "SCREEN_ANCHOR_BOTTOM_CENTER" },
            { 8, "SCREEN_ANCHOR_BOTTOM_RIGHT" }
        };

        (*enum_map)["eHitmanPermissionReason"] = std::map<uint32_t, std::string>{
            { 1, "eHPReason_MovementContext" },
            { 2, "eHPReason_ItemHandlingInProcess" },
            { 4, "eHPReason_SniperMode" },
            { 128, "eHPReason_FocusInteraction" },
            { 64, "eHPReason_InstinctBlendIn" },
            { 32, "eHPReason_PlayerInput" },
            { 16, "eHPReason_LevelDesign" },
            { 8, "eHPReason_CrippleBox" },
            { 7, "eHPReason_NUM" },
            { -1, "eHPReason_ALLReasons" }
        };

        (*enum_map)["EActorAnimationOrder"] = std::map<uint32_t, std::string>{
            { 0, "AAO_None" },
            { 1, "AAO_Stand" },
            { 2, "AAO_Move" },
            { 3, "AAO_React" },
            { 4, "AAO_Act" },
            { 5, "AAO_MoveToAct" },
            { 6, "AAO_MoveToCover" },
            { 7, "AAO_ShootFromCover" },
            { 8, "AAO_PeekFromCover" },
            { 9, "AAO_Death" },
            { 10, "AAO_Impact" },
            { 11, "AAO_Controlled" }
        };

        (*enum_map)["ESpeed"] = std::map<uint32_t, std::string>{
            { 0, "eSpeed_Idle" },
            { 1, "eSpeed_Slow" },
            { 2, "eSpeed_Normal" },
            { 3, "eSpeed_Fast" },
            { 4, "eSpeed_Sprint" }
        };

        (*enum_map)["ESeamFixMode"] = std::map<uint32_t, std::string>{
            { 0, "SEAMFIX_NONE" },
            { 1, "SEAMFIX_X" },
            { 2, "SEAMFIX_Y" },
            { 4, "SEAMFIX_Z" },
            { 5, "SEAMFIX_XZ" },
            { 3, "SEAMFIX_XY" },
            { 7, "SEAMFIX_XYZ" },
            { 6, "SEAMFIX_YZ" }
        };

        (*enum_map)["ESyncEvent"] = std::map<uint32_t, std::string>{
            { 0, "eSyncEvent_None" },
            { 200, "eSyncEvent_RightFoot" },
            { 100, "eSyncEvent_LeftFoot" },
            { 300, "eSyncEvent_BothFeet" }
        };

        (*enum_map)["CrowdUtil.ECrowdDirection"] = std::map<uint32_t, std::string>{
            { 0, "NORTH" },
            { 1, "EAST" },
            { 2, "SOUTH" },
            { 3, "WEST" }
        };

        (*enum_map)["ZPreferenceItemEntity.EBoolOptions"] = std::map<uint32_t, std::string>{
            { 0, "EBO_INVERTVERTICAL" },
            { 1, "EBO_FIXEDMAP" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioFamily"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioFamily_Exotics" },
            { 1, "eWBC_AudioFamily_Heavy" },
            { 2, "eWBC_AudioFamily_Light" },
            { 3, "eWBC_AudioFamily_NPC" },
            { 4, "eWBC_AudioFamily_Standard" }
        };

        (*enum_map)["ZStandOffSituation.EDialogState"] = std::map<uint32_t, std::string>{
            { 0, "eDS_ReportTarget" },
            { 1, "eDS_ArrestReason" },
            { 2, "eDS_ArrestReasonWait" },
            { 3, "eDS_Warning" },
            { 4, "eDS_WarningWait" },
            { 5, "eDS_WeaponWarning" },
            { 6, "eDS_WeaponWarningWait" },
            { 7, "eDS_StopWait" },
            { 8, "eDS_Done" }
        };

        (*enum_map)["ZCameraConeTriggerEntity.EAlignment"] = std::map<uint32_t, std::string>{
            { 0, "X_POSITIVE" },
            { 1, "Y_POSITIVE" },
            { 2, "Z_POSITIVE" },
            { 3, "X_NEGATIVE" },
            { 4, "Y_NEGATIVE" },
            { 5, "Z_NEGATIVE" }
        };

        (*enum_map)["ZHeroInventoryAction.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["IContractObjective.Category"] = std::map<uint32_t, std::string>{
            { 0, "PRIMARY" },
            { 1, "SECONDARY" },
            { 2, "CONDITION" }
        };

        (*enum_map)["ZHM5LedgeMount.ELedgeEndState"] = std::map<uint32_t, std::string>{
            { 0, "eHang" },
            { 1, "eStand" }
        };

        (*enum_map)["EHUDElement"] = std::map<uint32_t, std::string>{
            { 64, "HUD_ELEMENT_SPECIAL_BAR" },
            { 0, "HUD_ELEMENT_NONE" },
            { 128, "HUD_ELEMENT_TEXT_MESSAGES" },
            { 16384, "HUD_ELEMENT_ATTENTION_PEAKS" },
            { 32768, "HUD_ELEMENT_RATING_TRACKER" },
            { 32, "HUD_ELEMENT_WEAPON_SELECTOR" },
            { 8192, "HUD_ELEMENT_ACTION_BUTTONS" },
            { 2097152, "HUD_ELEMENT_OBJECTIVES" },
            { 16, "HUD_ELEMENT_RETICULES" },
            { 4096, "HUD_ELEMENT_CUSTOM_TEXTS" },
            { 1048576, "HUD_ELEMENT_HEALTH_BAR" },
            { 8, "HUD_ELEMENT_FOCUS_BAR" },
            { 1, "HUD_ELEMENT_MINIMAP" },
            { 2, "HUD_ELEMENT_DISGUISE" },
            { 4, "HUD_ELEMENT_WEAPON_DISPLAY" },
            { 256, "HUD_ELEMENT_CONTRACT_MARKS" },
            { 512, "HUD_ELEMENT_RATING_UPDATE" },
            { 1024, "HUD_ELEMENT_RANKING" },
            { 2048, "HUD_ELEMENT_CHALLENGES" },
            { 65536, "HUD_ELEMENT_TARGET_TRACKER" },
            { 131072, "HUD_ELEMENT_HINTS" },
            { 262144, "HUD_ELEMENT_CONTRACT_SCORING" },
            { 524288, "HUD_ELEMENT_TUTORIAL" },
            { 8388607, "HUD_ALL_ELEMENTS" }
        };

        (*enum_map)["ZHUDCamera3DControllerEntity.EMode"] = std::map<uint32_t, std::string>{
            { 0, "Auto" },
            { 1, "Mode2D" },
            { 2, "Mode3D" }
        };

        (*enum_map)["ZHUDTimerInstance.ETimeFormatChoice"] = std::map<uint32_t, std::string>{
            { 0, "YES" },
            { 1, "NO" },
            { 2, "AUTO" }
        };

        (*enum_map)["ECollidablesType"] = std::map<uint32_t, std::string>{
            { 0, "ECST_STATIC_AND_DYNAMIC" },
            { 1, "ECST_STATIC_ONLY" },
            { 2, "ECST_DYNAMIC_ONLY" }
        };

        (*enum_map)["EParticleModifierImpulseDirections"] = std::map<uint32_t, std::string>{
            { 0, "TARGET_ZAXIS_DIRECTION" },
            { 1, "TOWARDS_TARGET_PIVOT" },
            { 2, "AWAY_FROM_TARGET_PIVOT" }
        };

        (*enum_map)["EItemPoisonType"] = std::map<uint32_t, std::string>{
            { 0, "POISONTYPE_NONE" },
            { 1, "POISONTYPE_LETHAL" },
            { 2, "POISONTYPE_SEDATIVE" },
            { 3, "POISONTYPE_EMETIC" }
        };

        (*enum_map)["EScreenplayTriggerEvent"] = std::map<uint32_t, std::string>{
            { 0, "STT_ReportCrime" },
            { 1, "STT_ReportCrimeToHitman" },
            { 2, "STT_ReportCrimeMoveBack" }
        };

        (*enum_map)["ETakeDownAnim"] = std::map<uint32_t, std::string>{
            { 0, "eCoverLowOverNarrowCloseFront" },
            { 1, "eCoverLowOverNarrowCloseBack" },
            { 2, "eCoverLowOverNarrowFarFront" },
            { 3, "eCoverLowOverNarrowFarBack" },
            { 4, "eCoverLowOverWideFront" },
            { 5, "eCoverLowOverWideBack" },
            { 6, "eCoverLowCornerFront" },
            { 7, "eCoverLowCornerBack" },
            { 8, "eCoverHighCornerFront" },
            { 9, "eCoverHighCornerBack" },
            { 10, "eLocomotionBack" },
            { 11, "eLocomotionFallback" },
            { 12, "eLocomotionChair" },
            { 13, "eChokeGroundLoop" },
            { 14, "eChokeGroundEndPacify" },
            { 15, "eChokeGroundEndKill" },
            { 16, "eChokeGroundFallBackLoop" },
            { 17, "eChokeGroundFallBackEndPacify" },
            { 18, "eChokeGroundFallBackEndKill" },
            { 19, "eChokeChairLoop" },
            { 20, "eChokeChairEndPacify" },
            { 21, "eChokeChairEndKill" },
            { 22, "eChokeChairEndLeft90" },
            { 23, "eChokeChairEndLeft45" },
            { 24, "eChokeChairEndFront" },
            { 25, "eChokeChairEndRight45" },
            { 26, "eChokeChairEndRight90" },
            { 27, "eTakeDownAnimLast" }
        };

        (*enum_map)["eVRFadeType"] = std::map<uint32_t, std::string>{
            { 0, "FadeToGame" },
            { 1, "FadeToBlack" },
            { 2, "FadeToGame_HighPriority" },
            { 3, "FadeToBlack_HighPriority" },
            { 4, "CutToGame" },
            { 5, "CutToBlack" },
            { 6, "KeepCurrent" }
        };

        (*enum_map)["ZLogicMultipleGate.EGateType"] = std::map<uint32_t, std::string>{
            { 0, "eFireEveryTime" },
            { 1, "eFirePortOnce" },
            { 2, "eFireOnce" }
        };

        (*enum_map)["ZActBehaviorEntity.EMovementType"] = std::map<uint32_t, std::string>{
            { 0, "MT_WALK" },
            { 1, "MT_SNAP" },
            { 2, "MT_IGNORE_POSITION" }
        };

        (*enum_map)["ZPatrolBehaviorEntity.EMovementType"] = std::map<uint32_t, std::string>{
            { 0, "MT_WALK" },
            { 1, "MT_SNAP" },
            { 2, "MT_IGNORE_POSITION" }
        };

        (*enum_map)["EUIActionGroupIcon"] = std::map<uint32_t, std::string>{
            { 0, "EUIAGI_NoIcon" },
            { 1, "EUIAGI_GenericTool" },
            { 2, "EUIAGI_Crowbar" },
            { 3, "EUIAGI_Wrench" },
            { 4, "EUIAGI_Screwdriver" },
            { 5, "EUIAGI_ExplosiveDevice" },
            { 6, "EUIAGI_Poison" },
            { 7, "EUIAGI_Coin" },
            { 8, "EUIAGI_Lockpick" },
            { 9, "EUIAGI_LockpickElectic" },
            { 10, "EUIAGI_Card" },
            { 11, "EUIAGI_Key" },
            { 12, "EUIAGI_Password" },
            { 13, "EUIAGI_SAPIENZA_GolfBall" },
            { 14, "EUIAGI_SAPIENZA_ChemicalTube" },
            { 15, "EUIAGI_SAPIENZA_FlowBouquet" },
            { 16, "EUIAGI_SAPIENZA_Ammunitions" },
            { 17, "EUIAGI_RatFood" },
            { 18, "EUIAGI_Gunpowder" },
            { 19, "EUIAGI_Cannonball" },
            { 20, "EUIAGI_FuguFish" },
            { 21, "EUIAGI_CakeTopper" },
            { 22, "EUIAGI_Knife" },
            { 23, "EUIAGI_SpaghettiCan" },
            { 24, "EUIAGI_MAX" }
        };

        (*enum_map)["ESilenceRating"] = std::map<uint32_t, std::string>{
            { 0, "eSR_NotSilenced" },
            { 1, "eSR_Silenced" },
            { 2, "eSR_SuperSilenced" },
            { 3, "eSR_CompletelySilenced" }
        };

        (*enum_map)["GameLobbyState"] = std::map<uint32_t, std::string>{
            { 0, "GLS_None" },
            { 1, "GLS_InHud" },
            { 2, "GLS_CreatingLobby" },
            { 3, "GLS_JoiningLobby" },
            { 4, "GLS_ConnectToLobby" },
            { 5, "GLS_ConnectingLobby" },
            { 6, "GLS_InLobby" },
            { 7, "GLS_LeavingLobby" },
            { 8, "GLS_InGame" }
        };

        (*enum_map)["EVRConfigHeadAnchorMode"] = std::map<uint32_t, std::string>{
            { 0, "EVRCHAM_HeadBone" },
            { 1, "EVRCHAM_CapsuleBased" },
            { 2, "EVRCHAM_CapsuleGrid" },
            { 3, "EVRCHAM_LockPosition" },
            { 4, "EVRCHAM_KeepCurrent" }
        };

        (*enum_map)["EFilterMaskBit"] = std::map<uint32_t, std::string>{
            { 0, "eFilterMaskBit_ObjectCollision" },
            { 1, "eFilterMaskBit_OpaqueCollision" },
            { 2, "eFilterMaskBit_UNUSED_LAST" }
        };

        (*enum_map)["ECollisionPriority"] = std::map<uint32_t, std::string>{
            { 0, "ECOLLISIONPRIORITY_LOW" },
            { 1, "ECOLLISIONPRIORITY_NORMAL" },
            { 2, "ECOLLISIONPRIORITY_HIGH" },
            { 3, "ECOLLISIONPRIORITY_CRITICAL" }
        };

        (*enum_map)["ZActStateCondition.EState"] = std::map<uint32_t, std::string>{
            { 0, "MOVING" },
            { 1, "ENTERING" },
            { 2, "PLAYING" }
        };

        (*enum_map)["ELedgeDismountBehavior"] = std::map<uint32_t, std::string>{
            { 0, "eLDB_DontCare" },
            { 1, "eLDB_Stand" },
            { 2, "eLDB_Crouch" }
        };

        (*enum_map)["IContractObjective.Type"] = std::map<uint32_t, std::string>{
            { 0, "CONTRACT_OBJ_EVENT_BASED" },
            { 1, "CONTRACT_OBJ_SM_BASED" }
        };

        (*enum_map)["ZValueInt_Evaluation.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "EQUAL" },
            { 1, "NOT_EQUAL" },
            { 2, "LESS" },
            { 3, "LESS_OR_EQUAL" },
            { 4, "HIGHER" },
            { 5, "HIGHER_OR_EQUAL" }
        };

        (*enum_map)["IScatterContainerEntity.ECanvasSize"] = std::map<uint32_t, std::string>{
            { 0, "SIZE_128" },
            { 1, "SIZE_256" },
            { 2, "SIZE_512" },
            { 3, "SIZE_1024" },
            { 4, "SIZE_2048" },
            { 5, "SIZE_4096" }
        };

        (*enum_map)["ZMoveToOrder.EMoveSpeed"] = std::map<uint32_t, std::string>{
            { 0, "MS_AUTO" },
            { 1, "MS_SPRINT" },
            { 2, "MS_JOG" },
            { 3, "MS_WALK" }
        };

        (*enum_map)["ECCDecalEffect"] = std::map<uint32_t, std::string>{
            { 0, "eCCDecalEffect_None" },
            { 1, "eCCDecalEffect_Stab" },
            { 2, "eCCDecalEffect_Blunt1H" },
            { 3, "eCCDecalEffect_Blunt2H" },
            { 4, "eCCDecalEffect_Slash1H" },
            { 5, "eCCDecalEffect_Shatter" },
            { 6, "eCCDecalEffect_AxeCleave" },
            { 7, "eCCDecalEffect_AxePull" },
            { 8, "eCCDecalEffect_AxeSlitThroat" },
            { 9, "eCCDecalEffect_SlitThroat" },
            { 10, "eCCDecalEffect_FacePunch" },
            { 11, "eCCDecalEffect_Bruise" }
        };

        (*enum_map)["ZContractObjectiveHudHintEntity.EObjectiveHintIcon"] = std::map<uint32_t, std::string>{
            { 8, "eExclamationMark" },
            { 0, "eNone" },
            { 1, "eSuitcase" },
            { 3, "eObjective_A" },
            { 4, "eObjective_B" },
            { 5, "eObjective_C" },
            { 6, "eObjective_D" },
            { 7, "eInfoIcon" },
            { 9, "eIntelIcon" },
            { 10, "eDiscoveryIcon" },
            { 11, "eDiscoveryIcon_A" },
            { 12, "eDiscoveryIcon_B" },
            { 13, "eDiscoveryIcon_C" },
            { 14, "eDiscoveryIcon_A_Compleed" },
            { 15, "eDiscoveryIcon_B_Compleed" },
            { 16, "eDiscoveryIcon_C_Compleed" },
            { 17, "eMissionObjective" },
            { 18, "eExitMissionIcon" }
        };

        (*enum_map)["IScatterContainerEntity.ECanvasClearMode"] = std::map<uint32_t, std::string>{
            { 0, "CLEAR_ALL" },
            { 1, "CLEAR_INTENSITY" },
            { 2, "CLEAR_SPECIFICMATERIAL" },
            { 3, "CLEAR_ALLMATERIALS" },
            { 4, "CLEAR_SETINTENSITYVALUECLEARMATERIAL" },
            { 5, "CLEAR_SETINTENSITYVALUEKEEPMATERIAL" },
            { 6, "CLEAR_SETINTENSITYVALUEANDMATERIAL" },
            { 7, "CLEAR_SETMATERIALKEEPINTENSITYVALUE" }
        };

        (*enum_map)["ESpeechPriority"] = std::map<uint32_t, std::string>{
            { 0, "SP_AMBIENT" },
            { 1, "SP_ALERTED" },
            { 2, "SP_PAIN" },
            { 3, "SP_ALERTED_IMPORTANT" },
            { 4, "SP_COMBAT" },
            { 5, "SP_COMBAT_IMPORTANT" },
            { 6, "SP_DEATH" },
            { 7, "SP_MAX" },
            { 8, "SP_COUNT__" }
        };

        (*enum_map)["EActorRole"] = std::map<uint32_t, std::string>{
            { 0, "eActorRole_Default" },
            { 1, "eActorRole_Bodyguard" },
            { 2, "eActorRole_VIP" }
        };

        (*enum_map)["ECCWeaponAnimSet"] = std::map<uint32_t, std::string>{
            { 0, "AS_SLIT_THROAT_1H" },
            { 1, "AS_SWING_1H" },
            { 2, "AS_BASH_1H" },
            { 3, "AS_STAB_1H" },
            { 4, "AS_SYRINGE_1H" },
            { 5, "AS_SMASH_1H" },
            { 6, "AS_STRANGLE" },
            { 7, "AS_SWING_2H" },
            { 8, "AS_BASH_2H" },
            { 9, "AS_STAB_2H" },
            { 10, "AS_STRANGLE_2H" },
            { 11, "AS_SWORD_1H" },
            { 12, "AS_AXE" }
        };

        (*enum_map)["ECompiledBehaviorType"] = std::map<uint32_t, std::string>{
            { 0, "BT_ConditionScope" },
            { 1, "BT_Random" },
            { 2, "BT_Match" },
            { 3, "BT_Sequence" },
            { 4, "BT_Dummy" },
            { 5, "BT_Dummy2" },
            { 6, "BT_Error" },
            { 7, "BT_Wait" },
            { 8, "BT_WaitForStanding" },
            { 9, "BT_WaitBasedOnDistanceToTarget" },
            { 10, "BT_WaitForItemHandled" },
            { 11, "BT_AbandonOrder" },
            { 12, "BT_CompleteOrder" },
            { 13, "BT_PlayAct" },
            { 14, "BT_ConfiguredAct" },
            { 15, "BT_PlayReaction" },
            { 16, "BT_SimpleReaction" },
            { 17, "BT_SituationAct" },
            { 18, "BT_SituationApproach" },
            { 19, "BT_SituationGetHelp" },
            { 20, "BT_SituationFace" },
            { 21, "BT_SituationConversation" },
            { 22, "BT_Holster" },
            { 23, "BT_SpeakWait" },
            { 24, "BT_SpeakWaitWithFallbackIfAlone" },
            { 25, "BT_ConfiguredSpeak" },
            { 26, "BT_ConditionedConfiguredSpeak" },
            { 27, "BT_ConditionedConfiguredAct" },
            { 28, "BT_SpeakCustomOrDefaultDistractionAckSoundDef" },
            { 29, "BT_SpeakCustomOrDefaultDistractionInvestigationSoundDef" },
            { 30, "BT_SpeakCustomOrDefaultDistractionStndSoundDef" },
            { 31, "BT_Pickup" },
            { 32, "BT_Drop" },
            { 33, "BT_PlayConversation" },
            { 34, "BT_PlayAnimation" },
            { 35, "BT_MoveToLocation" },
            { 36, "BT_MoveToTargetKnownPosition" },
            { 37, "BT_MoveToTargetActualPosition" },
            { 38, "BT_MoveToInteraction" },
            { 39, "BT_MoveToNPC" },
            { 40, "BT_FollowTargetKnownPosition" },
            { 41, "BT_FollowTargetActualPosition" },
            { 42, "BT_PickUpItem" },
            { 43, "BT_GrabItem" },
            { 44, "BT_PutDownItem" },
            { 45, "BT_Search" },
            { 46, "BT_LimitedSearch" },
            { 47, "BT_MoveTo" },
            { 48, "BT_Reposition" },
            { 49, "BT_SituationMoveTo" },
            { 50, "BT_FormationMove" },
            { 51, "BT_SituationJumpTo" },
            { 52, "BT_AmbientWalk" },
            { 53, "BT_AmbientStand" },
            { 54, "BT_CrowdAmbientStand" },
            { 55, "BT_AmbientItemUse" },
            { 56, "BT_AmbientLook" },
            { 57, "BT_Act" },
            { 58, "BT_Patrol" },
            { 59, "BT_MoveToPosition" },
            { 60, "BT_AlertedStand" },
            { 61, "BT_AlertedDebug" },
            { 62, "BT_AttentionToPerson" },
            { 63, "BT_StunnedByFlashGrenade" },
            { 64, "BT_CuriousIdle" },
            { 65, "BT_InvestigateWeapon" },
            { 66, "BT_DeliverWeapon" },
            { 67, "BT_RecoverUnconscious" },
            { 68, "BT_GetOutfit" },
            { 69, "BT_RadioCall" },
            { 70, "BT_EscortOut" },
            { 71, "BT_StashItem" },
            { 72, "BT_CautiousSearchPosition" },
            { 73, "BT_LockdownWarning" },
            { 74, "BT_WakeUpUnconscious" },
            { 75, "BT_DeadBodyInvestigate" },
            { 76, "BT_GuardDeadBody" },
            { 77, "BT_DragDeadBody" },
            { 78, "BT_CuriousBystander" },
            { 79, "BT_DeadBodyBystander" },
            { 80, "BT_StandOffArrest" },
            { 81, "BT_StandOffReposition" },
            { 82, "BT_StandAndAim" },
            { 83, "BT_CloseCombat" },
            { 84, "BT_MoveToCloseCombat" },
            { 85, "BT_MoveAwayFromCloseCombat" },
            { 86, "BT_CoverFightSeasonTwo" },
            { 87, "BT_ShootFromPosition" },
            { 88, "BT_StandAndShoot" },
            { 89, "BT_CheckLastPosition" },
            { 90, "BT_ProtoSearchIdle" },
            { 91, "BT_ProtoApproachSearchArea" },
            { 92, "BT_ProtoSearchPosition" },
            { 93, "BT_ShootTarget" },
            { 94, "BT_TriggerAlarm" },
            { 95, "BT_MoveInCover" },
            { 96, "BT_MoveToCover" },
            { 97, "BT_HomeAttackOrigin" },
            { 98, "BT_Shoot" },
            { 99, "BT_Aim" },
            { 100, "BT_MoveToRandomNeighbourNode" },
            { 101, "BT_MoveToRandomNeighbourNodeAiming" },
            { 102, "BT_MoveToAndPlayCombatPositionAct" },
            { 103, "BT_MoveToAimingAndPlayCombatPositionAct" },
            { 104, "BT_PlayJumpyReaction" },
            { 105, "BT_JumpyInvestigation" },
            { 106, "BT_AgitatedPatrol" },
            { 107, "BT_AgitatedGuard" },
            { 108, "BT_HeroEscort" },
            { 109, "BT_Escort" },
            { 110, "BT_ControlledFormationMove" },
            { 111, "BT_EscortSearch" },
            { 112, "BT_LeadEscort" },
            { 113, "BT_LeadEscort2" },
            { 114, "BT_AimReaction" },
            { 115, "BT_FollowHitman" },
            { 116, "BT_RideTheLightning" },
            { 117, "BT_Scared" },
            { 118, "BT_Flee" },
            { 119, "BT_AgitatedBystander" },
            { 120, "BT_SentryFrisk" },
            { 121, "BT_SentryIdle" },
            { 122, "BT_SentryWarning" },
            { 123, "BT_SentryCheckItem" },
            { 124, "BT_VIPScared" },
            { 125, "BT_VIPSafeRoomTrespasser" },
            { 126, "BT_DefendVIP" },
            { 127, "BT_CautiousVIP" },
            { 128, "BT_CautiousGuardVIP" },
            { 129, "BT_InfectedConfused" },
            { 130, "BT_EnterInfected" },
            { 131, "BT_CureInfected" },
            { 132, "BT_SickActInfected" },
            { 133, "BT_Smart" },
            { 134, "BT_Controlled" },
            { 135, "BT_SpeakTest" },
            { 136, "BT_Conversation" },
            { 137, "BT_RunToHelp" },
            { 138, "BT_WaitForDialog" },
            { 139, "BT_WaitForConfiguredAct" },
            { 140, "BT_TestFlashbangGrenadeThrow" },
            { 141, "BT_BEHAVIORS_END" },
            { 142, "BT_RenewEvent" },
            { 143, "BT_ExpireEvent" },
            { 144, "BT_ExpireEvents" },
            { 145, "BT_SetEventHandled" },
            { 146, "BT_RenewSharedEvent" },
            { 147, "BT_ExpireSharedEvent" },
            { 148, "BT_ExpireAllEvents" },
            { 149, "BT_CreateOrJoinSituation" },
            { 150, "BT_JoinSituation" },
            { 151, "BT_ForceActorToJoinSituation" },
            { 152, "BT_JoinSituationWithActor" },
            { 153, "BT_LeaveSituation" },
            { 154, "BT_Escalate" },
            { 155, "BT_GotoPhase" },
            { 156, "BT_RenewGoal" },
            { 157, "BT_ExpireGoal" },
            { 158, "BT_RenewGoalOf" },
            { 159, "BT_ExpireGoalOf" },
            { 160, "BT_SetTension" },
            { 161, "BT_TriggerSpotted" },
            { 162, "BT_CopyKnownLocation" },
            { 163, "BT_UpdateKnownLocation" },
            { 164, "BT_TransferKnownObjectPositions" },
            { 165, "BT_WitnessAttack" },
            { 166, "BT_Speak" },
            { 167, "BT_StartDynamicEnforcer" },
            { 168, "BT_StopDynamicEnforcer" },
            { 169, "BT_StartRangeBasedDynamicEnforcer" },
            { 170, "BT_StopRangeBasedDynamicEnforcerForLocation" },
            { 171, "BT_StopRangeBasedDynamicEnforcer" },
            { 172, "BT_SetDistracted" },
            { 173, "BT_IgnoreAllDistractionsExceptTheNewest" },
            { 174, "BT_IgnoreDistractions" },
            { 175, "BT_PerceptibleEntityNotifyWillReact" },
            { 176, "BT_PerceptibleEntityNotifyReacted" },
            { 177, "BT_PerceptibleEntityNotifyInvestigating" },
            { 178, "BT_PerceptibleEntityNotifyInvestigated" },
            { 179, "BT_PerceptibleEntityNotifyTerminate" },
            { 180, "BT_LeaveDistractionAssistantRole" },
            { 181, "BT_LeaveDistractionAssitingGuardRole" },
            { 182, "BT_RequestSuitcaseAssistanceOverRadio" },
            { 183, "BT_RequestSuitcaseAssistanceFaceToFace" },
            { 184, "BT_ExpireArrestReasons" },
            { 185, "BT_SetDialogSwitch_NPCID" },
            { 186, "BT_InfectedAssignToFollowPlayer" },
            { 187, "BT_InfectedRemoveFromFollowPlayer" },
            { 188, "BT_Log" },
            { 189, "BT_COMMANDS_END" },
            { 190, "BT_Invalid" }
        };

        (*enum_map)["ZWaveformGeneratorEntity.ECurveType"] = std::map<uint32_t, std::string>{
            { 0, "CURVE_TYPE_SINE" },
            { 1, "CURVE_TYPE_SQUARE" },
            { 2, "CURVE_TYPE_TRIANGLE" },
            { 3, "CURVE_TYPE_SAWTOOTH" },
            { 4, "CURVE_TYPE_REVERSE_SAWTOOTH" }
        };

        (*enum_map)["EItemHUDType"] = std::map<uint32_t, std::string>{
            { 0, "EIHT_None" },
            { 1, "EIHT_GhostItem" },
            { 2, "EIHT_OutbreakHealingItem" }
        };

        (*enum_map)["ZDetectedInPrivateGroup.EInvestigateGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_Delay" },
            { 1, "IGS_Talk" },
            { 2, "IGS_Move" },
            { 3, "IGS_Wait" },
            { 4, "IGS_Completed" },
            { 5, "IGS_Max" }
        };

        (*enum_map)["ZActorPickerFilterKeyword.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["ZConditionListEntity.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL_TRUE" },
            { 1, "ANY_TRUE" }
        };

        (*enum_map)["ENetworkSystemID"] = std::map<uint32_t, std::string>{
            { 0, "NSID_UNDEFINED" },
            { 1, "NSID_DEBUG_SWAP_PLAYER_RPC" },
            { 2, "NSID_ROUNTTRIPTEST_REQUEST_RPC" },
            { 3, "NSID_ROUNTTRIPTEST_RESOLVE_RPC" },
            { 4, "NSID_CONNECTION_LOCALLY_DEREFERENCE_UPDATE_RPC" },
            { 5, "NSID_CONNECTION_READY_FOR_REPLICATION_RPC" },
            { 6, "NSID_NETPLAYER_LOGIN_RPC" },
            { 7, "NSID_NETPLAYER_LOGIN_COLOR_SELECTION_RPC" },
            { 8, "NSID_NETPLAYER_REPLICA" },
            { 9, "NSID_NETGAME_REPLICA" },
            { 10, "NSID_PLAYERREGISTRY_REPLICA" },
            { 11, "NSID_NETPLAYER_CONTROLLER_REPLICA" },
            { 12, "NSID_SYNCH_POINT_PLAYER_REPLICA" },
            { 13, "NSID_SYNCH_POINT_REPLICA" },
            { 14, "NSID_GAME_CHAT_RPC" },
            { 15, "NSID_NETWORK_CORE_SYSTEMS_END" },
            { 16, "NSID_PLAYER_INPUT_RPC" },
            { 17, "NSID_PLAYER_INPUT_REPLICA" },
            { 18, "NSID_GENERAL_ENTITY_PROPERTY_REPLICA" },
            { 19, "NSID_ANIMATED_ACTOR_RPC" },
            { 20, "NSID_CAMERA_REPLICA" },
            { 21, "NSID_GENERAL_PIN_SIGNAL_RPC" },
            { 22, "NSID_PIN_SIGNAL_RPC" },
            { 23, "NSID_NPC_DIALOGUE_RPC" },
            { 24, "NSID_GAME_STATE_REPLICA" },
            { 25, "NSID_HERO_STATE_REPLICA" },
            { 26, "NSID_NPC_STATE_REPLICA" },
            { 27, "NSID_ANIMATION_REPLICA" },
            { 28, "NSID_DESTRUCTIBLE_INTERACTIONS_RPC" },
            { 29, "NSID_ACT_REQUEST_RPC" },
            { 30, "NSID_CHARACTERSPEAK_REPLICA" },
            { 31, "NSID_ITEM_WEAPON_RPC" },
            { 32, "NSID_ITEM_REPLICA" },
            { 33, "NSID_ACCESSORY_ITEM_REPLICA" },
            { 34, "NSID_BULLET_IMPACT_RPC" },
            { 35, "NSID_NETWORKED_TIME_CONTROL_RPC" },
            { 36, "NSID_REQUEST_ACTION_SYCHRONIZATION_RPC" },
            { 37, "NSID_PHYSICS_SYSTEM_REPLICA" },
            { 38, "NSID_PHYSICS_OBJECT_REPLICA" },
            { 39, "NSID_WATER_SPLASH_GENERATOR_RPC" },
            { 40, "NSID_PROJECTILE_HIT_INFO_RPC" },
            { 41, "NSID_PROJECTILE_HIT_EVENT_RPC" },
            { 42, "NSID_INTERACTION_SUBACTION_EXECUTE_REQUEST" },
            { 43, "NSID_INTERACTION_SUBACTION_EXECUTE_RESOLVE" },
            { 44, "NSID_INTERACTION_SUBACTION_EXECUTE_SIGNAL_RESOLVE" },
            { 45, "NSID_INTERACTION_INPUT_REQUEST_RESOLVE" },
            { 46, "NSID_SUBCONTROLLER_REPLICA" },
            { 47, "NSID_CONTRACT_EVENT_REPLICA" },
            { 48, "NSID_NETWORK_SYNCHRONOUS_MULTIPLAYER_END" },
            { 49, "NSID_SNIPERS_CHALLENGE_SCORING_EVENT_REPLICA" },
            { 50, "NSID_CHARACTER_GHOST_REPLICA" },
            { 51, "NSID_PIP_GHOST_REPLICA" },
            { 52, "NSID_SPATIAL_GHOST_REPLICA" },
            { 53, "NSID_VS_TARGET_PICKER_REPLICA" },
            { 54, "NSID_VS_KILL_RPC" },
            { 55, "NSID_VS_KILL_CONFIRM_RPC" },
            { 56, "NSID_VS_CLIENT_DEAD_RPC" },
            { 57, "NSID_VS_SETUP_SYNC_REPLICA" },
            { 58, "NSID_VS_PIN_SIGNAL_RPC" },
            { 59, "NSID_VS_ENTITY_PROPERTY_REPLICA" },
            { 60, "NSID_VS_GENERIC_EVENT_RPC" },
            { 61, "NSID_VS_PENALTY_KILL_RPC" },
            { 62, "NSID_VS_ITEM_PICKUP_REQUEST_RPC" },
            { 63, "NSID_VS_ITEM_PICKUP_RESPONSE_RPC" },
            { 64, "NSID_VS_OUTFIT_PICK_REQUEST_RPC" },
            { 65, "NSID_VS_OUTFIT_PICK_RESPONSE_RPC" },
            { 66, "NSID_VS_PICKUP_SPAWNER_REPLICA" },
            { 67, "NSID_VS_KILL_EVENT_RPC" },
            { 68, "NSID_VS_PRE_SCORE_TIMER_RPC" },
            { 69, "NSID_VS_TARGET_PACIFY_RPC" },
            { 70, "NSID_VS_CONFIRM_TARGET_PACIFY_RPC" },
            { 71, "NSID_VS_TARGET_PACIFIED_CONFIRMED_RPC" },
            { 72, "NSID_MAX" }
        };

        (*enum_map)["ZHM5FaceFXReactionEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "eHitByBullet" },
            { 1, "eFireRegularWeapon" },
            { 2, "eReloadRegularWeapon" },
            { 3, "eFireHeavyWeapon" },
            { 4, "eReloadHeavyWeapon" },
            { 5, "eCCAttacking" },
            { 6, "eCCGettingHit" },
            { 7, "eCoverDodgebulletLeft" },
            { 8, "eCoverDodgebulletRight" },
            { 9, "eReactToExplosion" }
        };

        (*enum_map)["ZDebugSpatialEntity.EDrawLayer"] = std::map<uint32_t, std::string>{
            { 0, "DL_DEFAULT" },
            { 1, "DL_LIGHT" },
            { 2, "DL_PARTICLES" },
            { 3, "DL_PARTITIONING" },
            { 4, "DL_DECALS" },
            { 5, "DL_CROWD" },
            { 6, "DL_PHYSICS" },
            { 7, "DL_HERO" },
            { 8, "DL_AI" },
            { 9, "DL_AI_GRID" },
            { 10, "DL_AI_SITUATION" },
            { 11, "DL_NPC_LOCOMOTION" },
            { 12, "DL_GAME" },
            { 13, "DL_ALIGNMENT" },
            { 14, "DL_ENGINE" },
            { 15, "DL_SOUND" },
            { 16, "DL_ANIMATION" },
            { 17, "DL_CLOTH" },
            { 18, "DL_SOUND_PARTITIONING" },
            { 19, "DL_UI" }
        };

        (*enum_map)["EIntelTensionLevel"] = std::map<uint32_t, std::string>{
            { 0, "eITL_Undefined" },
            { 1, "eITL_Ambient" },
            { 2, "eITL_Agitated" },
            { 3, "eITL_Searching" },
            { 4, "eITL_AlertedLow" },
            { 5, "eITL_AlertedHigh" },
            { 6, "eITL_Hunting" },
            { 7, "eITL_Arrest" },
            { 8, "eITL_Combat" }
        };

        (*enum_map)["CrowdUtil.ECrowdSide"] = std::map<uint32_t, std::string>{
            { 0, "LEFT" },
            { 1, "RIGHT" }
        };

        (*enum_map)["ESentryActionPrompt"] = std::map<uint32_t, std::string>{
            { 0, "eSAP_None" },
            { 1, "eSAP_Frisk" },
            { 2, "eSAP_ShowItem" },
            { 3, "eSAP_Max" }
        };

        (*enum_map)["EInventoryConfigCustomRule"] = std::map<uint32_t, std::string>{
            { 0, "EICCR_None" },
            { 1, "EICCR_Surrender" },
            { 2, "EICCR_TempDropLeftHandItem" }
        };

        (*enum_map)["ZItemSpawner.EPhysicsMode"] = std::map<uint32_t, std::string>{
            { 0, "EPM_DEFINED_BY_ITEM" },
            { 1, "EPM_DYNAMIC" },
            { 2, "EPM_SLEEPING" },
            { 3, "EPM_KINEMATIC" }
        };

        (*enum_map)["ICameraEntity.EProjectionType"] = std::map<uint32_t, std::string>{
            { 0, "ePerspectiveRH" },
            { 1, "eOrtogonalRH" },
            { 2, "eCustom" }
        };

        (*enum_map)["EBIEventTypes"] = std::map<uint32_t, std::string>{
            { 0, "eBIL_HM_HitNPC" },
            { 1, "eBIL_HM_HitNPCKilled" },
            { 2, "eBIL_HM_HitNPCHeadShot" },
            { 3, "eBIL_HM_HitNPCCloseCombatShot" },
            { 4, "eBIL_NPC_HitHM" },
            { 5, "eBIL_Geometry" }
        };

        (*enum_map)["EInteractionInputType"] = std::map<uint32_t, std::string>{
            { 0, "EIIT_UNKNOWN" },
            { 1, "EIIT_PRESS" },
            { 2, "EIIT_HOLD" },
            { 3, "EIIT_HOLD_DOWN" },
            { 4, "EIIT_REPEAT" },
            { 5, "EIIT_GUIDE" }
        };

        (*enum_map)["ZDeadBodySituation.ESituationFlavor"] = std::map<uint32_t, std::string>{
            { 0, "SF_None" },
            { 1, "SF_Unconscious" },
            { 2, "SF_DeadBody" },
            { 3, "SF_Accident" }
        };

        (*enum_map)["ZSetVisibleEntity.StartBehavior"] = std::map<uint32_t, std::string>{
            { 0, "SB_Nothing" },
            { 1, "SB_MakeVisible" },
            { 2, "SB_MakeInvisible" }
        };

        (*enum_map)["ZBoxReflectionEntity.EBoundsType"] = std::map<uint32_t, std::string>{
            { 0, "eBox" },
            { 1, "eRoom" }
        };

        (*enum_map)["ERatingTitleRequirement"] = std::map<uint32_t, std::string>{
            { 0, "ERatingTitleRequirement_Optional" },
            { 1, "ERatingTitleRequirement_Required" },
            { 2, "ERatingTitleRequirement_Fails" }
        };

        (*enum_map)["EVRIKElement"] = std::map<uint32_t, std::string>{
            { 0, "EVRIKE_LeftHand" },
            { 1, "EVRIKE_RightHand" },
            { 2, "EVRIKE_Pelvis" },
            { 3, "EVRIKE_Spine" }
        };

        (*enum_map)["STestStruct.ETestEnum"] = std::map<uint32_t, std::string>{
            { 3, "ETomato" },
            { 5, "EPotato" }
        };

        (*enum_map)["ZHM5BodySoundEventConsumer.EAnimSoundBody"] = std::map<uint32_t, std::string>{
            { 0, "eHM5AnimSoundBody_Buttocks" },
            { 1, "eHM5AnimSoundBody_Back" },
            { 2, "eHM5AnimSoundBody_Head" },
            { 3, "eHM5AnimSoundBody_Knee_L" },
            { 4, "eHM5AnimSoundBody_Knee_R" },
            { 5, "eHM5AnimSoundBody_Elbow_L" },
            { 6, "eHM5AnimSoundBody_Elbow_R" },
            { 7, "eHM5AnimSoundBody_Hand_L" },
            { 8, "eHM5AnimSoundBody_Hand_R" },
            { 9, "eHM5AnimSoundBody_Foot_L" },
            { 10, "eHM5AnimSoundBody_Foot_R" },
            { 11, "eHM5AnimSoundBody_Shoulder_L" },
            { 12, "eHM5AnimSoundBody_Shoulder_R" },
            { 13, "eHM5AnimSoundBody_RollFwd" },
            { 14, "eHM5AnimSoundBody_RollSide" }
        };

        (*enum_map)["ECCNodeSubtype"] = std::map<uint32_t, std::string>{
            { 0, "eCCNodeSubtype_Left" },
            { 1, "eCCNodeSubtype_Right" },
            { 2, "eCCNodeSubtype_Front" },
            { 3, "eCCNodeSubtype_Back" },
            { 4, "eCCNodeSubtype_StairsAbove" },
            { 5, "eCCNodeSubtype_StairsBelow" },
            { 6, "eCCNodeSubtype_StairsAbove_Back" },
            { 7, "eCCNodeSubtype_StairsBelow_Back" },
            { 8, "eCCNodeSubtype_Front_Back" },
            { 9, "eCCNodeSubtype_Count" }
        };

        (*enum_map)["ZPIPMessageEntity.EIcon"] = std::map<uint32_t, std::string>{
            { 0, "IconStart" },
            { 1, "MagnifyingGlass" },
            { 2, "ArrowRight" },
            { 3, "ExclamationMark" },
            { 4, "Target" },
            { 5, "SurveillanceCamera" },
            { 6, "Concealed" },
            { 7, "QuestionMark" },
            { 8, "IconEnd" }
        };

        (*enum_map)["EWaypointRotationAlignment"] = std::map<uint32_t, std::string>{
            { 0, "RA_NONE" },
            { 1, "RA_LOOSE" },
            { 2, "RA_EXACT" }
        };

        (*enum_map)["EHintMessageSoundType"] = std::map<uint32_t, std::string>{
            { 0, "EHintMessageSound_None" },
            { 1, "EHintMessageSound_GeneralHint" },
            { 2, "EHintMessageSound_UsefulItem" },
            { 3, "EHintMessageSound_NeedsItem" },
            { 4, "EHintMessageSound_ObjectiveExit" },
            { 5, "EHintMessageSound_Target" },
            { 6, "EHintMessageSound_AccidentKill" },
            { 7, "EHintMessageSound_Trespassing" },
            { 8, "EHintMessageSound_CanBeTurnedOn" },
            { 9, "EHintMessageSound_CanBeTurnedOff" },
            { 10, "EHintMessageSound_Agility" }
        };

        (*enum_map)["EAsyncRaycastsGroup"] = std::map<uint32_t, std::string>{
            { 0, "eAsyncRaycasts_Gameplay" },
            { 1, "eAsyncRaycasts_ParticlesSoundCloth" },
            { 2, "eAsyncRaycasts_UNUSED_LAST" }
        };

        (*enum_map)["EKnownEntityType"] = std::map<uint32_t, std::string>{
            { 0, "KET_UNKNOWN" },
            { 1, "KET_HITMAN" },
            { 2, "KET_ACTOR" },
            { 3, "KET_ITEM" },
            { 4, "KET_LOCATION" },
            { 5, "KET_OBJECT" },
            { 6, "KET_COVER" },
            { 7, "KET_PERCEPTIBLE" },
            { 8, "KET_TEMPCROWDBODY" }
        };

        (*enum_map)["ZActorAccessoryItemCondition.EConditionTypes"] = std::map<uint32_t, std::string>{
            { 0, "ECT_HasInPool" },
            { 1, "ECT_HasAttached" }
        };

        (*enum_map)["ZHM5DynamicRayCastEntity.ECollideType"] = std::map<uint32_t, std::string>{
            { 0, "eAll" },
            { 1, "eStaticOnly" },
            { 2, "eDynamicOnly" },
            { 3, "eBaseCharacter" },
            { 4, "eHitman" },
            { 5, "eActor" }
        };

        (*enum_map)["ZCausalNodeEntity.ECauseEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["ESoundGateFlags"] = std::map<uint32_t, std::string>{
            { 1, "SOUNDGATE_FLAGS_AMBIENCE" },
            { 2, "SOUNDGATE_FLAGS_OCCLUSION" },
            { 255, "SOUNDGATE_FLAGS_ALL" }
        };

        (*enum_map)["CrowdUtil.EGenderReq"] = std::map<uint32_t, std::string>{
            { 0, "eGender_Any" },
            { 1, "eGender_Male" },
            { 2, "eGender_Female" }
        };

        (*enum_map)["EInteractionIndicatorState"] = std::map<uint32_t, std::string>{
            { 0, "EIIS_AVAILABLE" },
            { 1, "EIIS_COLLAPSED" },
            { 2, "EIIS_ACTIVATING" },
            { 3, "EIIS_NOTAVAILABLE" }
        };

        (*enum_map)["EActorLookAtPriority"] = std::map<uint32_t, std::string>{
            { 0, "eALAPriority_Ambient" },
            { 1, "eALAPriority_AmbientHigh" },
            { 2, "eALAPriority_Alert" },
            { 3, "eALAPriority_AlertHigh" },
            { 4, "eALAPriority_Hunt" },
            { 5, "eALAPriority_HuntHigh" },
            { 6, "eALAPriority_Combat" },
            { 7, "eALAPriority_CombatHigh" }
        };

        (*enum_map)["ZCameraPlaneTriggerEntity.EAlignment"] = std::map<uint32_t, std::string>{
            { 0, "XY" },
            { 1, "XZ" },
            { 2, "YZ" }
        };

        (*enum_map)["ECharacterAnimEventState"] = std::map<uint32_t, std::string>{
            { 0, "eES_None" },
            { 1, "eES_Pending" },
            { 2, "eES_Completed" },
            { 3, "eES_TimedOut" }
        };

        (*enum_map)["ZFriskSuspectGroup.EHitmanGreetState"] = std::map<uint32_t, std::string>{
            { 0, "HGS_NotGreeted" },
            { 1, "HGS_Greeted" },
            { 2, "HGS_ShrugOff" }
        };

        (*enum_map)["SDoorSoundDefs.EDefinition"] = std::map<uint32_t, std::string>{
            { 0, "DoorOpen" },
            { 1, "DoorClose" },
            { 2, "DoorSlam" },
            { 3, "DoorOpenStop" }
        };

        (*enum_map)["ZActorPickerEffectDrama.EEffectPhaseAction"] = std::map<uint32_t, std::string>{
            { 0, "EPA_START" },
            { 1, "EPA_LEAVE" }
        };

        (*enum_map)["ZAudioSequenceTrack.EPlaybackTarget"] = std::map<uint32_t, std::string>{
            { 0, "eNone" },
            { 1, "eTargetEntityAudioPlayer" },
            { 2, "eTargetEntityEmitter" },
            { 3, "ePreviewEmitter" }
        };

        (*enum_map)["EItemModifierType"] = std::map<uint32_t, std::string>{
            { 0, "MODIFIER_NONE" },
            { 1, "MODIFIER_THROW" },
            { 2, "MODIFIER_CARRY" },
            { 3, "MODIFIER_AMMO" },
            { 4, "MODIFIER_PRECISION" },
            { 5, "MODIFIER_DAMAGE" },
            { 6, "MODIFIER_IMPACT" },
            { 7, "MODIFIER_EXPLOSIVE" },
            { 8, "MODIFIER_RANGE" },
            { 9, "MODIFIER_ZOOM" },
            { 10, "MODIFIER_SUPPRESSOR" },
            { 11, "MODIFIER_RECOIL" },
            { 12, "MODIFIER_RATEOFFIRE" },
            { 13, "MODIFIER_SCOPEBOBBING" },
            { 14, "MODIFIER_MUZZLEVELOCITY" },
            { 15, "MODIFIER_KNOCKDOWN" },
            { 16, "MODIFIER_FULLAUTO" },
            { 17, "MODIFIER_PRECISIONSHOT" },
            { 18, "MODIFIER_SCOPETIMESLOWDOWN" },
            { 19, "MODIFIER_STYLE" },
            { 20, "MODIFIER_RELOAD" },
            { 21, "MODIFIER_THRESHOLD" },
            { 22, "MODIFIER_PERK" },
            { 23, "MODIFIER_MAGAZINE" },
            { 24, "MODIFIER_BURST" }
        };

        (*enum_map)["EAISituation"] = std::map<uint32_t, std::string>{
            { 0, "AIS_GENERIC" },
            { 1, "AIS_SPECIAL" },
            { 2, "AIS_SENSOR" },
            { 3, "AIS_TRESPASSING" },
            { 4, "AIS_STANDOFF" },
            { 5, "AIS_SMUGGLE" },
            { 6, "AIS_GET_HELP" },
            { 7, "AIS_CONVERSTION_GROUP" },
            { 8, "AIS_PROTO_COMBAT" },
            { 9, "AIS_SENTRY" },
            { 10, "AIS_LEAD_ESCORT" },
            { 11, "AIS_INVESTIGATE_WEAPON" },
            { 12, "AIS_INVESTIGATE_STANDING" },
            { 13, "AIS_INVESTIGATE_CURIOUS" },
            { 14, "AIS_INVESTIGATE_CAUTIOUS" },
            { 15, "AIS_HERO_ESCORT" },
            { 16, "AIS_EVACUATE" },
            { 17, "AIS_ESCORT" },
            { 18, "AIS_ENTOURAGE" },
            { 19, "AIS_DRAMA" },
            { 20, "AIS_DISGUISE" },
            { 21, "AIS_DEAD_BODY" },
            { 22, "AIS_COMBAT" },
            { 23, "AIS_CLOSE_COMBAT" },
            { 24, "AIS_AVOID_EXPLOSION" },
            { 25, "AIS_ACCIDENT" },
            { 26, "AIS_PRIVATE" },
            { 27, "AIS_GUARD_BODY" },
            { 28, "AIS_RECOVER_UNC" },
            { 29, "AIS_SNIPER" }
        };

        (*enum_map)["EScreenPlayState"] = std::map<uint32_t, std::string>{
            { 0, "State_Stopped" },
            { 1, "State_Running" },
            { 2, "State_Resuming" },
            { 3, "State_Pausing" },
            { 4, "State_Paused" },
            { 5, "State_Done" },
            { 6, "State_Terminated" }
        };

        (*enum_map)["EDramaSituationDescriptors"] = std::map<uint32_t, std::string>{
            { 64, "eDSD_ALERT" },
            { 0, "eDSD_NONE" },
            { 128, "eDSD_PARANOID" },
            { 32, "eDSD_DEATH" },
            { 16, "eDSD_SICK" },
            { 8, "eDSD_URGENT" },
            { 1, "eDSD_HERO" },
            { 2, "eDSD_CONVERSATION" },
            { 4, "eDSD_DISTRACTION" },
            { 256, "eDSD_SCARY" },
            { 512, "eDSD_TRIVIAL" },
            { 1024, "eDSD_PREOCCUPIED" },
            { 2048, "eDSD_PRIVATE" }
        };

        (*enum_map)["ERagdollPart"] = std::map<uint32_t, std::string>{
            { 0, "ERAGDOLLPART_NONE" },
            { 1, "ERAGDOLLPART_BODY" },
            { 2, "ERAGDOLLPART_HEAD" },
            { 3, "ERAGDOLLPART_HAND" },
            { 4, "ERAGDOLLPART_FOOT" }
        };

        (*enum_map)["EOpportunityState"] = std::map<uint32_t, std::string>{
            { 1, "OS_TRACKED" },
            { 2, "OS_ENABLED" },
            { 4, "OS_UNLOCKED" },
            { 8, "OS_REVEAL_PENDING" }
        };

        (*enum_map)["EOrderCompletionStatus"] = std::map<uint32_t, std::string>{
            { 0, "OCS_Undetermined" },
            { 1, "OCS_Succeeded" },
            { 2, "OCS_Interrupted" },
            { 3, "OCS_Blocked" },
            { 4, "OCS_Failed" }
        };

        (*enum_map)["ZHM5ContextActionEntity.EInteraction"] = std::map<uint32_t, std::string>{
            { 0, "eIT_RotateLeftStick" },
            { 1, "eIT_AutoComplete" },
            { 2, "eIT_NoInteraction" }
        };

        (*enum_map)["EOutbreakInfectionStage"] = std::map<uint32_t, std::string>{
            { 0, "eOIS_Stage1" },
            { 1, "eOIS_Stage2" },
            { 2, "eOIS_Stage3" }
        };

        (*enum_map)["eActionRewardType"] = std::map<uint32_t, std::string>{
            { 0, "AR_None" },
            { 1, "AR_QuestItem" },
            { 2, "AR_Keycard" },
            { 3, "AR_Key" }
        };

        (*enum_map)["ZAT2Controller.EFootstepsMovementType"] = std::map<uint32_t, std::string>{
            { 0, "MOVEMENT_WALKING" },
            { 1, "MOVEMENT_WALKING_SLIDING" },
            { 2, "MOVEMENT_RUNNING" },
            { 3, "MOVEMENT_STANDING" }
        };

        (*enum_map)["ZMoveToPositionBehaviorEntity.EMovementType"] = std::map<uint32_t, std::string>{
            { 0, "MT_WALK" },
            { 1, "MT_SNAP" },
            { 2, "MT_IGNORE_POSITION" }
        };

        (*enum_map)["ZHM5ForceCover.EFaceDirection"] = std::map<uint32_t, std::string>{
            { 0, "eFaceLeft" },
            { 1, "eFaceRight" }
        };

        (*enum_map)["AudioCurve"] = std::map<uint32_t, std::string>{
            { 0, "AudioCurve_Log3" },
            { 1, "AudioCurve_Sine" },
            { 2, "AudioCurve_Log1" },
            { 3, "AudioCurve_InvSCurve" },
            { 4, "AudioCurve_Linear" },
            { 5, "AudioCurve_SCurve" },
            { 6, "AudioCurve_Exp1" },
            { 7, "AudioCurve_SineRecip" },
            { 8, "AudioCurve_Exp3" },
            { 9, "AudioCurve_LastFadeCurve" },
            { 10, "AudioCurve_Constant" }
        };

        (*enum_map)["EActivationPriority"] = std::map<uint32_t, std::string>{
            { 0, "eActivatable_First" },
            { 1, "eActivatable_PlayModeAddedPhysics" },
            { 2, "eActivatable_DestructiblePhysics" },
            { 3, "eActivatable_PhysicsWind" },
            { 4, "eActivatable_Physics" },
            { 5, "eActivatable_PhysicsDone" },
            { 6, "eActivatable_Keywords" },
            { 7, "eActivatable_EventChannel" },
            { 8, "eActivatable_Items" },
            { 9, "eActivatable_Actor" },
            { 10, "eActivatable_Hitman" },
            { 11, "eActivatable_AnimPlayer" },
            { 12, "eActivatable_Sequence" },
            { 13, "eActivatable_SpawnPoint" },
            { 14, "eActivatable_CoverPlane" },
            { 15, "eActivatable_GuardPoint" },
            { 16, "eActivatable_Guide" },
            { 17, "eActivatable_CombatAct" },
            { 18, "eActivatable_CombatAttractor" },
            { 19, "eActivatable_SequenceController" },
            { 20, "eActivatable_KnownEntityAspects" },
            { 21, "eActivatable_AIReasoningGrid" },
            { 22, "eActivatable_HM5Zone" },
            { 23, "eActivatable_Crowds" },
            { 24, "eActivatable_ChildNetworkAct" },
            { 25, "eActivatable_Door" },
            { 26, "eActivatable_UI_Setup" },
            { 27, "eActivatable_NormalGameplay" },
            { 28, "eActivatable_NormalGameplay_Condition" },
            { 29, "eActivatable_NormalGameplay_Values" },
            { 30, "eActivatable_NormalGameplay_State" },
            { 31, "eActivatable_HeroSpawns" },
            { 32, "eActivatable_UI" },
            { 33, "eActivatable_Locomotion" },
            { 34, "eActivatable_Timers" },
            { 35, "eActivatable_AreaTriggers" },
            { 36, "eActivatable_Default" },
            { 37, "eActivatable_GameEventListener" },
            { 38, "eLAST_ACTIVATION_PRIORITY" }
        };

        (*enum_map)["ZMenuSliderNavigationEntity.ESliderInputMode"] = std::map<uint32_t, std::string>{
            { 0, "E_HORIZONTAL" },
            { 1, "E_VERTICAL" }
        };

        (*enum_map)["ZActDurationCondition.EState"] = std::map<uint32_t, std::string>{
            { 0, "MOVING" },
            { 1, "ENTERING" },
            { 2, "PLAYING" },
            { 3, "STOPPING" }
        };

        (*enum_map)["ETextAlignment"] = std::map<uint32_t, std::string>{
            { 0, "TEXT_ALIGN_LEFT" },
            { 1, "TEXT_ALIGN_CENTER" },
            { 2, "TEXT_ALIGN_RIGHT" }
        };

        (*enum_map)["eItemSize"] = std::map<uint32_t, std::string>{
            { 0, "ITEMSIZE_SMALL" },
            { 1, "ITEMSIZE_MEDIUM" },
            { 2, "ITEMSIZE_LARGE" }
        };

        (*enum_map)["ZEscortSituation2Entity.EEscortState"] = std::map<uint32_t, std::string>{
            { 0, "EES_Idle" },
            { 1, "EES_Evaluate" },
            { 2, "EES_Escorting" },
            { 3, "EES_Searching" },
            { 4, "EES_Intermediate" }
        };

        (*enum_map)["ZLeadEscortSituationEntity.ETargetState"] = std::map<uint32_t, std::string>{
            { 0, "ETS_Unknown" },
            { 1, "ETS_NoTarget" },
            { 2, "ETS_RunningActBehavior" },
            { 3, "ETS_RunningDummyBehavior" },
            { 4, "ETS_RunningOtherBehavior" },
            { 5, "ETS_Dead" },
            { 6, "ETS_TargetIsHitman" }
        };

        (*enum_map)["ECharacterSubcontrollerType"] = std::map<uint32_t, std::string>{
            { 0, "eSCT_IDGenerator" },
            { 1, "eSCT_Debugger" },
            { 2, "eSCT_Time" },
            { 3, "eSCT_Tags" },
            { 4, "eSCT_Descriptors" },
            { 5, "eSCT_ActionRequest" },
            { 6, "eSCT_Events" },
            { 7, "eSCT_Animator" },
            { 8, "eSCT_Motor" },
            { 9, "eSCT_Inventory" },
            { 10, "eSCT_FlavorIdle" },
            { 11, "eSCT_Lookat" },
            { 12, "eSCT_AimAssistant" },
            { 13, "eSCT_CharacterCollector" },
            { 14, "eSCT_SwipeMechanic" },
            { 15, "eSCT_OpenDoor" },
            { 16, "eSCT_Fullbody" },
            { 17, "eSCT_Aim" },
            { 18, "eSCT_PrecisionShot" },
            { 19, "eSCT_Marking" },
            { 20, "eSCT_Grip" },
            { 21, "eSCT_Suitcase" },
            { 22, "eSCT_VRConfig" },
            { 23, "eSCT_Hands" },
            { 24, "eSCT_StickingItems" },
            { 25, "eSCT_Fiberwire" }
        };

        (*enum_map)["ECrowdFlowCandidates"] = std::map<uint32_t, std::string>{
            { 0, "NONE" },
            { 1, "EVERYONE" },
            { 2, "SPAWNED_ON_FLOW" }
        };

        (*enum_map)["EThrownCollisionSoundLoudness"] = std::map<uint32_t, std::string>{
            { 0, "eLoudness_Default" },
            { 1, "eLoudness_Low" },
            { 2, "eLoudness_Normal" },
            { 3, "eLoudness_Loud" }
        };

        (*enum_map)["EUIBusyOperationId"] = std::map<uint32_t, std::string>{
            { 0, "eOperationId_GameSave" },
            { 1, "eOperationId_MenuTransition" },
            { 2, "eOperationId_ManuallyTriggered" },
            { 3, "eOperationId_TriggerBootFlow" },
            { 4, "eOperationId_SaveContract" },
            { 5, "eOperationId_StartContract" },
            { 6, "eOperationId_SessionStart" },
            { 7, "eOperationId_ReportContract" },
            { 8, "eOperationId_ConnectionFlow" },
            { 9, "eOperationId_LoadingTransitionDelayed" },
            { 10, "eOperationId_SaveSuccess" },
            { 11, "eOperationId_SaveFailed" },
            { 12, "eOperationId_SessionReconnect" },
            { 13, "eOperationId_NetSynchedMenuPause" },
            { 14, "eOperationId_NetSynchedMenuRestart" },
            { 15, "eOperationId_NetSynchedMenuReplan" },
            { 16, "eOperationId_StartingMultiplayerSession" },
            { 17, "eOperationId_HandlingMultiplayerRequest" },
            { 18, "eOperationId_PoorConnectivity" },
            { 19, "eOperationId_DefaultLoadout" },
            { 20, "eOperationId_Max" },
            { 21, "eOperationId_Custom_Begin" },
            { 22, "eOperationId_Custom_02" },
            { 23, "eOperationId_Custom_03" },
            { 24, "eOperationId_Custom_04" },
            { 25, "eOperationId_Custom_05" },
            { 26, "eOperationId_Custom_06" },
            { 27, "eOperationId_Custom_07" },
            { 28, "eOperationId_Custom_08" },
            { 29, "eOperationId_Custom_09" },
            { 30, "eOperationId_Custom_10" },
            { 31, "eOperationId_Custom_11" },
            { 32, "eOperationId_Custom_12" },
            { 33, "eOperationId_Custom_13" },
            { 34, "eOperationId_Custom_14" },
            { 35, "eOperationId_Custom_15" },
            { 36, "eOperationId_Custom_16" },
            { 37, "eOperationId_Custom_17" },
            { 38, "eOperationId_Custom_18" },
            { 39, "eOperationId_Custom_19" },
            { 40, "eOperationId_Custom_20" },
            { 41, "eOperationId_Custom_21" },
            { 42, "eOperationId_Custom_22" },
            { 43, "eOperationId_Custom_23" },
            { 44, "eOperationId_Custom_24" },
            { 45, "eOperationId_Custom_25" },
            { 46, "eOperationId_Custom_End" },
            { 47, "eOperationId_None" }
        };

        (*enum_map)["EAttachLocation"] = std::map<uint32_t, std::string>{
            { 0, "eALRightHand" },
            { 1, "eALLeftHand" },
            { 2, "eALFreeBone" },
            { 3, "eALBack" },
            { 4, "eALRifle" },
            { 5, "eALAttachCount" },
            { 6, "eALUndefined" }
        };

        (*enum_map)["EEngineFrameUpdatePriority"] = std::map<uint32_t, std::string>{
            { 0, "eFUP_AILogger" },
            { 1, "eFUP_GridManager" },
            { 2, "eFrameUpdatePriority_HitmanStart" },
            { 3, "eFUP_ActorManager_Prepare" },
            { 4, "eFrameUpdatePriority_CrowdSyncAI" },
            { 5, "eFrameUpdatePriority_PlayerCommunicationEarly" },
            { 6, "eFrameUpdatePriority_Scatter" },
            { 7, "eFrameUpdatePriority_VolumeTriggerManager" },
            { 8, "eFrameUpdatePriority_DistanceToCamManager" },
            { 9, "eFrameUpdatePriority_Door" },
            { 10, "eFUP_GameEntityManager" },
            { 11, "eFrameUpdatePriority_AreaManager" },
            { 12, "eFrameUpdatePriority_SpeechManager" },
            { 13, "eFUP_SmoothOperator" },
            { 14, "eFUP_LimitedVisionArea" },
            { 15, "eFUP_WindManager" },
            { 16, "eFrameUpdatePriority_KeywordDebug" },
            { 17, "eFUP_DebugUPlot" },
            { 18, "eFUP_IntelManager" },
            { 19, "eFrameUpdatePriority_LocalizationManager" },
            { 20, "eFrameUpdatePriority_SubtitleManager" },
            { 21, "eFrameUpdatePriority_HttpManager" },
            { 22, "eFrameUpdatePriority_WebsocketManager" },
            { 23, "eFrameUpdatePriority_OnlineEventsClient" },
            { 1001, "eFrameUpdatePriority_OutfitStreamingManager" },
            { 24, "eFrameUpdatePriority_OnlineEventManager" },
            { 25, "eFrameUpdatePriority_OnlineManager" },
            { 26, "eFrameUpdatePriority_EntitlementManager" },
            { 27, "eFrameUpdatePriority_MetricsManager" },
            { 28, "eFUP_SoundGateManager" },
            { 29, "eFUP_SoundAmbienceManager" },
            { 30, "eFrameUpdatePriority_UserProfileManager" },
            { 31, "eFrameUpdatePriority_RenderManager" },
            { 32, "eFrameUpdatePriority_TextInputDialog" },
            { 33, "eFrameUpdatePriority_SystemDialog" },
            { 34, "eFrameUpdatePriority_ContractsManager" },
            { 35, "eFUP_ParticleHeirEmitting" },
            { 36, "eFrameUpdatePriority_EditorServer" },
            { 37, "eFrameUpdatePriority_DebugConsole" },
            { 38, "eFrameUpdatePriority_GraphicsSettingsManager" },
            { 39, "eFrameUpdatePriority_ContractsGameManager" },
            { 40, "eFrameUpdatePriority_Weapon" },
            { 41, "eFrameUpdatePriority_CrowdDensityEntity" },
            { 42, "eFrameUpdatePriority_ActBehaviorStateCondition" },
            { 43, "eFrameUpdatePriority_ActorCanSeeCondition" },
            { 44, "eFrameUpdatePriority_DebugCheckKey" },
            { 45, "eFrameUpdatePriority_KeyControl" },
            { 46, "eFUP_LookAtTrigger" },
            { 47, "eFUP_FootstepEventConsumer" },
            { 48, "eFUP_BloodSplatCreator" },
            { 49, "eFrameUpdatePriority_PersistentEntityManager" },
            { 50, "eFUP_ProjectedCrowdRegion" },
            { 51, "eFrameUpdatePriority_Timers" },
            { 52, "eFrameUpdatePriority_ConditionalTimer" },
            { 53, "eFrameUpdatePriority_ValueEntityManager" },
            { 54, "eFrameUpdatePriority_ActorCounter" },
            { 55, "eFrameUpdatePriority_WaveformGeneratorManager" },
            { 56, "eFUP_ItsATrap" },
            { 57, "eFrameUpdatePriority_SecurityCamera" },
            { 58, "eFrameUpdatePriority_QueryingAIOutput" },
            { 59, "eFrameUpdatePriority_Projectile" },
            { 60, "eFrameUpdatePriority_MassImpulseCharacterExploder" },
            { 61, "eFrameUpdatePriority_Hitman" },
            { 62, "eFrameUpdatePriority_AudioManagerWwise" },
            { 63, "eFUP_GridManagerWaitForJobs" },
            { 64, "eFUP_ActorManager_StateUpdate_Pre" },
            { 65, "eFUP_ActorManager_StateUpdate" },
            { 66, "eFrameUpdatePriority_ActorAnimUpdate" },
            { 67, "eFrameUpdatePriority_CrowdSyncAll" },
            { 68, "eFrameUpdatePriority_CrowdCulling" },
            { 69, "eFrameUpdatePriority_PreCrowd" },
            { 70, "eFrameUpdatePriority_Crowd" },
            { 71, "eFrameUpdatePriority_PhysicsStart" },
            { 200, "eFrameUpdateLinkedEntity" },
            { 72, "eFrameUpdatePriority_ActorManager_AIUpdate" },
            { 201, "eFrameUpdatePriority_RayProbeEntity" },
            { 73, "eFUP_SmuggleSituationManager" },
            { 202, "eFrameUpdatePriority_GhostEntity" },
            { 74, "eFrameUpdatePriority_ActorManager3" },
            { 203, "eFrameUpdateLinkedEntityManagerMainThread" },
            { 75, "eFrameUpdatePriority_CharacterTemplateAspect" },
            { 204, "eFUP_syncDestructionEffects" },
            { 76, "eFrameUpdatePriority_Hitman2" },
            { 205, "eFrameUpdatePriority_RaycastsStart" },
            { 77, "eFrameUpdatePriority_PlayerCommunicationLate" },
            { 206, "eFrameUpdatePriority_ActorStartNetworkUpdate" },
            { 78, "eFrameUpdatePriority_ParticleDecalSpawn" },
            { 79, "eFrameUpdatePriority_ParticleColliderEmission" },
            { 80, "eFrameUpdatePriority_Physics" },
            { 81, "eFrameUpdatePriority_EntityManager" },
            { 82, "eFrameUpdatePriority_FreeCameraControlEntity" },
            { 83, "eFrameUpdatePriority_RenderVideoPlayer" },
            { 84, "eFrameUpdatePriority_AnimPlayerEntityManager" },
            { 85, "eFrameUpdatePriority_CameraAlignedBoneAnimator" },
            { 86, "eFrameUpdatePriority_ClothCharacterEntity" },
            { 87, "eFrameUpdatePriority_TextureStreamingCameraControlEntity" },
            { 88, "eFrameUpdatePriority_PostFilter" },
            { 89, "eFrameUpdatePriority_VideoPlayerControl" },
            { 90, "eFrameUpdatePriority_TrackDollyController" },
            { 91, "eFrameUpdatePriority_TestGroup" },
            { 92, "eFrameUpdatePriority_LookAtEntity" },
            { 93, "eFUP_InfoDisplay" },
            { 94, "eFUP_CrowdPossedAct" },
            { 95, "eFUP_SoundCollisionManager" },
            { 96, "eFUP_AudioLevelMeter" },
            { 97, "eFUP_ActorInstanceEntityManager" },
            { 98, "eFUP_WorldInventory" },
            { 99, "eFUP_DestructionSystem" },
            { 100, "eFUP_DestructionSystemPostTick" },
            { 101, "eFUP_DecalSpawnManager" },
            { 102, "eFrameUpdatePriority_DebugHandlers" },
            { 103, "eFrameUpdatePriority_OSD" },
            { 104, "eFrameUpdatePriority_Default" },
            { 105, "eFrameUpdatePriority_SecuritySystemCameraUI" },
            { 106, "eFrameUpdatePriority_ThrownItemUI" },
            { 107, "eFrameUpdatePriority_SequenceManager" },
            { 108, "eFrameUpdatePriority_StartPathfinderUpdate" },
            { 109, "eFrameUpdatePriority_VisibilitySensorWait" },
            { 110, "eFrameUpdatePriority_ActionManagerStart" },
            { 501, "eFrameUpdatePriority_NotificationsController" },
            { 111, "eFrameUpdatePriority_PathFinderJobManager" },
            { 500, "eFrameUpdatePriority_TextureManager" },
            { 502, "eFrameUpdatePriority_WaitForPathfinderUpdate" },
            { 1000, "eFrameUpdatePriority_LevelManager" }
        };

        (*enum_map)["EInformationBarMessage"] = std::map<uint32_t, std::string>{
            { 0, "eIBM_Clear" },
            { 1, "eIBM_LastEnemyKilledCP" },
            { 2, "eIBM_InvestigateStand" },
            { 3, "eIBM_BodyFound" },
            { 4, "eIBM_Suspicious" },
            { 5, "eIBM_DisguiseBlown" },
            { 6, "eIBM_Hunt" },
            { 7, "eIBM_Exposed" },
            { 8, "eIBM_LandMineArmed" },
            { 9, "eIBM_RadioOn" },
            { 10, "eIBM_Trespassing" },
            { 11, "eIBM_NUM" }
        };

        (*enum_map)["ESniperScoreMessageType"] = std::map<uint32_t, std::string>{
            { 0, "ESSMT_STYLE_DEFAULT" },
            { 1, "ESSMT_STYLE_SCORE" },
            { 2, "ESSMT_STYLE_PENALTY" },
            { 3, "ESSMT_STYLE_STREAK" },
            { 4, "ESSMT_STYLE_CIVKILL" }
        };

        (*enum_map)["ZHttpUrl.EVerb"] = std::map<uint32_t, std::string>{
            { 0, "eNONE" },
            { 1, "eGET" },
            { 2, "ePOST" },
            { 3, "ePUT" },
            { 4, "eHEAD" }
        };

        (*enum_map)["ESmoothingMode"] = std::map<uint32_t, std::string>{
            { 0, "ESM_LINEAR" },
            { 1, "ESM_EXPONENTIAL" }
        };

        (*enum_map)["ZActorPickerEffectTemplate.EEffectPhaseAction"] = std::map<uint32_t, std::string>{
            { 0, "EPA_NONE" },
            { 1, "EPA_START_TEMPLATE" },
            { 2, "EPA_STOP_TEMPLATE" }
        };

        (*enum_map)["EActorAIDot"] = std::map<uint32_t, std::string>{
            { 0, "eAAID_None" },
            { 1, "eAAID_Distracted" },
            { 2, "eAAID_PotentialThreat" },
            { 3, "eAAID_PotentialThreatDistracted" },
            { 4, "eAAID_Aggressive" },
            { 5, "eAAID_EscortingOut" },
            { 6, "eAAID_Fleeing" },
            { 7, "eAAID_Unconscious" },
            { 8, "eAAID_Stunned" },
            { 9, "eAAID_Grenade" },
            { 100, "eAAID_PotentialThreatDisabled" }
        };

        (*enum_map)["ZDecalControllerEntity.EScaleType"] = std::map<uint32_t, std::string>{
            { 0, "eScaleDisable" },
            { 1, "eScaleUniform" },
            { 2, "eScaleY" }
        };

        (*enum_map)["ZUIControlLayoutLegacyAspect.ELayoutMode"] = std::map<uint32_t, std::string>{
            { 0, "E_LAYOUT_MODE_Absolute" },
            { 1, "E_LAYOUT_MODE_Proportional" },
            { 2, "E_LAYOUT_MODE_Proportional_MaintainAspect" },
            { 3, "E_LAYOUT_MODE_Proportional_MaintainAspect_Fill" },
            { 4, "E_LAYOUT_MODE_Proportional_Scale_Fit" },
            { 5, "E_LAYOUT_MODE_Proportional_Scale_MaintainAspect" },
            { 6, "E_LAYOUT_MODE_Proportional_Scale_MaintainAspect_Fill" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioClass"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioClass_AssaultRifle" },
            { 1, "eWBC_AudioClass_Pistol" },
            { 2, "eWBC_AudioClass_Shotgun" },
            { 3, "eWBC_AudioClass_SMG" },
            { 4, "eWBC_AudioClass_SniperRifle" }
        };

        (*enum_map)["ZUIControlLayoutLegacyAspect.EAlignmentType"] = std::map<uint32_t, std::string>{
            { 0, "E_ALIGNMENT_TYPE_Center" },
            { 1, "E_ALIGNMENT_TYPE_TopCenter" },
            { 2, "E_ALIGNMENT_TYPE_BottomCenter" },
            { 3, "E_ALIGNMENT_TYPE_CenterLeft" },
            { 4, "E_ALIGNMENT_TYPE_CenterRight" },
            { 5, "E_ALIGNMENT_TYPE_TopLeft" },
            { 6, "E_ALIGNMENT_TYPE_TopRight" },
            { 7, "E_ALIGNMENT_TYPE_BottomLeft" },
            { 8, "E_ALIGNMENT_TYPE_BottomRight" }
        };

        (*enum_map)["ZEscortSituationEntity.EEscortState"] = std::map<uint32_t, std::string>{
            { 0, "EES_Idle" },
            { 1, "EES_Evaluate" },
            { 2, "EES_Escorting" },
            { 3, "EES_Searching" },
            { 4, "EES_Intermediate" }
        };

        (*enum_map)["SniperControllerConditionType"] = std::map<uint32_t, std::string>{
            { 0, "SCCT_Enabled" },
            { 1, "SCCT_PrecisionAim" }
        };

        (*enum_map)["ZActorInstanceEntity.EFFXMode"] = std::map<uint32_t, std::string>{
            { 0, "eFFX_MODE_DISABLE" },
            { 1, "eFFX_MODE_OVERWRITE" }
        };

        (*enum_map)["EMapType"] = std::map<uint32_t, std::string>{
            { 0, "E_MAPTYPE_Minimap" },
            { 1, "E_MAPTYPE_MainMap" },
            { 2, "E_MAPTYPE_MenuMap" }
        };

        (*enum_map)["CrowdUtil.ECrowdFacing"] = std::map<uint32_t, std::string>{
            { 0, "FRONT" },
            { 1, "BACK" }
        };

        (*enum_map)["EActorEmotionState"] = std::map<uint32_t, std::string>{
            { 0, "AES_Ambient" },
            { 1, "AES_Alerted" },
            { 2, "AES_Scared" },
            { 3, "AES_Hunt" },
            { 4, "AES_Combat" },
            { 268435455, "AES_None" }
        };

        (*enum_map)["EKeywordSetBehavior"] = std::map<uint32_t, std::string>{
            { 0, "EKWSB_All" },
            { 1, "EKWSB_None" },
            { 2, "EKWSB_Any" },
            { 3, "EKWSB_No" }
        };

        (*enum_map)["eItemRarity"] = std::map<uint32_t, std::string>{
            { 0, "ITEMRARITY_COMMON" },
            { 1, "ITEMRARITY_UNCOMMON" },
            { 2, "ITEMRARITY_RARE" }
        };

        (*enum_map)["eHolsterAbility"] = std::map<uint32_t, std::string>{
            { 0, "eUndecided" },
            { 1, "eHolsteringAllowed" },
            { 2, "eHolsterTemporarilyOnly" },
            { 3, "eHolsterSecondaryOnly" },
            { 4, "eHolsterQuestStorage" },
            { 5, "eCanNotBeHolstered" }
        };

        (*enum_map)["EMoveEndState"] = std::map<uint32_t, std::string>{
            { 0, "MES_Moving" },
            { 1, "MES_Stopping" },
            { 2, "MES_Standing" }
        };

        (*enum_map)["EPlaceableType"] = std::map<uint32_t, std::string>{
            { 0, "PLACEABLE_NOT" },
            { 1, "PLACEABLE_FULL_PHYSICS" },
            { 2, "PLACEABLE_SLEEP_PHYSICS" },
            { 3, "PLACEABLE_NO_PHYSICS" },
            { 4, "PLACEABLE_ATTACH" }
        };

        (*enum_map)["ELastSoundTension"] = std::map<uint32_t, std::string>{
            { 0, "EST_Unknown" },
            { 1, "EST_Ambient" },
            { 2, "EST_Yellow" },
            { 3, "EST_Orange" },
            { 4, "EST_Red" }
        };

        (*enum_map)["ZSequenceEntity.ELetterBoxAspect"] = std::map<uint32_t, std::string>{
            { 0, "LETTERBOXASPECT_WIDESCREEN" },
            { 1, "LETTERBOXASPECT_CINEMASCOPE" }
        };

        (*enum_map)["ZHM5HIKEventConsumer.EEffector"] = std::map<uint32_t, std::string>{
            { 0, "eLeftHand" },
            { 1, "eRightHand" }
        };

        (*enum_map)["ZWaypointEntity.EMovementType"] = std::map<uint32_t, std::string>{
            { 0, "MT_WALK" },
            { 1, "MT_WALK_IF_NOT_IN_SEQUENCE" },
            { 2, "MT_SNAP" },
            { 3, "MT_IGNORE_POSITION" }
        };

        (*enum_map)["EInventoryStorageType"] = std::map<uint32_t, std::string>{
            { 0, "EIST_None" },
            { 1, "EIST_Bag" },
            { 2, "EIST_RightHand" },
            { 3, "EIST_LeftHand" },
            { 4, "EIST_Back" },
            { 5, "EIST_Support" },
            { 6, "EIST_Quest" },
            { 7, "EIST_Temporary" },
            { 8, "EIST_Debug" }
        };

        (*enum_map)["EDialogEventEndReason"] = std::map<uint32_t, std::string>{
            { 0, "EDialogEvent_Completed" },
            { 1, "EDialogEvent_Stopped" }
        };

        (*enum_map)["EDefaultCollidableLayer"] = std::map<uint32_t, std::string>{
            { 0, "DCL_STATIC" },
            { 1, "DCL_KINEMATIC" },
            { 2, "DCL_KINEMATIC_TRANSPARENT" },
            { 3, "DCL_DYNAMIC" },
            { 4, "DCL_DYNAMIC_TRANSPARENT" },
            { 5, "DCL_COLLIDE_ALL" },
            { 6, "DCL_STATIC_TRANSPARENT" },
            { 7, "DCL_COLLIDE_STATIC_ONLY" },
            { 8, "DCL_DYNAMIC_NO_CHARACTER" },
            { 9, "DCL_UNUSED_LAST" }
        };

        (*enum_map)["eItemHands"] = std::map<uint32_t, std::string>{
            { 0, "IH_NONE" },
            { 1, "IH_ONEHANDED" },
            { 2, "IH_TWOHANDED" }
        };

        (*enum_map)["ZActorProviderFilterKeyword.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["ZHUDUIRoot.EHUDVisibility"] = std::map<uint32_t, std::string>{
            { 0, "eHV_INSTANT_OFF" },
            { 1, "eHV_FADE_OUT" },
            { 2, "eHV_FADE_IN" },
            { 3, "eHV_INSTANT_ON" }
        };

        (*enum_map)["EKillType"] = std::map<uint32_t, std::string>{
            { 0, "EKillType_Undefined" },
            { 1, "EKillType_Throw" },
            { 2, "EKillType_Fiberwire" },
            { 3, "EKillType_PistolExecute" },
            { 4, "EKillType_ItemTakeOutFront" },
            { 5, "EKillType_ItemTakeOutBack" },
            { 6, "EKillType_ChokeOut" },
            { 7, "EKillType_SnapNeck" },
            { 8, "EKillType_KnockOut" },
            { 9, "EKillType_Push" },
            { 10, "EKillType_Pull" }
        };

        (*enum_map)["EHUDIconType"] = std::map<uint32_t, std::string>{
            { 0, "HUD_ICON_NONE" },
            { 1, "HUD_ICON_ALARM_TIMER" },
            { 2, "HUD_ICON_CLOSECOMBAT" },
            { 3, "HUD_ICON_BREADCRUMB" },
            { 4, "HUD_ICON_EXIT" },
            { 5, "HUD_ICON_OBJECTIVE" },
            { 6, "HUD_ICON_TARGET" },
            { 7, "HUD_ICON_TUTORIAL_ARROW" }
        };

        (*enum_map)["ECharacterActionSyncRequests"] = std::map<uint32_t, std::string>{
            { 1, "eSM_ASR_Reload" },
            { 2, "eSM_ASR_SwapItemHandL" },
            { 4, "eSM_ASR_SwapItemHandR" }
        };

        (*enum_map)["ZInvestigateCautiousSituation.ESituationState"] = std::map<uint32_t, std::string>{
            { 0, "SS_Main" },
            { 1, "SS_StandDownPending" },
            { 2, "SS_StandDown" }
        };

        (*enum_map)["EActBodyType"] = std::map<uint32_t, std::string>{
            { 0, "ABT_UpperBodyOnly" },
            { 1, "ABT_FullBodyWithLeadIn" },
            { 2, "ABT_FullBodyOnly" }
        };

        (*enum_map)["ZHUDOccluderTriggerEntity.EOccluderTestBit"] = std::map<uint32_t, std::string>{
            { 1, "PhysX_Far" },
            { 2, "Crowd_Far" },
            { 4, "PhysX_Middle" },
            { 128, "Crowd_LowerRight" },
            { 64, "PhysX_LowerRight" },
            { 32, "Crowd_LowerLeft" },
            { 16, "PhysX_LowerLeft" },
            { 8, "Crowd_Middle" },
            { 256, "PhysX_UpperLeft" },
            { 512, "PhysX_UpperRight" },
            { 1024, "LateralShift" }
        };

        (*enum_map)["EOpportunityRevealState"] = std::map<uint32_t, std::string>{
            { 1, "ORS_REVEALING" },
            { 9, "ORS_REVEAL_FLAGS" },
            { 2, "ORS_HINT" },
            { 4, "ORS_DISTANCE" },
            { 6, "ORS_RANGE_FLAGS" },
            { 8, "ORS_REVEALED" },
            { 16, "ORS_BYPASS_MENU" }
        };

        (*enum_map)["EGSBodyPart"] = std::map<uint32_t, std::string>{
            { 0, "GSBODYPART_UNKNOWN" },
            { 1, "GSBODYPART_HEAD" },
            { 2, "GSBODYPART_TORSO" },
            { 3, "GSBODYPART_ARM" },
            { 4, "GSBODYPART_LEG" }
        };

        (*enum_map)["EActorSecondaryIconState"] = std::map<uint32_t, std::string>{
            { 0, "eSIS_Clear" },
            { 1, "eSIS_Infected" },
            { 2, "eSIS_Infected_Stage1" },
            { 3, "eSIS_Infected_Stage2" },
            { 4, "eSIS_Infected_Stage3" }
        };

        (*enum_map)["ZInputListenerAxisEntity.eInputListenerAxes"] = std::map<uint32_t, std::string>{
            { 0, "EILA_HORIZONTAL" },
            { 1, "EILA_VERTICAL" },
            { 2, "EILA_TRIGGER_1" },
            { 3, "EILA_TRIGGER_2" }
        };

        (*enum_map)["EActorEventTypes"] = std::map<uint32_t, std::string>{
            { 0, "eAET_OnAlive" },
            { 1, "eAET_OnPacified" },
            { 2, "eAET_OnMurdered" },
            { 3, "eAET_OnAccidentDeath" },
            { 4, "eAET_OnUnnoticableKill" },
            { 5, "eAET_OnBodyNoticed" },
            { 6, "eAET_OnBodyBagged" },
            { 7, "eAET_OnDying" },
            { 8, "eAET_OnDead" },
            { 9, "eAET_OnBodyHidden" },
            { 10, "eAET_OnBodyFlushed" },
            { 11, "eAET_OnBodyDumped" },
            { 12, "eAET_OnDelete" }
        };

        (*enum_map)["EVRConfigCustomEvent"] = std::map<uint32_t, std::string>{
            { 0, "EVRCCE_ClosetInside" },
            { 1, "EVRCCE_ClosetExiting" },
            { 2, "EVRCCE_DrainPipeMounted" },
            { 3, "EVRCCE_DrainPipeStartedDismounting" },
            { 4, "EVRCCE_LadderMounted" },
            { 5, "EVRCCE_LadderStartedDismounting" },
            { 6, "EVRCCE_LedgeChange" },
            { 7, "EVRCCE_LedgeShimmyStart" },
            { 8, "EVRCCE_LedgeShimmyStop" },
            { 9, "EVRCCE_PeekEntered" },
            { 10, "EVRCCE_PeekStartedExiting" },
            { 11, "EVRCCE_FriskStartedTurning" },
            { 12, "EVRCCE_FriskEndedTurning" },
            { 13, "EVRCCE_ShowItemStartedTurning" },
            { 14, "EVRCCE_ShowItemEndedTurning" },
            { 15, "EVRCCE_SubactionAnimationStarted" },
            { 16, "EVRCCE_SubactionAnimation_Hide" },
            { 17, "EVRCCE_FocussedInteractionEntered" },
            { 18, "EVRCCE_FocussedInteractionStartedExiting" },
            { 19, "EVRCCE_DisguiseSafeZoneEntered" },
            { 20, "EVRCCE_DisguiseSafeZoneStartedExiting" },
            { 21, "EVRCCE_SniperModeEnter" },
            { 22, "EVRCCE_SniperModeEntered" },
            { 23, "EVRCCE_SniperModeExit" },
            { 24, "EVRCCE_SniperModeExited" },
            { 25, "EVRCCE_VictimMovement0" },
            { 26, "EVRCCE_VictimMovement1" },
            { 27, "EVRCCE_VictimMovement2" },
            { 28, "EVRCCE_VictimMovement3" },
            { 29, "EVRCCE_VictimMovementRemoveControl" },
            { 30, "EVRCCE_AgilityLowVault" },
            { 31, "EVRCCE_AgilityHighVault" },
            { 32, "EVRCCE_AgilityRemoveControl" },
            { 33, "EVRCCE_FiberwireStartPriming" },
            { 34, "EVRCCE_FiberwireStopPriming" },
            { 35, "EVRCCE_ToiletDrownEntered" },
            { 36, "EVRCCE_ToiletDrownExited" },
            { 37, "EVRCCE_CloseCombatStartPriming" },
            { 38, "EVRCCE_CloseCombatStopPriming" },
            { 39, "EVRCCE_SilentTakedownEnterActivationArea" },
            { 40, "EVRCCE_SilentTakedownExitActivationArea" },
            { 41, "EVRCCE_SilentTakedownInstaPacification" },
            { 42, "EVRCCE_SnapNeckEnterActivationArea" },
            { 43, "EVRCCE_SnapNeckExitActivationArea" },
            { 44, "EVRCCE_TeleportStart" },
            { 45, "EVRCCE_TeleportEnd" }
        };

        (*enum_map)["EMultiplayerNetworkState"] = std::map<uint32_t, std::string>{
            { 0, "Base" },
            { 1, "Idle" },
            { 2, "Searching" },
            { 3, "Connecting" },
            { 4, "Joining" },
            { 5, "Creating" },
            { 6, "Connected" },
            { 7, "Disconnecting" },
            { 8, "Count" }
        };

        (*enum_map)["EBoolCheckType"] = std::map<uint32_t, std::string>{
            { 0, "eBCT_IGNORE" },
            { 1, "eBCT_TRUE" },
            { 2, "eBCT_FALSE" }
        };

        (*enum_map)["EActorFaction"] = std::map<uint32_t, std::string>{
            { 0, "eActorFaction_Default" },
            { 1, "eActorFaction_VIP_1" },
            { 2, "eActorFaction_VIP_2" },
            { 3, "eActorFaction_VIP_3" },
            { 4, "eActorFaction_VIP_4" },
            { 5, "eActorFaction_VIP_5" },
            { 6, "eActorFaction_VIP_6" },
            { 7, "eActorFaction_VIP_7" }
        };

        (*enum_map)["SoundPlayState"] = std::map<uint32_t, std::string>{
            { 0, "ePlaying" },
            { 1, "eLoopBreaking" },
            { 2, "eStopping" }
        };

        (*enum_map)["ECausalGraphTraversal"] = std::map<uint32_t, std::string>{
            { 0, "eCGT_NONE" },
            { 1, "eCGT_INPUT" },
            { 2, "eCGT_OUTPUT" },
            { 3, "eCGT_BOTH" }
        };

        (*enum_map)["EPushNotificationType"] = std::map<uint32_t, std::string>{
            { 0, "PUSH_NOTIFICATION_OBJECTIVE" },
            { 1, "PUSH_NOTIFICATION_DISGUISE" },
            { 2, "PUSH_NOTIFICATION_CONTRACT" },
            { 3, "PUSH_NOTIFICATION_CHALLENGE" },
            { 4, "PUSH_NOTIFICATION_RATING" },
            { 5, "PUSH_NOTIFICATION_SPECIAL_RATING_UNLOCKED" },
            { 6, "PUSH_NOTIFICATION_CONTRACT_PICKED_UP" },
            { 7, "PUSH_NOTIFICATION_TECHNIQUE_UNLOCKED" },
            { 8, "PUSH_NOTIFICATION_SCORE_COMPARISON" }
        };

        (*enum_map)["ZCharacterSpeakCondition.EState"] = std::map<uint32_t, std::string>{
            { 0, "Started" },
            { 1, "Playing" },
            { 2, "PlayingAndAudible" },
            { 3, "Completed" },
            { 4, "SubsSeenAndCompleted" },
            { 5, "Failed" },
            { 6, "Stopped" }
        };

        (*enum_map)["EVRRenderDeviceType"] = std::map<uint32_t, std::string>{
            { 0, "RENDER_VR_DEVICE_TYPE_DUMMY" },
            { 1, "RENDER_VR_DEVICE_TYPE_OCULUS" },
            { 2, "RENDER_VR_DEVICE_TYPE_PSVR" },
            { 3, "RENDER_VR_DEVICE_TYPE_COUNT" }
        };

        (*enum_map)["IHM5Door.EOpenDir"] = std::map<uint32_t, std::string>{
            { 0, "OD_AWAY" },
            { 1, "OD_TOWARS" },
            { 2, "OD_IN" },
            { 3, "OD_OUT" }
        };

        (*enum_map)["EDamageResponse"] = std::map<uint32_t, std::string>{
            { 0, "eDR_Fractured" },
            { 1, "eDR_Detached" },
            { 2, "eDR_Destroyed" },
            { 3, "eDR_Collided" },
            { 4, "eDR_Count" }
        };

        (*enum_map)["EInventoryConfigFormerEquippedItems"] = std::map<uint32_t, std::string>{
            { 0, "EICFEI_Equip" },
            { 1, "EICFEI_EquipOrPickup" },
            { 2, "EICFEI_Forget" }
        };

        (*enum_map)["ESituationJoinReason"] = std::map<uint32_t, std::string>{
            { 0, "AISJR_Default" },
            { 1, "AISJR_HeardSound" },
            { 2, "AISJR_Alarm" },
            { 3, "AISJR_HitmanStrange" },
            { 4, "AISJR_HitmanIllegal" },
            { 5, "AISJR_Assist" },
            { 6, "AISJR_AssistingGuard" },
            { 7, "AISJR_Propagate" },
            { 8, "AISJR_ResumeSituation" },
            { 9, "AISJR_Spawned" },
            { 10, "AISJR_HelpCivilian" },
            { 11, "AISJR_Escalating" },
            { 12, "AISJR_DeadBody" },
            { 13, "AISJR_Accident" },
            { 14, "AISJR_StandDown" },
            { 15, "AISJR_Report" },
            { 16, "AISJR_ForcedToHold" },
            { 17, "AISJR_Wounded" },
            { 18, "AISJR_SC_HeardBulletImpact" },
            { 19, "AISJR_SC_HeardSetPiece" }
        };

        (*enum_map)["UIMapLayer.EUIMapLayerID"] = std::map<uint32_t, std::string>{
            { 0, "eUIMLI_UNSPECIFIED" },
            { 1, "eUIMLI_STAIRCASE" },
            { 2, "eUIMLI_AREA_UNDISCOVERED" },
            { 3, "eUIMLI_TEXT" },
            { 4, "eUIMLI_DROPPED_ITEMS_AND_DISGUISES" },
            { 5, "eUIMLI_NPC" },
            { 6, "eUIMLI_NORTH_INDICATOR" },
            { 7, "eUIMLI_SECURITY_CAMERA" },
            { 8, "eUIMLI_AGENCY_PICKUP" },
            { 9, "eUIMLI_OPPORTUNITY" },
            { 10, "eUIMLI_EXIT" },
            { 11, "eUIMLI_OBJECTIVE" },
            { 12, "eUIMLI_TARGET" },
            { 13, "eUIMLI_OPPONENT" },
            { 14, "eUIMLI_HERO" }
        };

        (*enum_map)["EScreenplayStateFlag"] = std::map<uint32_t, std::string>{
            { 0, "eSSF_DEFAULT" },
            { 8, "eSSF_RUNNING" },
            { 16, "eSSF_DONE" },
            { 32, "eSSF_TERMINATED" },
            { 1, "eSSF_ENABLED" },
            { 2, "eSSF_CAST" },
            { 4, "eSSF_RESUMING" }
        };

        (*enum_map)["ENetPlayerEvent"] = std::map<uint32_t, std::string>{
            { 0, "Login" },
            { 1, "Logout" },
            { 2, "NewHost" }
        };

        (*enum_map)["ZCoverPlane.ECoverType"] = std::map<uint32_t, std::string>{
            { 0, "eCoverOnly" },
            { 1, "eCoverAndRail" },
            { 2, "eRailOnly" }
        };

        (*enum_map)["ECameraOffset_old"] = std::map<uint32_t, std::string>{
            { 0, "eCameraOffset_Center" },
            { 1, "eCameraOffset_Left" },
            { 2, "eCameraOffset_Right" }
        };

        (*enum_map)["EVRConfigCapsuleGridSize"] = std::map<uint32_t, std::string>{
            { 0, "EVRCCGS_RegularGrid" },
            { 1, "EVRCCGS_SmallGrid" }
        };

        (*enum_map)["ECompiledConditionType"] = std::map<uint32_t, std::string>{
            { 0, "CT_IsCurrent" },
            { 1, "CT_Current" },
            { 2, "CT_IsExpired" },
            { 3, "CT_HasGoal" },
            { 4, "CT_HasGoalOnTarget" },
            { 5, "CT_HasUnhandledGoal" },
            { 6, "CT_MaxActorsWithGoal" },
            { 7, "CT_MinExpired" },
            { 8, "CT_MaxExpired" },
            { 9, "CT_MinAge" },
            { 10, "CT_MaxAge" },
            { 11, "CT_IsSharedOlder" },
            { 12, "CT_HasLongTermMemory" },
            { 13, "CT_IsSharedEventCurrent" },
            { 14, "CT_SharedEventCurrent" },
            { 15, "CT_IsSharedEventExpired" },
            { 16, "CT_MinSharedEventExpired" },
            { 17, "CT_MaxSharedEventExpired" },
            { 18, "CT_MinSharedEventAge" },
            { 19, "CT_MaxSharedEventAge" },
            { 20, "CT_IsNull" },
            { 21, "CT_IsEqual" },
            { 22, "CT_IsOfType" },
            { 23, "CT_IsOfActorType" },
            { 24, "CT_IsOfActorRank" },
            { 25, "CT_IsOfActorRole" },
            { 26, "CT_IsActorRoleBodyguard" },
            { 27, "CT_IsActorRoleVIP" },
            { 28, "CT_IsActorDeafBlind" },
            { 29, "CT_IsActorAlive" },
            { 30, "CT_IsActorConscious" },
            { 31, "CT_HasConfiguredAct" },
            { 32, "CT_HasConfiguredSpeak" },
            { 33, "CT_IsConfiguredConditionTrue" },
            { 34, "CT_HasTension" },
            { 35, "CT_HasTensionOrHigher" },
            { 36, "CT_IsAIModified" },
            { 37, "CT_HasSameOutfit" },
            { 38, "CT_IsLegal" },
            { 39, "CT_IsItemLegalOnHitman" },
            { 40, "CT_IsItemIllegalOnHitman" },
            { 41, "CT_IsItemSuspiciousOnHitman" },
            { 42, "CT_AreVisibleItemsLegal" },
            { 43, "CT_AreVisibleItemsIllegal" },
            { 44, "CT_AreVisibleItemsSuspicious" },
            { 45, "CT_AreVisibleWeaponsLegal" },
            { 46, "CT_AreVisibleWeaponsIllegal" },
            { 47, "CT_AreVisibleWeaponsSuspicious" },
            { 48, "CT_OnlyLegalWeaponsDropped" },
            { 49, "CT_IllegalWeaponDropped" },
            { 50, "CT_SuspiciousWeaponDropped" },
            { 51, "CT_OnlyLegalItemsDropped" },
            { 52, "CT_IllegalItemDropped" },
            { 53, "CT_SuspiciousItemDropped" },
            { 54, "CT_OnlyLegalWeaponsPickedUp" },
            { 55, "CT_IllegalWeaponPickedUp" },
            { 56, "CT_SuspiciousWeaponPickedUp" },
            { 57, "CT_OnlyLegalItemsPickedUp" },
            { 58, "CT_IllegalItemPickedUp" },
            { 59, "CT_SuspiciousItemPickedUp" },
            { 60, "CT_ThrownBy" },
            { 61, "CT_GetItemBeingThrownBy" },
            { 62, "CT_Owner" },
            { 63, "CT_PreviousOwner" },
            { 64, "CT_IsOwner" },
            { 65, "CT_IsPreviousOwner" },
            { 66, "CT_GetItemDroppedBy" },
            { 67, "CT_CausedBy" },
            { 68, "CT_BeingPickedUpBy" },
            { 69, "CT_IsSuitcase" },
            { 70, "CT_IsNotSuitcase" },
            { 71, "CT_IsVisiblyPerceptible" },
            { 72, "CT_HasCustomInvestigationAckSoundDef" },
            { 73, "CT_FindStashPointsForItem" },
            { 74, "CT_MinDistance" },
            { 75, "CT_MaxDistance" },
            { 76, "CT_MinActualDistance" },
            { 77, "CT_MinActualDistance2D" },
            { 78, "CT_MaxActualDistance" },
            { 79, "CT_MaxActualDistance2D" },
            { 80, "CT_ActualInsideSlice" },
            { 81, "CT_MaxDistanceToKnownPosition" },
            { 82, "CT_IsDistanceFromKnownToActualGreaterThan" },
            { 83, "CT_IsDistanceFromKnownToActualLessThan" },
            { 84, "CT_InSituation" },
            { 85, "CT_IsActorInSituation" },
            { 86, "CT_SituationTarget" },
            { 87, "CT_SituationHuntTarget" },
            { 88, "CT_IsSituationTarget" },
            { 89, "CT_SituationFromActors" },
            { 90, "CT_ActorsNotInSituation" },
            { 91, "CT_SituationFromTarget" },
            { 92, "CT_TargetsNotInSituation" },
            { 93, "CT_MaxSituationAge" },
            { 94, "CT_MinSituationAge" },
            { 95, "CT_InPhase" },
            { 96, "CT_MinPhaseAge" },
            { 97, "CT_MaxPhaseAge" },
            { 98, "CT_IsOrderInvalid" },
            { 99, "CT_IsSituationOlderThanSharedEvent" },
            { 100, "CT_HasOrder" },
            { 101, "CT_HasPendingOrder" },
            { 102, "CT_IsCombatDisabled" },
            { 103, "CT_CanJoinSituation" },
            { 104, "CT_InfectedMayFollowPlayer" },
            { 105, "CT_CanDistractionEscalateTo" },
            { 106, "CT_CanReactToDistractionType" },
            { 107, "CT_CanInvestigateDistraction" },
            { 108, "CT_IsCurrentDistractionTarget" },
            { 109, "CT_IsNotCurrentDistractionTarget" },
            { 110, "CT_IsHighestPriorityTypeToInvestigate" },
            { 111, "CT_IsHighestPriorityTypeToInvestigateFor" },
            { 112, "CT_IsHighestPriorityTypeToReact" },
            { 113, "CT_HasBystanderSomethingElseToInvestigate" },
            { 114, "CT_IsDistractionNewerThanCurrentOne" },
            { 115, "CT_ShouldDistractionDiscardNewOcurrences" },
            { 116, "CT_IsDistractionInterruptableByNewOcurrences" },
            { 117, "CT_IsSameDistractionTypeAsTheCurrentOne" },
            { 118, "CT_IsDistracted" },
            { 119, "CT_HasCustomDistractionAckSoundDef" },
            { 120, "CT_HasCustomDistractionStndSoundDef" },
            { 121, "CT_PerceptibleEntityEndAfterReact" },
            { 122, "CT_PerceptibleEntityEndAfterMoveTo" },
            { 123, "CT_FindSuitcaseAssitanceForCivilian" },
            { 124, "CT_FindSuitcaseAssitanceForSentry" },
            { 125, "CT_IsValidForSuitcaseAssistance" },
            { 126, "CT_SuitcaseAssistanceRequesterFaceToFace" },
            { 127, "CT_SuitcaseAssistanceRequesterRadio" },
            { 128, "CT_IsDistractionTargetBeingHandled" },
            { 129, "CT_CanArrestReasonEscalateTo" },
            { 130, "CT_IsHighestSeverityReasonToArrest" },
            { 131, "CT_Breakpoint" },
            { 132, "CT_Cooldown" },
            { 133, "CT_Trigger" },
            { 134, "CT_IsInSniperIsland" },
            { 135, "CT_SituationIsType" },
            { 136, "CT_IntRuntimeParamEqualsTo" },
            { 137, "CT_IsNewArrestEnabled" },
            { 138, "CT_IsDebugBuild" },
            { 139, "CT_GetRandomFightTarget" },
            { 140, "CT_IsCombatPositionActReady" },
            { 141, "CT_CurrentSetpieceVisualDistractions" },
            { 142, "CT_CurrentSetpieceSoundDistractions" },
            { 143, "CT_AreBehaviorsDisabled" },
            { 144, "CT_IsDifficultyEqual" },
            { 145, "CT_IsDifficultyGreaterOrEqual" },
            { 146, "CT_IsDifficultyLessOrEqual" },
            { 147, "CT_IsSeasonTwoCombatEnabled" },
            { 148, "CT_IsRealCombatSituation" },
            { 149, "CT_RemoveSituationsWithDesiredNumberOfCombatants" },
            { 150, "CT_RemoveCombatSituationsStandingDown" },
            { 151, "CT_CombatSituationsWithDesiredNumberOfCombatants" },
            { 152, "CT_CombatSituationsThatCanDiscardCombatants" },
            { 153, "CT_CombatSituationsStandingDown" },
            { 154, "CT_HasCombatFightOrder" },
            { 155, "CT_IsAccidentDeathContext" },
            { 156, "CT_IsAccidentScaleContext" },
            { 157, "CT_IsAccidentVictimDisposable" },
            { 158, "CT_IsAccidentBystandingGuardSearching" }
        };

        (*enum_map)["EItemGripType"] = std::map<uint32_t, std::string>{
            { 0, "IGT_Empty" },
            { 1, "IGT_Suitcase" },
            { 2, "IGT_Suitcase_Big" },
            { 3, "IGT_Firearm_Pistol" },
            { 4, "IGT_Firearm_Pistol_Stealth" },
            { 5, "IGT_Firearm_AssaultRifle_Bullpup" },
            { 6, "IGT_Firearm_AssaultRifle_Carbine" },
            { 7, "IGT_Firearm_Shotgun" },
            { 8, "IGT_Firearm_SMG" },
            { 9, "IGT_Firearm_SMG02" },
            { 10, "IGT_Firearm_SniperRifle" },
            { 11, "IGT_Firearm_SniperRifle02" },
            { 12, "IGT_Firearm_Dartgun" },
            { 13, "IGT_Melee_1H_Baton" },
            { 14, "IGT_Melee_1H_Cleaver" },
            { 15, "IGT_Melee_1H_Hammer" },
            { 16, "IGT_Melee_1H_Knife" },
            { 17, "IGT_Melee_1H_Rock" },
            { 18, "IGT_Melee_1H_Screwdriver" },
            { 19, "IGT_Melee_1H_Stick" },
            { 20, "IGT_Melee_1H_Sword" },
            { 21, "IGT_Melee_1H_FireExtinguisher" },
            { 22, "IGT_Melee_1H_Bust" },
            { 23, "IGT_Melee_1H_Crowbar" },
            { 24, "IGT_Melee_1H_Syringe" },
            { 25, "IGT_Melee_2H_Axe" },
            { 26, "IGT_Melee_2H_Stick" },
            { 27, "IGT_Melee_2H_Sword" },
            { 28, "IGT_Gadget_Fiberwire" },
            { 29, "IGT_Gadget_Coin" },
            { 30, "IGT_Gadget_Vial" },
            { 31, "IGT_Gadget_C4" },
            { 32, "IGT_Gadget_Detonator" },
            { 33, "IGT_Gadget_Grenade_ThickGrip" },
            { 34, "IGT_Gadget_Grenade_ThinGrip" },
            { 35, "IGT_Gadget_Mine" },
            { 36, "IGT_Gadget_Remote" },
            { 37, "IGT_Prop_1H" },
            { 38, "IGT_Prop_1H_Duck" },
            { 39, "IGT_Prop_1h_Phone" },
            { 40, "IGT_None" }
        };

        (*enum_map)["EUIBusyState"] = std::map<uint32_t, std::string>{
            { 0, "eBusyState_Saving" },
            { 1, "eBusyState_Saving_Failed" },
            { 2, "eBusyState_Saving_Success" },
            { 3, "eBusyState_Online_Connecting" },
            { 4, "eBusyState_Loading" },
            { 5, "eBusyState_Fetching" },
            { 6, "eBusyState_None" }
        };

        (*enum_map)["EBulletType"] = std::map<uint32_t, std::string>{
            { 0, "BULLET_TYPE_NONE" },
            { 1, "BULLET_TYPE_GUN" },
            { 2, "BULLET_TYPE_REVOLVER" },
            { 3, "BULLET_TYPE_SMG" },
            { 4, "BULLET_TYPE_MG" },
            { 5, "BULLET_TYPE_RIFLE" },
            { 6, "BULLET_TYPE_SHOTGUN" },
            { 7, "BULLET_TYPE_SNIPER" },
            { 8, "BULLET_TYPE_RPG" }
        };

        (*enum_map)["EPathFinderBoxType"] = std::map<uint32_t, std::string>{
            { 0, "PFBT_INCLUDE_MESH_COLLISION" },
            { 1, "PFBT_EXCLUDE_MESH_COLLISION" },
            { 2, "PFBT_CREATE_MESH_COLLISION" },
            { 3, "PFBT_REGION" },
            { 4, "PFBT_INCLUDE_PORTALS" },
            { 5, "PFBT_EXCLUDE_PORTALS" },
            { 6, "PFBT_IGNORE" }
        };

        (*enum_map)["ZVRHUDWristAlignmentController.EAxis"] = std::map<uint32_t, std::string>{
            { 0, "X_AXIS" },
            { 1, "Y_AXIS" },
            { 2, "Z_AXIS" }
        };

        (*enum_map)["EMotionType"] = std::map<uint32_t, std::string>{
            { 0, "MOTION_LOCKED" },
            { 1, "MOTION_LIMITED" },
            { 2, "MOTION_FREE" }
        };

        (*enum_map)["ECharacterStateTags"] = std::map<uint32_t, std::string>{
            { 1, "eSM_ST_UsingFullBody" },
            { 2, "eSM_ST_UsingRightHand" },
            { 4, "eSM_ST_UsingLeftHand" },
            { 128, "eSM_ST_UpperBodyRequestedControlLeftHand" },
            { 64, "eSM_ST_UpperBodyRequestedControlRightHand" },
            { 16384, "eSM_ST_EmptyStateRightHand" },
            { 32768, "eSM_ST_EnableOldMovement" },
            { 32, "eSM_ST_EnslavingFullBody" },
            { 8396800, "eSM_ST_EmptyStateLeftHand" },
            { 16, "eSM_ST_EnslavingLeftHand" },
            { 4096, "eSM_ST_Reloading" },
            { 8, "eSM_ST_EnslavingRightHand" },
            { 256, "eSM_ST_CanGiveUpControlRightHand" },
            { 512, "eSM_ST_CanGiveUpControlLeftHand" },
            { 1024, "eSM_ST_Pickup" },
            { 2048, "eSM_ST_Sniping" },
            { 262144, "eSM_ST_Assemble" },
            { 65536, "eSM_ST_UsingItemLeft" },
            { 131072, "eSM_ST_UsingItemRight" }
        };

        (*enum_map)["ECCDUsage"] = std::map<uint32_t, std::string>{
            { 0, "ECCDUSAGE_DISABLED" },
            { 1, "ECCDUSAGE_AGAINST_STATIC" },
            { 2, "ECCDUSAGE_AGAINST_STATIC_DYNAMIC" }
        };

        (*enum_map)["ZUIControlEntity.EAlignment"] = std::map<uint32_t, std::string>{
            { 0, "TopLeft" },
            { 1, "TopCenter" },
            { 2, "TopRight" },
            { 3, "CenterLeft" },
            { 4, "Center" },
            { 5, "CenterRight" },
            { 6, "BottomLeft" },
            { 7, "BottomCenter" },
            { 8, "BottomRight" },
            { 9, "StretchCenter" },
            { 10, "StretchHorizontalTop" },
            { 11, "StretchHorizontalCenter" },
            { 12, "StretchHorizontalBottom" },
            { 13, "StretchVerticalLeft" },
            { 14, "StretchVerticalCenter" },
            { 15, "StretchVerticalRight" }
        };

        (*enum_map)["EAmmoBehaviourConfigType"] = std::map<uint32_t, std::string>{
            { 0, "eAB_None" },
            { 1, "eAB_Explosive" },
            { 2, "eAB_Penetration" }
        };

        (*enum_map)["ZHM5BodyContainer.eBCCloseMode"] = std::map<uint32_t, std::string>{
            { 0, "BC_AUTOCLOSE" },
            { 1, "BC_MANUALCLOSE" }
        };

        (*enum_map)["EFSMStateStatus"] = std::map<uint32_t, std::string>{
            { 0, "eInactive" },
            { 1, "eActive" },
            { 2, "eCompleted" },
            { 3, "eFailed" },
            { 4, "eInterrupted" }
        };

        (*enum_map)["EHM5SoundFootstepEvent"] = std::map<uint32_t, std::string>{
            { 0, "EFSE_START" },
            { 1, "EFSE_STOP" },
            { 2, "EFSE_LEFT" },
            { 3, "EFSE_RIGHT" },
            { 4, "EFSE_TURN" },
            { 5, "EFSE_SLIDE" },
            { 6, "EFSE_JUMP" },
            { 7, "EFSE_LAND" }
        };

        (*enum_map)["ZHM5ValueEntity_bool.EForwardRule"] = std::map<uint32_t, std::string>{
            { 0, "eAlways" },
            { 1, "eValueChanged" }
        };

        (*enum_map)["Network.PacketReliability"] = std::map<uint32_t, std::string>{
            { 0, "UNRELIABLE" },
            { 1, "UNRELIABLE_SEQUENCED" },
            { 2, "RELIABLE" },
            { 3, "RELIABLE_ORDERED" },
            { 4, "RELIABLE_SEQUENCED" },
            { 5, "UNRELIABLE_WITH_ACK_RECEIPT" },
            { 6, "RELIABLE_WITH_ACK_RECEIPT" },
            { 7, "RELIABLE_ORDERED_WITH_ACK_RECEIPT" },
            { 8, "NUMBER_OF_RELIABILITIES" }
        };

        (*enum_map)["IContractObjective.State"] = std::map<uint32_t, std::string>{
            { 0, "IN_PROGRESS" },
            { 1, "COMPLETED" },
            { 2, "FAILED" }
        };

        (*enum_map)["ZActorDebugEntity.EActorDebugColor"] = std::map<uint32_t, std::string>{
            { 8323072, "EDC_DARK_BLUE" },
            { 16711680, "EDC_BLUE" },
            { 127, "EDC_DARK_RED" },
            { 255, "EDC_RED" },
            { 32512, "EDC_DARK_GREEN" },
            { 65280, "EDC_GREEN" },
            { 8323199, "EDC_DARK_PINK" },
            { 16711935, "EDC_PINK" },
            { 0, "EDC_BLACK" },
            { 16777215, "EDC_WHITE" }
        };

        (*enum_map)["ERenderVRTrackingID"] = std::map<uint32_t, std::string>{
            { 16, "RENDER_VR_TRACK_HEAD" },
            { 17, "RENDER_VR_TRACK_EYE_LEFT" },
            { 18, "RENDER_VR_TRACK_EYE_RIGHT" },
            { 19, "RENDER_VR_TRACK_HAND_LEFT" },
            { 20, "RENDER_VR_TRACK_HAND_RIGHT" }
        };

        (*enum_map)["EMoveSpeed"] = std::map<uint32_t, std::string>{
            { 0, "MS_Sneaking" },
            { 1, "MS_Walking" },
            { 2, "MS_Jogging" },
            { 3, "MS_Sprinting" },
            { 4, "MS_Flash" }
        };

        (*enum_map)["EDeathContext"] = std::map<uint32_t, std::string>{
            { 0, "eDC_UNDEFINED" },
            { 1, "eDC_NOT_HERO" },
            { 2, "eDC_HIDDEN" },
            { 3, "eDC_ACCIDENT" },
            { 4, "eDC_MURDER" }
        };

        (*enum_map)["ECoordinateSpace"] = std::map<uint32_t, std::string>{
            { 0, "CSPACE_INVALID" },
            { 1, "CSPACE_OBJECT_TO_WORLD" },
            { 2, "CSPACE_OBJECT_TO_PARENT" },
            { 3, "CSPACE_SEQUENCE_ORIGIN" },
            { 4, "CSPACE_CHARACTER_RELATIVE" }
        };

        (*enum_map)["ECameraAssistanceMode"] = std::map<uint32_t, std::string>{
            { 0, "eCAM_None" },
            { 1, "eCAM_TrackAssist" },
            { 2, "eCAM_AutoTrackTarget" }
        };

        (*enum_map)["ZUIListNavigationEntity.ENavigationInputMode"] = std::map<uint32_t, std::string>{
            { 0, "E_MODE_UP_DOWN" },
            { 1, "E_MODE_LEFT_RIGHT" },
            { 2, "E_MODE_PGPREV_PGNEXT" },
            { 3, "E_MODE_SMART_2D" }
        };

        (*enum_map)["EExtendedPropertyType"] = std::map<uint32_t, std::string>{
            { 0, "TYPE_RESOURCEPTR" },
            { 1, "TYPE_INT32" },
            { 2, "TYPE_UINT32" },
            { 3, "TYPE_FLOAT" },
            { 4, "TYPE_STRING" },
            { 5, "TYPE_BOOL" },
            { 6, "TYPE_ENTITYREF" },
            { 7, "TYPE_VARIANT" }
        };

        (*enum_map)["ERegistrationState"] = std::map<uint32_t, std::string>{
            { 0, "eINITIAL" },
            { 1, "eNOP" },
            { 2, "ePENDING" },
            { 3, "eCANCELLED_REMOTELY" },
            { 4, "eCANCELLED_LOCALLY" },
            { 5, "eFOUNDMATCH" }
        };

        (*enum_map)["EDisconnectReason"] = std::map<uint32_t, std::string>{
            { 0, "eNOTIFICATION" },
            { 1, "eCONNECTION_LOST" },
            { 2, "eNETWORK_STATE" }
        };

        (*enum_map)["EIntelStage"] = std::map<uint32_t, std::string>{
            { 0, "eIT_MAIN" },
            { 1, "eIT_STAGE" },
            { 2, "eIT_HINT" }
        };

        (*enum_map)["EActorVoiceVariation"] = std::map<uint32_t, std::string>{
            { 399, "eAVV_JOB_STINGLIFEGRDM06" },
            { 0, "eAVV_Undefined" },
            { 1, "eAVV_ABIATTI" },
            { 393, "eAVV_JOB_SITEWKR07" },
            { 2, "eAVV_ATHENASAVALAS" },
            { 3, "eAVV_BDYGRD01" },
            { 387, "eAVV_JOB_RANGANSECHI03" },
            { 4, "eAVV_BDYGRD02" },
            { 5, "eAVV_BDYGRD03" },
            { 397, "eAVV_JOB_SITEWKRES02" },
            { 6, "eAVV_BDYGRD04" },
            { 7, "eAVV_BDYGRD05" },
            { 407, "eAVV_JOB_STINGTECHM07" },
            { 8, "eAVV_BDYGRD06" },
            { 9, "eAVV_BDYGRD07" },
            { 401, "eAVV_JOB_STINGSEC06" },
            { 10, "eAVV_BERG" },
            { 11, "eAVV_BULLDOG_MANSIONSTAFF_F05" },
            { 395, "eAVV_JOB_SITEWKR09" },
            { 12, "eAVV_BULLDOG_MANSIONSTAFF_F08" },
            { 13, "eAVV_BULLDOG_MANSIONSTAFF_F09" },
            { 405, "eAVV_JOB_STINGSTAFFM08" },
            { 14, "eAVV_BULLDOG_MANSIONSTAFF_F10" },
            { 15, "eAVV_BULLDOG_MANSIONSTAFF_M06" },
            { 415, "eAVV_JOB_SUBWKR06" },
            { 16, "eAVV_BULLDOG_MANSIONSTAFF_M07" },
            { 17, "eAVV_BULLDOG_MANSIONSTAFF_M08" },
            { 409, "eAVV_JOB_STINGTRAINERF06" },
            { 18, "eAVV_BULLDOG_MANSIONSTAFF_M09" },
            { 19, "eAVV_BULLDOG_MANSIONSTAFF_M11" },
            { 403, "eAVV_JOB_STINGSTAFFF08" },
            { 20, "eAVV_BUSEY" },
            { 21, "eAVV_CARLISLEALEXA" },
            { 413, "eAVV_JOB_STINGWTRM07" },
            { 22, "eAVV_CARLISLEEDWARD" },
            { 23, "eAVV_CARLISLEEMMA" },
            { 487, "eAVV_SCIENTIST01" },
            { 24, "eAVV_CARLISLEGREGORY" },
            { 25, "eAVV_CARLISLEPATRICK" },
            { 481, "eAVV_REYNARD" },
            { 26, "eAVV_CARLISLEREBECCA" },
            { 27, "eAVV_CARUSO" },
            { 411, "eAVV_JOB_STINGVILLAGRD06" },
            { 28, "eAVV_CASSIDY" },
            { 29, "eAVV_CHEF01" },
            { 485, "eAVV_ROYCE" },
            { 30, "eAVV_CHEF02" },
            { 31, "eAVV_CHEF03" },
            { 431, "eAVV_LLAMA_WAITER_M06" },
            { 32, "eAVV_CHEF04" },
            { 33, "eAVV_CHURCHSTAFF01" },
            { 425, "eAVV_KNOX_S" },
            { 34, "eAVV_CHURCHSTAFF02" },
            { 35, "eAVV_CHURCHSTAFF03" },
            { 419, "eAVV_JOB_THUGHI02" },
            { 36, "eAVV_CIVFEM01" },
            { 37, "eAVV_CIVFEM02" },
            { 429, "eAVV_LLAMA_WAITER_F06" },
            { 38, "eAVV_CIVFEM03" },
            { 39, "eAVV_CIVFEM04" },
            { 439, "eAVV_LLAMA_WORKER_MES03" },
            { 40, "eAVV_CIVFEM05" },
            { 41, "eAVV_CIVFEM06" },
            { 433, "eAVV_LLAMA_WAITER_M11" },
            { 42, "eAVV_CIVFEM07" },
            { 43, "eAVV_CIVFEM08" },
            { 427, "eAVV_LJUDMILAVETROVA" },
            { 44, "eAVV_CIVFEM09" },
            { 45, "eAVV_CIVFEM10" },
            { 437, "eAVV_LLAMA_WORKER_MES01" },
            { 46, "eAVV_CIVFEMCH01" },
            { 47, "eAVV_CIVFEMCH02" },
            { 447, "eAVV_MECH02" },
            { 48, "eAVV_CIVFEMES01" },
            { 49, "eAVV_CIVFEMES02" },
            { 441, "eAVV_MAMBACREW01" },
            { 50, "eAVV_CIVFEMES03" },
            { 51, "eAVV_CIVFEMHI01" },
            { 435, "eAVV_LLAMA_WORKER_FES02" },
            { 52, "eAVV_CIVFEMHI02" },
            { 53, "eAVV_CIVFEMHI03" },
            { 445, "eAVV_MARTINEZ" },
            { 54, "eAVV_CIVMALE01" },
            { 55, "eAVV_CIVMALE02" },
            { 391, "eAVV_JOB_SERVANTFEM07" },
            { 56, "eAVV_CIVMALE03" },
            { 57, "eAVV_CIVMALE04" },
            { 385, "eAVV_JOB_RANGANSECHI01" },
            { 58, "eAVV_CIVMALE05" },
            { 59, "eAVV_CIVMALE06" },
            { 443, "eAVV_MAMBACREW03" },
            { 60, "eAVV_CIVMALE07" },
            { 61, "eAVV_CIVMALE08" },
            { 389, "eAVV_JOB_SERVANT09" },
            { 62, "eAVV_CIVMALE09" },
            { 63, "eAVV_CIVMALE10" },
            { 463, "eAVV_RAT_DUMPLINGCOOK_FCH02" },
            { 64, "eAVV_CIVMALE11" },
            { 65, "eAVV_CIVMALECH01" },
            { 457, "eAVV_NORFOLK" },
            { 66, "eAVV_CIVMALECH02" },
            { 67, "eAVV_CIVMALEES01" },
            { 451, "eAVV_MODEL02" },
            { 68, "eAVV_CIVMALEES02" },
            { 69, "eAVV_CIVMALEES03" },
            { 461, "eAVV_RAT_DUMPLINGCOOK_F05" },
            { 70, "eAVV_CIVMALEHI01" },
            { 71, "eAVV_CIVMALEHI02" },
            { 471, "eAVV_RAT_FACILITYSTAFF_F09" },
            { 72, "eAVV_CIVMALEHI03" },
            { 73, "eAVV_CLEANER01" },
            { 465, "eAVV_RAT_DUMPLINGCOOK_M07" },
            { 74, "eAVV_CLEANER02" },
            { 75, "eAVV_CLEANER03" },
            { 459, "eAVV_ORSON" },
            { 76, "eAVV_CLEANERFEM01" },
            { 77, "eAVV_CLEANERFEM02" },
            { 469, "eAVV_RAT_DUMPLINGCOOK_MCH02" },
            { 78, "eAVV_CLEANERFEM03" },
            { 79, "eAVV_CONSTANT" },
            { 479, "eAVV_PRIEST01" },
            { 80, "eAVV_CREW01" },
            { 81, "eAVV_CREW02" },
            { 473, "eAVV_RAT_FACILITYSTAFF_FCH02" },
            { 82, "eAVV_CREW04" },
            { 83, "eAVV_CROSS" },
            { 467, "eAVV_RAT_DUMPLINGCOOK_M09" },
            { 84, "eAVV_DALIA" },
            { 85, "eAVV_DELGADO" },
            { 477, "eAVV_RAT_FACILITYSTAFF_M09" },
            { 86, "eAVV_DESANTIS" },
            { 87, "eAVV_DINO_BOSCO" },
            { 88, "eAVV_DOCTOR01" },
            { 89, "eAVV_DOCTOR06" },
            { 90, "eAVV_DOCTORFEM03" },
            { 91, "eAVV_DOCTORFEM04" },
            { 475, "eAVV_RAT_FACILITYSTAFF_M07" },
            { 92, "eAVV_ET_ADONIS" },
            { 93, "eAVV_ET_ARGUS" },
            { 94, "eAVV_ET_CARDINAL" },
            { 95, "eAVV_ET_CONNER" },
            { 495, "eAVV_SHAH" },
            { 96, "eAVV_ET_DYLAN" },
            { 97, "eAVV_ET_FABA" },
            { 489, "eAVV_SCIENTIST03" },
            { 98, "eAVV_ET_GARY_LUNN" },
            { 99, "eAVV_ET_HARVERFOEK" },
            { 483, "eAVV_ROBERT_KNOX" },
            { 100, "eAVV_ET_LARIN" },
            { 101, "eAVV_ET_MOXON" },
            { 493, "eAVV_SCIENTISTFEM03" },
            { 102, "eAVV_ET_MR_GIGGLES" },
            { 103, "eAVV_ET_PHILLIPOS" },
            { 503, "eAVV_SOFTGRD03" },
            { 104, "eAVV_ET_SCOTT_SARNO" },
            { 105, "eAVV_ET_TORVIK" },
            { 497, "eAVV_SHEEPGRD05" },
            { 106, "eAVV_ET_TROUTT" },
            { 107, "eAVV_ET_VITO_DURIC" },
            { 491, "eAVV_SCIENTISTFEM01" },
            { 108, "eAVV_FILMCREW01" },
            { 109, "eAVV_FILMCREW02" },
            { 501, "eAVV_SOFTGRD01" },
            { 110, "eAVV_FILMCREW03" },
            { 111, "eAVV_FILMCREW04" },
            { 511, "eAVV_SPECOPS02" },
            { 112, "eAVV_FILMCREWFEM01" },
            { 113, "eAVV_FILMCREWFEM02" },
            { 505, "eAVV_SOLDIER02" },
            { 114, "eAVV_FILMCREWFEM03" },
            { 115, "eAVV_FILMCREWFEM04" },
            { 499, "eAVV_SHEEPGRD07" },
            { 116, "eAVV_FILMCREWFEM05" },
            { 117, "eAVV_FOD_FEM" },
            { 509, "eAVV_SOLDIERBOSS03" },
            { 118, "eAVV_FOD_MALE" },
            { 119, "eAVV_FOX_BARTENDER_F08" },
            { 455, "eAVV_MODELFEM03" },
            { 120, "eAVV_FOX_BARTENDER_F10" },
            { 121, "eAVV_FOX_BARTENDER_M07" },
            { 449, "eAVV_MENDOLA" },
            { 122, "eAVV_FOX_BARTENDER_M08" },
            { 123, "eAVV_FOX_BARTENDER_M09" },
            { 507, "eAVV_SOLDIERBOSS01" },
            { 124, "eAVV_FOX_BARTENDER_MCH01" },
            { 125, "eAVV_FOX_DJ_M07" },
            { 453, "eAVV_MODELFEM01" },
            { 126, "eAVV_FOX_DJ_M11" },
            { 127, "eAVV_FOX_ICAAGENT_M01" },
            { 128, "eAVV_FOX_ICAAGENT_M02" },
            { 314, "eAVV_JOB_FARMFES02" },
            { 129, "eAVV_FOX_ICAAGENT_M04" },
            { 130, "eAVV_FOX_ICAAGENT_M05" },
            { 260, "eAVV_JOB_CASTLESTFEM07" },
            { 131, "eAVV_FOX_ICAAGENT_M06" },
            { 132, "eAVV_FOX_ICAAGENT_M07" },
            { 318, "eAVV_JOB_FARMMES02" },
            { 133, "eAVV_FOX_STAFF_F05" },
            { 134, "eAVV_FOX_STAFF_F06" },
            { 312, "eAVV_JOB_FARMF05" },
            { 135, "eAVV_FOX_STAFF_FCH01" },
            { 136, "eAVV_FOX_STAFF_M07" },
            { 258, "eAVV_JOB_CASSIDYGRD07" },
            { 137, "eAVV_FOX_STAFF_M08" },
            { 138, "eAVV_FOX_STAFF_M09" },
            { 268, "eAVV_JOB_CAVEWKRES02" },
            { 139, "eAVV_FOX_TECH_F05" },
            { 140, "eAVV_FOX_TECH_F06" },
            { 262, "eAVV_JOB_CASTLESTFF10" },
            { 141, "eAVV_FOX_TECH_F08" },
            { 142, "eAVV_FOX_TECH_FCH01" },
            { 256, "eAVV_JOB_CASSIDYGRD05" },
            { 143, "eAVV_FOX_TECH_M06" },
            { 144, "eAVV_FOX_TECH_M07" },
            { 266, "eAVV_JOB_CAVEWKR08" },
            { 145, "eAVV_FOX_TECH_M08" },
            { 146, "eAVV_FOX_TECH_M11" },
            { 276, "eAVV_JOB_CHEFM09" },
            { 147, "eAVV_FOX_THUG_M04" },
            { 148, "eAVV_FOX_THUG_M05" },
            { 270, "eAVV_JOB_CHEFES01" },
            { 149, "eAVV_FOX_THUG_M06" },
            { 150, "eAVV_FOX_THUG_M07" },
            { 264, "eAVV_JOB_CAVEGDES02" },
            { 151, "eAVV_FRANCO" },
            { 152, "eAVV_GARDENER01" },
            { 274, "eAVV_JOB_CHEFM06" },
            { 153, "eAVV_GARDENER02" },
            { 154, "eAVV_GARDENER03" },
            { 284, "eAVV_JOB_CNSTR09" },
            { 155, "eAVV_GARDENERFEM01" },
            { 156, "eAVV_GARDENERFEM02" },
            { 278, "eAVV_JOB_CIVGRD05" },
            { 157, "eAVV_GARDENERFEM03" },
            { 158, "eAVV_GECKO_ARTTECH_F05" },
            { 272, "eAVV_JOB_CHEFES03" },
            { 159, "eAVV_GECKO_ARTTECH_F06" },
            { 160, "eAVV_GECKO_ARTTECH_F08" },
            { 161, "eAVV_GECKO_ARTTECH_F10" },
            { 162, "eAVV_GECKO_ARTTECH_M05" },
            { 292, "eAVV_JOB_COP04" },
            { 163, "eAVV_GECKO_ARTTECH_M06" },
            { 164, "eAVV_GECKO_ARTTECH_M08" },
            { 165, "eAVV_GECKO_ARTTECH_M09" },
            { 166, "eAVV_GECKO_ATTENDANT_M09" },
            { 167, "eAVV_GECKO_ATTENDANT_MHI03" },
            { 168, "eAVV_GECKO_MAINTENANCE_FCH01" },
            { 290, "eAVV_JOB_COCAGRD06" },
            { 169, "eAVV_GECKO_MAINTENANCE_M05" },
            { 170, "eAVV_GECKO_WAITER_F05" },
            { 300, "eAVV_JOB_CUSTDNF05" },
            { 171, "eAVV_GECKO_WAITER_F06" },
            { 172, "eAVV_GECKO_WAITER_F09" },
            { 294, "eAVV_JOB_COP06" },
            { 173, "eAVV_GECKO_WAITER_F10" },
            { 174, "eAVV_GECKO_WAITER_FHI02" },
            { 288, "eAVV_JOB_CNSTRGRD06" },
            { 175, "eAVV_GECKO_WAITER_M07" },
            { 176, "eAVV_GECKO_WAITER_M08" },
            { 298, "eAVV_JOB_COUNSLRM07" },
            { 177, "eAVV_GECKO_WAITER_M09" },
            { 178, "eAVV_GECKO_WAITER_M11" },
            { 308, "eAVV_JOB_CUSTDNM09" },
            { 179, "eAVV_GECKO_WAITER_MHI03" },
            { 180, "eAVV_GRAVES" },
            { 302, "eAVV_JOB_CUSTDNF07" },
            { 181, "eAVV_GUARDIA02" },
            { 182, "eAVV_GUARDIA03" },
            { 296, "eAVV_JOB_COUNSLRF08" },
            { 183, "eAVV_HOUSSTFF01" },
            { 184, "eAVV_HOUSSTFF02" },
            { 306, "eAVV_JOB_CUSTDNM07" },
            { 185, "eAVV_HOUSSTFF03" },
            { 186, "eAVV_HOUSSTFF06" },
            { 316, "eAVV_JOB_FARMM08" },
            { 187, "eAVV_HOUSSTFFEM01" },
            { 188, "eAVV_HOUSSTFFEM02" },
            { 310, "eAVV_JOB_DRUGLABWKRES02" },
            { 189, "eAVV_HOUSSTFFEM03" },
            { 190, "eAVV_HUSH" },
            { 304, "eAVV_JOB_CUSTDNF09" },
            { 191, "eAVV_ICAAGENT_M05" },
            { 192, "eAVV_INGRAM" },
            { 378, "eAVV_JOB_QUEENTGHI02" },
            { 193, "eAVV_INTERN" },
            { 194, "eAVV_JANUS" },
            { 324, "eAVV_JOB_GARBAGRD05" },
            { 195, "eAVV_JOB_ACTORFEMHI02" },
            { 196, "eAVV_JOB_ACTORHI01" },
            { 382, "eAVV_JOB_RAIDER07" },
            { 197, "eAVV_JOB_ARCHTCTF05" },
            { 198, "eAVV_JOB_ARCHTCTF07" },
            { 376, "eAVV_JOB_QUEENGRDHI03" },
            { 199, "eAVV_JOB_ARCHTCTF08" },
            { 200, "eAVV_JOB_ARCHTCTF09" },
            { 322, "eAVV_JOB_GARBAGEES01" },
            { 201, "eAVV_JOB_ARCHTCTM06" },
            { 202, "eAVV_JOB_ARCHTCTM07" },
            { 332, "eAVV_JOB_GRDNRGRD06" },
            { 203, "eAVV_JOB_ARCHTCTM08" },
            { 204, "eAVV_JOB_ARCHTCTM09" },
            { 326, "eAVV_JOB_GARBAGRD07" },
            { 205, "eAVV_JOB_ARCHTCTM10" },
            { 206, "eAVV_JOB_ARKELITE04" },
            { 320, "eAVV_JOB_GARBAGE08" },
            { 207, "eAVV_JOB_ARKELITE05" },
            { 208, "eAVV_JOB_ARKELITE06" },
            { 330, "eAVV_JOB_GRDNRGRD04" },
            { 209, "eAVV_JOB_ARKELITE07" },
            { 210, "eAVV_JOB_ARKIANF05" },
            { 340, "eAVV_JOB_HSSTFMES02" },
            { 549, "eAVV_WASHINGTON" },
            { 211, "eAVV_JOB_ARKIANF06" },
            { 212, "eAVV_JOB_ARKIANF07" },
            { 334, "eAVV_JOB_GUARD04" },
            { 213, "eAVV_JOB_ARKIANF08" },
            { 214, "eAVV_JOB_ARKIANF09" },
            { 328, "eAVV_JOB_GRDNR09" },
            { 215, "eAVV_JOB_ARKIANM06" },
            { 216, "eAVV_JOB_ARKIANM07" },
            { 338, "eAVV_JOB_HSSTFFES02" },
            { 217, "eAVV_JOB_ARKIANM08" },
            { 218, "eAVV_JOB_ARKIANM09" },
            { 348, "eAVV_JOB_LNDRYGRDHI03" },
            { 557, "eAVV_YAMASAKI" },
            { 219, "eAVV_JOB_ARKIANM10" },
            { 220, "eAVV_JOB_ARKPTRNF05" },
            { 342, "eAVV_JOB_JANUSGRD04" },
            { 221, "eAVV_JOB_ARKPTRNF06" },
            { 222, "eAVV_JOB_ARKPTRNF07" },
            { 336, "eAVV_JOB_GUARD06" },
            { 545, "eAVV_WAITERFEM01" },
            { 223, "eAVV_JOB_ARKPTRNF08" },
            { 224, "eAVV_JOB_ARKPTRNF09" },
            { 282, "eAVV_JOB_CLOTHSALHI02" },
            { 225, "eAVV_JOB_ARKPTRNM06" },
            { 226, "eAVV_JOB_ARKPTRNM07" },
            { 356, "eAVV_JOB_MEDICFEM06" },
            { 227, "eAVV_JOB_ARKPTRNM08" },
            { 228, "eAVV_JOB_ARKPTRNM09" },
            { 286, "eAVV_JOB_CNSTRGRD04" },
            { 229, "eAVV_JOB_ARKPTRNM10" },
            { 230, "eAVV_JOB_ARKSTAFFF05" },
            { 280, "eAVV_JOB_CIVGRD07" },
            { 231, "eAVV_JOB_ARKSTAFFF06" },
            { 232, "eAVV_JOB_ARKSTAFFM06" },
            { 354, "eAVV_JOB_MANSIONGD07" },
            { 233, "eAVV_JOB_ARKSTAFFM07" },
            { 234, "eAVV_JOB_ARKSTAFFM08" },
            { 364, "eAVV_JOB_MUMBSRVHI03" },
            { 235, "eAVV_JOB_BANKERF05" },
            { 236, "eAVV_JOB_BANKERF06" },
            { 358, "eAVV_JOB_MTLWKRHI02" },
            { 237, "eAVV_JOB_BANKERF08" },
            { 238, "eAVV_JOB_BANKERM06" },
            { 352, "eAVV_JOB_MANSIONGD05" },
            { 239, "eAVV_JOB_BANKERM07" },
            { 240, "eAVV_JOB_BANKERM08" },
            { 362, "eAVV_JOB_MUMBSRVHI01" },
            { 241, "eAVV_JOB_BANKERM09" },
            { 242, "eAVV_JOB_BANKGRD05" },
            { 372, "eAVV_JOB_NITIATEM06" },
            { 243, "eAVV_JOB_BANKGRD06" },
            { 244, "eAVV_JOB_BANKGRD07" },
            { 366, "eAVV_JOB_MUSICIANM06" },
            { 245, "eAVV_JOB_BANKGRDCHF" },
            { 246, "eAVV_JOB_BANKIT07" },
            { 360, "eAVV_JOB_MUMBSECHI02" },
            { 247, "eAVV_JOB_BANKMTNC08" },
            { 248, "eAVV_JOB_BBQGRD04" },
            { 370, "eAVV_JOB_MUSICIANM10" },
            { 249, "eAVV_JOB_BBQGRD05" },
            { 250, "eAVV_JOB_BBQGRD06" },
            { 380, "eAVV_JOB_RAIDER05" },
            { 251, "eAVV_JOB_BBQGRD07" },
            { 252, "eAVV_JOB_BOLLYCREWHI01" },
            { 374, "eAVV_JOB_QUEENGRDHI01" },
            { 253, "eAVV_JOB_BOLLYCREWHI02" },
            { 254, "eAVV_JOB_BOLLYCREWHI03" },
            { 368, "eAVV_JOB_MUSICIANM08" },
            { 255, "eAVV_JOB_CASSIDYGRD04" },
            { 257, "eAVV_JOB_CASSIDYGRD06" },
            { 259, "eAVV_JOB_CASTLESTFEM05" },
            { 261, "eAVV_JOB_CASTLESTFF07" },
            { 263, "eAVV_JOB_CAVEGDES01" },
            { 265, "eAVV_JOB_CAVEWKR07" },
            { 267, "eAVV_JOB_CAVEWKRES01" },
            { 269, "eAVV_JOB_CAVEWKRES03" },
            { 271, "eAVV_JOB_CHEFES02" },
            { 273, "eAVV_JOB_CHEFF07" },
            { 275, "eAVV_JOB_CHEFM07" },
            { 277, "eAVV_JOB_CIVGRD04" },
            { 279, "eAVV_JOB_CIVGRD06" },
            { 281, "eAVV_JOB_CLOTHSALHI01" },
            { 283, "eAVV_JOB_CNSTR08" },
            { 285, "eAVV_JOB_CNSTRES01" },
            { 287, "eAVV_JOB_CNSTRGRD05" },
            { 289, "eAVV_JOB_CNSTRGRD07" },
            { 291, "eAVV_JOB_COCAGRDES02" },
            { 293, "eAVV_JOB_COP05" },
            { 295, "eAVV_JOB_COP07" },
            { 297, "eAVV_JOB_COUNSLRF09" },
            { 299, "eAVV_JOB_COUNSLRM09" },
            { 301, "eAVV_JOB_CUSTDNF06" },
            { 303, "eAVV_JOB_CUSTDNF08" },
            { 305, "eAVV_JOB_CUSTDNM06" },
            { 307, "eAVV_JOB_CUSTDNM08" },
            { 309, "eAVV_JOB_CUSTDNM10" },
            { 311, "eAVV_JOB_DRUGLABWKRES03" },
            { 313, "eAVV_JOB_FARMFES01" },
            { 315, "eAVV_JOB_FARMM06" },
            { 317, "eAVV_JOB_FARMMES01" },
            { 319, "eAVV_JOB_FARMMES03" },
            { 321, "eAVV_JOB_GARBAGE09" },
            { 323, "eAVV_JOB_GARBAGRD04" },
            { 325, "eAVV_JOB_GARBAGRD06" },
            { 327, "eAVV_JOB_GRDNR08" },
            { 329, "eAVV_JOB_GRDNRES01" },
            { 331, "eAVV_JOB_GRDNRGRD05" },
            { 333, "eAVV_JOB_GRDNRGRD07" },
            { 335, "eAVV_JOB_GUARD05" },
            { 337, "eAVV_JOB_GUARD07" },
            { 339, "eAVV_JOB_HSSTFFES03" },
            { 341, "eAVV_JOB_HSSTFMES03" },
            { 343, "eAVV_JOB_JANUSGRD05" },
            { 344, "eAVV_JOB_JANUSGRD06" },
            { 553, "eAVV_YACHTCREW01" },
            { 345, "eAVV_JOB_JANUSGRD07" },
            { 346, "eAVV_JOB_LNDRYGRDHI01" },
            { 347, "eAVV_JOB_LNDRYGRDHI02" },
            { 349, "eAVV_JOB_LNDRYWKRHI01" },
            { 350, "eAVV_JOB_LNDRYWKRHI02" },
            { 351, "eAVV_JOB_LNDRYWKRHI03" },
            { 353, "eAVV_JOB_MANSIONGD06" },
            { 355, "eAVV_JOB_MANSIONGDES02" },
            { 357, "eAVV_JOB_MTLWKRHI01" },
            { 359, "eAVV_JOB_MTLWKRHI03" },
            { 361, "eAVV_JOB_MUMBSECHI03" },
            { 363, "eAVV_JOB_MUMBSRVHI02" },
            { 365, "eAVV_JOB_MUSICIANF05" },
            { 367, "eAVV_JOB_MUSICIANM07" },
            { 369, "eAVV_JOB_MUSICIANM09" },
            { 371, "eAVV_JOB_NITIATEF05" },
            { 373, "eAVV_JOB_NITIATEM07" },
            { 375, "eAVV_JOB_QUEENGRDHI02" },
            { 377, "eAVV_JOB_QUEENSTGHI03" },
            { 379, "eAVV_JOB_RAIDER04" },
            { 381, "eAVV_JOB_RAIDER06" },
            { 383, "eAVV_JOB_RANGANGRD05" },
            { 384, "eAVV_JOB_RANGANGRD06" },
            { 529, "eAVV_STYLISTFEM01" },
            { 386, "eAVV_JOB_RANGANSECHI02" },
            { 388, "eAVV_JOB_SERVANT08" },
            { 533, "eAVV_THUGES01" },
            { 390, "eAVV_JOB_SERVANTES02" },
            { 392, "eAVV_JOB_SITEWKR06" },
            { 537, "eAVV_THUGHI02" },
            { 394, "eAVV_JOB_SITEWKR08" },
            { 396, "eAVV_JOB_SITEWKRES01" },
            { 541, "eAVV_WAITER01" },
            { 398, "eAVV_JOB_SITEWKRES03" },
            { 400, "eAVV_JOB_STINGSEC05" },
            { 402, "eAVV_JOB_STINGSMASSF09" },
            { 404, "eAVV_JOB_STINGSTAFFM06" },
            { 406, "eAVV_JOB_STINGTECHF08" },
            { 408, "eAVV_JOB_STINGTECHM09" },
            { 410, "eAVV_JOB_STINGVILLAGRD05" },
            { 412, "eAVV_JOB_STINGWTRF05" },
            { 414, "eAVV_JOB_STINGWTRM08" },
            { 416, "eAVV_JOB_SUBWKR07" },
            { 417, "eAVV_JOB_TEASERVHI02" },
            { 418, "eAVV_JOB_THUGHI01" },
            { 420, "eAVV_JOB_THUGHI03" },
            { 421, "eAVV_JOB_TRAINSERVHI01" },
            { 422, "eAVV_JOB_VILLAGEGDES01" },
            { 423, "eAVV_JOB_VILLAGEGDES03" },
            { 424, "eAVV_KNOX_R" },
            { 426, "eAVV_KONG" },
            { 428, "eAVV_LLAMA_WAITER_F05" },
            { 430, "eAVV_LLAMA_WAITER_F10" },
            { 432, "eAVV_LLAMA_WAITER_M07" },
            { 513, "eAVV_STAFFEM03" },
            { 434, "eAVV_LLAMA_WORKER_FES01" },
            { 436, "eAVV_LLAMA_WORKER_FES03" },
            { 517, "eAVV_STAFFMALE02" },
            { 438, "eAVV_LLAMA_WORKER_MES02" },
            { 440, "eAVV_MAELSTROM" },
            { 521, "eAVV_STAFFMALE06" },
            { 442, "eAVV_MAMBACREW02" },
            { 444, "eAVV_MAMBACREW04" },
            { 525, "eAVV_STYLIST01" },
            { 446, "eAVV_MECH01" },
            { 448, "eAVV_MECH03" },
            { 450, "eAVV_MODEL01" },
            { 452, "eAVV_MODEL03" },
            { 454, "eAVV_MODELFEM02" },
            { 456, "eAVV_MORGAN" },
            { 458, "eAVV_NOVIKOV" },
            { 460, "eAVV_PARVATI" },
            { 462, "eAVV_RAT_DUMPLINGCOOK_F06" },
            { 464, "eAVV_RAT_DUMPLINGCOOK_M06" },
            { 466, "eAVV_RAT_DUMPLINGCOOK_M08" },
            { 468, "eAVV_RAT_DUMPLINGCOOK_MCH01" },
            { 470, "eAVV_RAT_FACILITYSTAFF_F06" },
            { 472, "eAVV_RAT_FACILITYSTAFF_FCH01" },
            { 474, "eAVV_RAT_FACILITYSTAFF_M06" },
            { 476, "eAVV_RAT_FACILITYSTAFF_M08" },
            { 478, "eAVV_RAT_FACILITYSTAFF_M11" },
            { 480, "eAVV_RANGAN" },
            { 482, "eAVV_RITTER" },
            { 484, "eAVV_ROSE" },
            { 486, "eAVV_SANTA" },
            { 488, "eAVV_SCIENTIST02" },
            { 490, "eAVV_SCIENTIST04" },
            { 492, "eAVV_SCIENTISTFEM02" },
            { 494, "eAVV_SCIENTISTFEM04" },
            { 496, "eAVV_SHEEPGRD04" },
            { 498, "eAVV_SHEEPGRD06" },
            { 500, "eAVV_SIERRA_KNOX" },
            { 502, "eAVV_SOFTGRD02" },
            { 504, "eAVV_SOLDIER01" },
            { 506, "eAVV_SOLDIER03" },
            { 508, "eAVV_SOLDIERBOSS02" },
            { 510, "eAVV_SPECOPS01" },
            { 512, "eAVV_SPECOPS03" },
            { 514, "eAVV_STAFFEM04" },
            { 515, "eAVV_STAFFEM05" },
            { 516, "eAVV_STAFFMALE01" },
            { 518, "eAVV_STAFFMALE03" },
            { 519, "eAVV_STAFFMALE04" },
            { 520, "eAVV_STAFFMALE05" },
            { 522, "eAVV_STEVENBRADLEY" },
            { 523, "eAVV_STRANDBERG" },
            { 524, "eAVV_STUYVESANT" },
            { 526, "eAVV_STYLIST02" },
            { 527, "eAVV_STYLIST03" },
            { 528, "eAVV_STYLIST04" },
            { 530, "eAVV_STYLISTFEM02" },
            { 531, "eAVV_STYLISTFEM03" },
            { 532, "eAVV_STYLISTFEM04" },
            { 534, "eAVV_THUGES02" },
            { 535, "eAVV_THUGES03" },
            { 536, "eAVV_THUGHI01" },
            { 538, "eAVV_THUGHI03" },
            { 539, "eAVV_TYSONWILLIAMS" },
            { 540, "eAVV_VIDAL" },
            { 542, "eAVV_WAITER02" },
            { 543, "eAVV_WAITER03" },
            { 544, "eAVV_WAITER04" },
            { 546, "eAVV_WAITERFEM02" },
            { 547, "eAVV_WAITERFEM03" },
            { 548, "eAVV_WAITERFEM04" },
            { 550, "eAVV_WS_BOULTON" },
            { 551, "eAVV_WS_NEWMAN" },
            { 552, "eAVV_WS_WATSON" },
            { 554, "eAVV_YACHTCREW02" },
            { 555, "eAVV_YACHTCREW03" },
            { 556, "eAVV_YACHTCREW04" },
            { 558, "eAVV_YATES" },
            { 559, "eAVV_ZAYDAN" }
        };

        (*enum_map)["ECharacterAnimChildNetworkSlot"] = std::map<uint32_t, std::string>{
            { 0, "eCACNS_None" },
            { 1, "eCACNS_FlavorIdle_FB" },
            { 2, "eCACNS_FlavorIdle_RH" },
            { 3, "eCACNS_FlavorIdle_LH" },
            { 4, "eCACNS_Interaction_RH" },
            { 5, "eCACNS_Interaction_LH" }
        };

        (*enum_map)["EStealthSituation"] = std::map<uint32_t, std::string>{
            { 0, "SS_DISGUISE_TAKEN" },
            { 1, "SS_DISGUISE_TRESPASSING" },
            { 2, "SS_DISGUISE_ILLEGAL_ACTION" },
            { 3, "SS_DISGUISE_REMOVED" },
            { 4, "SS_DISGUISE_INVESTIGATED" },
            { 5, "SS_DISGUISE_BLOWN" },
            { 6, "SS_HIDE_IN_CLOSET_FIRSTTIME" },
            { 7, "SS_HIDE_IN_CLOSET" },
            { 8, "SS_DOOR_START_LOCKPICKING" },
            { 9, "SS_DOOR_CANCEL_LOCKPICKING" },
            { 10, "SS_LANDMINE_ARMED" },
            { 11, "SS_RADIO_ON" },
            { 12, "SS_VENT_ENTER" },
            { 13, "SS_VENT_EXIT" },
            { 14, "SS_BLEND_IN_ACTIVATED" },
            { 15, "SS_BODY_DISCOVERED" },
            { 16, "SS_INVESTIGATE_GUNSHOT" },
            { 17, "SS_INVESTIGATE_NOISE" },
            { 18, "SS_INVESTIGATE_EXPLOSION" },
            { 19, "SS_FUSEBOX_DISABLE_UNIQUE" },
            { 20, "SS_FUSEBOX_DISABLE" },
            { 21, "SS_INVESTIGATING_FUSEBOX" },
            { 22, "SS_FUSEBOX_FIXED" },
            { 23, "SS_KEYCARD_PICKUP" },
            { 24, "SS_KEYCARD_USE" },
            { 25, "SS_BODY_PUT_IN_CONTAINER" },
            { 26, "SS_BODY_DRAGGING_SEEN" },
            { 27, "SS_BODY_OVER_RAIL" },
            { 28, "SS_BODY_OVER_LEDGE" },
            { 29, "SS_USE_HEALTH_STATION" },
            { 30, "SS_HITMAN_SPOTTED" },
            { 31, "SS_HITMAN_HIDDEN" },
            { 32, "SS_HITMAN_ENTERED_COMBAT" },
            { 33, "SS_TRESPASSING" },
            { 34, "SS_SECURITYTAPE_PICKUP" },
            { 35, "SS_NEWOBJECTIVE" },
            { 36, "SS_FOCUSADDED" },
            { 37, "SS_FOCUSFULL" },
            { 38, "SS_FOCUSEMPTY" },
            { 39, "SS_STARTED_SPRINTING" },
            { 40, "SS_STARTED_SNEAKING" },
            { 41, "SS_TOOK_COVER" },
            { 42, "SS_INVESTIGATE_CURIOUS" },
            { 43, "SS_GENERIC_PICKUP" },
            { 44, "SS_ATTENTION_ON" },
            { 45, "SS_ATTENTION_OFF" },
            { 46, "SS_DEADBODY_SEEN" },
            { 47, "SS_TARGET_EVACUATES" },
            { 48, "SS_UNKNOWN_BODY_SPOTTED" }
        };

        (*enum_map)["ZHeroKeywordCondition.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" },
            { 3, "ANY_TRUE_IF_NO_REQS" }
        };

        (*enum_map)["EAudioVolumetricMixingMode"] = std::map<uint32_t, std::string>{
            { 0, "AUDIO_VOLUMETRIC_MIXING_MAX_ALL" },
            { 1, "AUDIO_VOLUMETRIC_MIXING_MAX_3D" },
            { 2, "AUDIO_VOLUMETRIC_MIXING_ADD" }
        };

        (*enum_map)["ZActorBoneAttachEntity.EResetMode"] = std::map<uint32_t, std::string>{
            { 0, "eAtBoneOrigin" },
            { 1, "eKeepOffset" }
        };

        (*enum_map)["IHM5Door.ECPDoorLockType"] = std::map<uint32_t, std::string>{
            { 0, "CPDLT_NORMAL_LOCK" },
            { 1, "CPDLT_PUSH_BAR_PAD_LOCK" }
        };

        (*enum_map)["EActorRank"] = std::map<uint32_t, std::string>{
            { 0, "AR_Default" },
            { 1, "AR_Elite" },
            { 2, "AR_Cop" },
            { 3, "AR_Goon" }
        };

        (*enum_map)["EPersistentBoolDataSyncType"] = std::map<uint32_t, std::string>{
            { 0, "ePBDST_None" },
            { 1, "ePBDST_ServerAuthoritative" },
            { 2, "ePBDST_Full" }
        };

        (*enum_map)["ESaveLoadStatus"] = std::map<uint32_t, std::string>{
            { 0, "ESaveLoadStatus_OK" },
            { 1, "ESaveLoadStatus_PROCESSING" },
            { 2, "ESaveLoadStatus_MISSING" },
            { 3, "ESaveLoadStatus_ERROR_NODISKSPACE" },
            { 4, "ESaveLoadStatus_ERROR_DEVICEREMOVED" },
            { 5, "ESaveLoadStatus_ERROR_CORRUPT" },
            { 6, "ESaveLoadStatus_ERROR_VERSION" },
            { 7, "ESaveLoadStatus_ERROR_TAMPERED" },
            { 8, "ESaveLoadStatus_ERROR_SAVING" },
            { 9, "ESaveLoadStatus_ERROR_FATAL" },
            { 10, "ESaveLoadStatus_ERROR_ONLINE" },
            { 11, "ESaveLoadStatus_ERROR_REQUIREMENTCHECK" },
            { 12, "ESaveLoadStatus_ERROR_VRMODE" },
            { 13, "ESaveLoadStatus_ERROR_INVALIDATED" }
        };

        (*enum_map)["ESituationAvailability"] = std::map<uint32_t, std::string>{
            { 0, "ESA_AMBIENCE" },
            { 10000, "ESA_AMBIENCE_RESV" },
            { 20000, "ESA_OVR_STANDING" },
            { 70000, "ESA_OVR_ALL" },
            { 30000, "ESA_OVR_CURIOUS" },
            { 40000, "ESA_OVR_SENTRY" },
            { 50000, "ESA_OVR_CAUTIOUS" },
            { 60000, "ESA_OVR_COMBAT" }
        };

        (*enum_map)["EEntityOrdering"] = std::map<uint32_t, std::string>{
            { 0, "EO_LOW" },
            { 1, "EO_NORMAL" },
            { 2, "EO_HIGH" }
        };

        (*enum_map)["EAccidentDeathContext"] = std::map<uint32_t, std::string>{
            { 0, "eADC_DeathByAccident" },
            { 1, "eADC_DeathBySniper" },
            { 2, "eADC_DeathByPoison" },
            { 3, "eADC_DeathByShot" }
        };

        (*enum_map)["EActorType"] = std::map<uint32_t, std::string>{
            { 0, "eAT_Civilian" },
            { 1, "eAT_Guard" },
            { 2, "eAT_Hitman" },
            { 3, "eAT_Last" }
        };

        (*enum_map)["EDialogEventItemType"] = std::map<uint32_t, std::string>{
            { 1, "eDEIT_WavFile" },
            { 2, "eDEIT_RandomContainer" },
            { 3, "eDEIT_SwitchContainer" },
            { 4, "eDEIT_SequenceContainer" },
            { 15, "eDEIT_Invalid" }
        };

        (*enum_map)["ZAT2Controller.EFoot"] = std::map<uint32_t, std::string>{
            { 0, "FOOT_LEFT" },
            { 1, "FOOT_RIGHT" }
        };

        (*enum_map)["ECharacterUpperBodyStateType"] = std::map<uint32_t, std::string>{
            { 0, "eSM_UB_EmptyHanded" },
            { 1, "eSM_UB_Unholster" },
            { 2, "eSM_UB_Hold" },
            { 3, "eSM_UB_Reload" },
            { 4, "eSM_UB_Holster" },
            { 5, "eSM_UB_Conceal" },
            { 6, "eSM_UB_SwapItemHand" },
            { 7, "eSM_UB_Aiming" },
            { 8, "eSM_UB_Pickup" },
            { 9, "eSM_UB_InteractionSwipe" },
            { 10, "eSM_UB_Interaction" },
            { 11, "eSM_UB_Slave" },
            { 12, "eSM_UB_OpenDoor" },
            { 13, "eSM_UB_FlavorIdle" },
            { 14, "eSM_UB_ChangeAmmo" },
            { 15, "eSM_UB_Assemble" },
            { 16, "eSM_UB_Fiberwire" },
            { 17, "eSM_UB_PrimeTwoHanded" },
            { 18, "eSM_UB_SnapNeck" }
        };

        (*enum_map)["ZDecalControllerEntity.ERotationType"] = std::map<uint32_t, std::string>{
            { 0, "eRotationDisable" },
            { 1, "eRotationAlignRay" },
            { 2, "eRotationRandom" }
        };

        (*enum_map)["IScatterContainerEntity.EBrushType"] = std::map<uint32_t, std::string>{
            { 0, "BRUSH_SQUARE" },
            { 1, "BRUSH_CIRCLE" }
        };

        (*enum_map)["EBaseMovementType"] = std::map<uint32_t, std::string>{
            { 4, "eMovementCover" },
            { -1, "eMovementNone" },
            { 36, "eMovementLocomotion" },
            { 0, "eMovementDead" },
            { 1, "eMovementNewFullBody" },
            { 2, "eMovementAgility" },
            { 3, "eMovementAlign" },
            { 5, "eMovementDrainPipe" },
            { 6, "eMovementLadder" },
            { 7, "eMovementPullVictimFromWindow" },
            { 8, "eMovementFiberWireKill" },
            { 9, "eMovementDumpBody" },
            { 10, "eMovementThrowBodyOverRail" },
            { 11, "eMovementDumpBodyOverLedge" },
            { 12, "eMovementOperateCPDoor" },
            { 13, "eMovementDisguiseSafeZone" },
            { 14, "eMovementHideInCloset" },
            { 15, "eMovementTakeDown" },
            { 16, "eMovementCloseCombat" },
            { 17, "eMovementRecoveryFinisher" },
            { 18, "eMovementContextAction" },
            { 19, "eMovementSubaction" },
            { 20, "eMovementGrabVictim" },
            { 21, "eMovementPushVictimThroughWindowAndRail" },
            { 22, "eMovementContextKill" },
            { 23, "eMovementKickVictimOverLedge" },
            { 24, "eMovementDragBody" },
            { 25, "eMovementTakeClothes" },
            { 26, "eMovementCoupDeGrace" },
            { 27, "eMovementThrow" },
            { 28, "eMovementPlace" },
            { 29, "eMovementSurrender" },
            { 30, "eMovementFrisk" },
            { 31, "eMovementShowItem" },
            { 32, "eMovementPeek" },
            { 33, "eMovementFocusedInteraction" },
            { 34, "eMovementSilentTakedown" },
            { 35, "eMovementSnapNeck" },
            { 37, "eMovementLast" },
            { 38, "eMovementPickupItem" }
        };

        (*enum_map)["ZHM5LedgeMount.EFaceDirection"] = std::map<uint32_t, std::string>{
            { 0, "eFaceFront" },
            { 1, "eFaceLeft" },
            { 2, "eFaceRight" }
        };

        (*enum_map)["ZMoveToPositionBehaviorEntity.EApproachAlignment"] = std::map<uint32_t, std::string>{
            { 0, "AA_STRICT" },
            { 1, "AA_LOOSE" }
        };

        (*enum_map)["EClothBendConstrainType"] = std::map<uint32_t, std::string>{
            { 0, "eClothBendConstrainType_Stick" },
            { 1, "eClothBendConstrainType_Triangle" }
        };

        (*enum_map)["ERoleEvent"] = std::map<uint32_t, std::string>{
            { 0, "eRE_NONE" },
            { 1, "eRE_CAST" },
            { 2, "eRE_CLEAR" },
            { 3, "eRE_ENTER_DRAMA" },
            { 4, "eRE_REENTER_DRAMA" },
            { 5, "eRE_LEAVE_DRAMA" },
            { 6, "eRE_NEW_DRAMA" },
            { 7, "eRE_PAUSED" },
            { 8, "eRE_RESUMING" },
            { 9, "eRE_RUNNING" }
        };

        (*enum_map)["ZWorldUIElementInstanceBase.EVisibility"] = std::map<uint32_t, std::string>{
            { 0, "EVIS_ALWAYS" },
            { 1, "EVIS_POSY" },
            { 2, "EVIS_NEGY" },
            { 3, "EVIS_POSX" },
            { 4, "EVIS_NEGX" }
        };

        (*enum_map)["EHUDIconFlags"] = std::map<uint32_t, std::string>{
            { 1, "HUD_ICON_FLAG_POSITION" },
            { 2, "HUD_ICON_FLAG_VALUE" },
            { 4, "HUD_ICON_FLAG_SCALE" },
            { 8, "HUD_ICON_FLAG_TEXT" }
        };

        (*enum_map)["EDeathAnimationType"] = std::map<uint32_t, std::string>{
            { 0, "DAT_None" },
            { 1, "DAT_SingleShot" },
            { 2, "DAT_BurstShot" },
            { 3, "DAT_HeavyShot" },
            { 4, "DAT_Unknown" }
        };

        (*enum_map)["ZCautiousInvestigateGroup.EAssistantState"] = std::map<uint32_t, std::string>{
            { 0, "AS_Waiting" },
            { 1, "AS_CalculateFields" },
            { 2, "AS_RequestNode" },
            { 3, "AS_Moving" },
            { 4, "AS_Investigating" },
            { 5, "AS_Max" }
        };

        (*enum_map)["ZHM5Animator.EBoneAnimator"] = std::map<uint32_t, std::string>{
            { 0, "BA_ANIMATION" },
            { 1, "BA_RAGDOLL" },
            { 2, "BA_BLENDING" },
            { 3, "BA_POWEREDRAGDOLL" },
            { 4, "BA_POSE" }
        };

        (*enum_map)["ERestitutionCombineMode"] = std::map<uint32_t, std::string>{
            { 0, "ERestitutionCombineMode_AVERAGE" },
            { 1, "ERestitutionCombineMode_MIN" },
            { 2, "ERestitutionCombineMode_MULTIPLY" },
            { 3, "ERestitutionCombineMode_MAX" }
        };

        (*enum_map)["ECharacterIdlePriority"] = std::map<uint32_t, std::string>{
            { 0, "eCIP_Irrelevant" },
            { 1, "eCIP_VeryLow" },
            { 2, "eCIP_QuiteLow" },
            { 3, "eCIP_Low" },
            { 4, "eCIP_MediumLow" },
            { 5, "eCIP_Medium" },
            { 6, "eCIP_MediumHigh" },
            { 7, "eCIP_High" },
            { 8, "eCIP_QuiteHigh" },
            { 9, "eCIP_VeryHigh" },
            { 10, "eCIP_Urgent" }
        };

        (*enum_map)["ZSequenceEntity.ECommand"] = std::map<uint32_t, std::string>{
            { 0, "COMMAND_START" },
            { 1, "COMMAND_ENABLE_PREVIEW" },
            { 2, "COMMAND_STOP" },
            { 3, "COMMAND_DISABLE_PREVIEW" },
            { 4, "COMMAND_ABORT" },
            { 5, "COMMAND_PAUSE" },
            { 6, "COMMAND_UNPAUSE" },
            { 7, "COMMAND_SETTIME" },
            { 8, "COMMAND_SKIPTOEND" },
            { 9, "COMMAND_GAME_PAUSE" },
            { 10, "COMMAND_GAME_UNPAUSE" },
            { 11, "COMMAND_PREVIEW_PAUSE" },
            { 12, "COMMAND_PREVIEW_STOP" },
            { 13, "COMMAND_PREVIEW_PLAY" },
            { 14, "COMMAND_PREVIEW_LOOPED" },
            { 15, "COMMAND_CLEAR" }
        };

        (*enum_map)["ZVRCameraEntity.EEye"] = std::map<uint32_t, std::string>{
            { 0, "eLeftEye" },
            { 1, "eRightEye" }
        };

        (*enum_map)["EDisposalType"] = std::map<uint32_t, std::string>{
            { 0, "DISPOSAL_UNDEFINED" },
            { 1, "DISPOSAL_NOTHING" },
            { 2, "DISPOSAL_HOLSTER" },
            { 3, "DISPOSAL_HIDE" },
            { 4, "DISPOSAL_PLACE" },
            { 5, "DISPOSAL_DROP" },
            { 6, "DISPOSAL_DROP_HIDE" },
            { 7, "DISPOSAL_DESTROY" },
            { 8, "DISPOSAL_COUNT" }
        };

        (*enum_map)["EDynamicVariableType"] = std::map<uint32_t, std::string>{
            { -1, "DV_Invalid" },
            { 4, "DV_Sounds" },
            { 0, "DV_Me" },
            { 1, "DV_Hitman" },
            { 2, "DV_InSight" },
            { 3, "DV_RecentlyInSight" },
            { 5, "DV_NumVariables" }
        };

        (*enum_map)["EButtonState"] = std::map<uint32_t, std::string>{
            { -1, "BUTTON_STATE_DISABLED" },
            { 0, "BUTTON_STATE_NONE" },
            { 1, "BUTTON_STATE_ENABLED" },
            { 2, "BUTTON_STATE_PRESSED" },
            { 3, "BUTTON_STATE_BLINKING" }
        };

        (*enum_map)["ZHM5AgilityEventConsumer.EEvent"] = std::map<uint32_t, std::string>{
            { 0, "EAE_LEFTHAND" },
            { 1, "EAE_RIGHTHAND" },
            { 2, "EAE_LEFTLEG" },
            { 3, "EAE_RIGHTLEG" }
        };

        (*enum_map)["ZHM5BodyContainer.EBCState"] = std::map<uint32_t, std::string>{
            { 0, "BC_CLOSED" },
            { 1, "BC_OPEN" },
            { 2, "BC_FLUSH" },
            { 3, "BC_OPENING" },
            { 4, "BC_CLOSING" },
            { 5, "BC_FLUSHING" }
        };

        (*enum_map)["ZHM5Item.EKeywordEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["InputControlNamesp.eHM5InputActionID"] = std::map<uint32_t, std::string>{
            { 0, "eIDButtonFaceDown" },
            { 1, "eIDButtonFaceLeft" },
            { 2, "eIDButtonFaceRight" },
            { 3, "eIDButtonFaceUp" },
            { 4, "eIDBumperLeft" },
            { 5, "eIDBumperRight" },
            { 6, "eIDDpadDown" },
            { 7, "eIDDpadLeft" },
            { 8, "eIDDpadRight" },
            { 9, "eIDDpadUp" },
            { 10, "eIDThumbLeft" },
            { 11, "eIDThumbRight" },
            { 12, "eIDButtonStart" },
            { 13, "eIDButtonSelect" },
            { 14, "eIDTriggerLeft" },
            { 15, "eIDTriggerRight" },
            { 16, "eIDStickLeftHorizontal" },
            { 17, "eIDStickLeftVertical" },
            { 18, "eIDStickRightHorizontal" },
            { 19, "eIDStickRightVertical" },
            { 20, "eIDKeyboard" },
            { 21, "eID_INVALID" }
        };

        (*enum_map)["JSONTemplate.ETemplateType"] = std::map<uint32_t, std::string>{
            { 0, "ETT_BASE" },
            { 1, "ETT_DATA_VALUE" },
            { 2, "ETT_DATA_VALUE_CUSTOM" },
            { 3, "ETT_OBJECT" },
            { 4, "ETT_ARRAY" },
            { 5, "ETT_ASYNCROOT" },
            { 6, "ETT_IF" },
            { 7, "ETT_SWITCH" },
            { 8, "ETT_EACH" },
            { 9, "ETT_LOC" },
            { 10, "ETT_PARENTHESIS" },
            { 11, "ETT_FORMATSTRING" },
            { 12, "ETT_FORMATINT" },
            { 13, "ETT_INTCLAMP" },
            { 14, "ETT_FORMATPUBLICID" },
            { 15, "ETT_ISNULL" },
            { 16, "ETT_DATA_CONTEXT" },
            { 17, "ETT_ARRAY_ELEMENT" },
            { 18, "ETT_ARRAY_FILTER" },
            { 19, "ETT_ARRAY_CONVERT" },
            { 20, "ETT_ARRAY_SIZE" },
            { 21, "ETT_ARRAY_SORT" },
            { 22, "ETT_ARRAY_GROUPBY" },
            { 23, "ETT_EXPAND" },
            { 24, "ETT_ISNULLOREMPTY" },
            { 25, "ETT_NOT" },
            { 26, "ETT_COMPARE" },
            { 27, "ETT_PARENT" },
            { 28, "ETT_AND" },
            { 29, "ETT_OR" },
            { 30, "ETT_MERGEOBJECTS" },
            { 31, "ETT_MERGEARRAYS" },
            { 32, "ETT_LOCALE_TEXT" },
            { 33, "ETT_LOCALE_AUDIO" },
            { 34, "ETT_CURRENT_LOCALE_TEXT" },
            { 35, "ETT_GET_APPLICATION_OPTION" },
            { 36, "ETT_PLATFORM" },
            { 37, "ETT_REGION" },
            { 38, "ETT_STOREREGION" },
            { 39, "ETT_STORE" },
            { 40, "ETT_IS_DISC_RELEASE" },
            { 41, "ETT_ISDEBUG" },
            { 42, "ETT_IS_CONTROLLER_AVAILABLE" },
            { 43, "ETT_IS_KEYBOARD_AVAILABLE" },
            { 44, "ETT_IS_VR_DEVICE_AVAILABLE" },
            { 45, "ETT_IS_VR_DEVICE_ACTIVE" },
            { 46, "ETT_IS_VR_ACTIVE" },
            { 47, "ETT_VR_MODE" },
            { 48, "ETT_ONLINE_RESOURCE" },
            { 49, "ETT_INCLUDE" },
            { 50, "ETT_UI_OPTION_VALUE" },
            { 51, "ETT_UI_OPTION_DEFAULT_VALUE" },
            { 52, "ETT_UI_OPTION_DEBUG_VALUE" },
            { 53, "ETT_UI_OPTION_AVAILABLE_DISPLAY_RESOLUTIONS" },
            { 54, "ETT_UI_OPTION_AVAILABLE_DISPLAY_FULLSCREEN" },
            { 55, "ETT_UI_OPTION_AVAILABLE_TEXTURE_QUALITY" },
            { 56, "ETT_UI_OPTION_AVAILABLE_SHADOW_QUALITY" },
            { 57, "ETT_UI_OPTION_AVAILABLE_SUPERSAMPLING" },
            { 58, "ETT_UI_OPTION_AVAILABLE_DOUBLEFRAMERATE" },
            { 59, "ETT_CURRENT_CONTRACT_CONTEXT" },
            { 60, "ETT_CURRENT_CONTRACT_OBJECTIVES" },
            { 61, "ETT_CURRENT_CONTRACT_SESSIONID" },
            { 62, "ETT_CURRENT_CONTRACT_CHARACTER_INFO" },
            { 63, "ETT_CURRENT_CONTRACT_TRACKED_OPPORTUNITY" },
            { 64, "ETT_CURRENT_DIFFICULTY" },
            { 65, "ETT_CURRENT_ENGINE_MODE" },
            { 66, "ETT_CURRENT_GAME_MODE" },
            { 67, "ETT_CAN_SAVE" },
            { 68, "ETT_IS_SAVELIMIT_EXCEEDED" },
            { 69, "ETT_IS_ALLOWED_TO_RESTART" },
            { 70, "ETT_ACTIVE_CHALLENGES" },
            { 71, "ETT_USER" },
            { 72, "ETT_ISUSER" },
            { 73, "ETT_DLCCOUNT" },
            { 74, "ETT_PROFILEID" },
            { 75, "ETT_HDRGAMMAVALUERANGEMIN" },
            { 76, "ETT_HDRGAMMAVALUERANGEMAX" },
            { 77, "ETT_HDRGAMMAVALUESTEP" },
            { 78, "ETT_ISHDRAVAILABLE" },
            { 79, "ETT_ISHDRRENDERING" },
            { 80, "ETT_ISVRSAVAILABLE" },
            { 81, "ETT_ITEM" },
            { 82, "ETT_REPOSITORY" },
            { 83, "ETT_ISONLINE" },
            { 84, "ETT_ISINGAME" },
            { 85, "ETT_ISINEDITOR" },
            { 86, "ETT_ISUGCRESTRICTED" },
            { 87, "ETT_ISPACKAGEOWNED" },
            { 88, "ETT_ISININVENTORY" },
            { 89, "ETT_ISPLATFORMENTITLEMENTOWNED" },
            { 90, "ETT_MULTIPLAYERLOBBYSTATE" },
            { 91, "ETT_MULTIPLAYERLOCALID" },
            { 92, "ETT_MULTIPLAYERJOINEDIDS" },
            { 93, "ETT_MULTIPLAYERISPLAYERREADY" },
            { 94, "ETT_MULTIPLAYERINFO" },
            { 95, "ETT_MULTIPLAYERMATCHMAKINGTIME" },
            { 96, "ETT_MULTIPLAYERNATTYPE" },
            { 97, "ETT_LOADOUT" },
            { 98, "ETT_LOADOUT_SLOT" },
            { 99, "ETT_LOADOUT_AGENCY_PICKUP" },
            { 100, "ETT_LOADOUT_SELECTED_ENTRANCE" },
            { 101, "ETT_PERSISTENTMENUDATA" },
            { 102, "ETT_GAMEPERSISTENTDATA" },
            { 103, "ETT_ENDGAME_PAGETRANSITION_OVERRIDE" },
            { 104, "ETT_VERSUSINFO" },
            { 105, "ETT_SNIPERINFO" },
            { 106, "ETT_GAMEMODE" },
            { 107, "ETT_CACHEDUSERCENTRICCONTRACT" },
            { 108, "ETT_SAVEGAMES" },
            { 109, "ETT_AVAILABILITY_OF_RESOURCES" },
            { 110, "ETT_AVAILABILITY_OF_CONTRACT" },
            { 111, "ETT_AS3DATE" },
            { 112, "ETT_IOIACCOUNT_STATUS" },
            { 113, "ETT_ISCONTRACT_IN_PLAYLIST" },
            { 114, "ETT_ISCONTRACT_IN_PLAYLIST_MARKED_FOR_DELETION" },
            { 115, "ETT_VIDEOMEMORYINFO_IS_SUPPORTED" },
            { 116, "ETT_VIDEOMEMORYINFO" },
            { 117, "ETT_DISPLAY_SIZE" },
            { 118, "ETT_IS_VIDEO_VALID" },
            { 119, "ETT_INTEL_IDS" },
            { 120, "ETT_INTEL_DETAILS" },
            { 121, "ETT_MENU_CONFIGURATION" },
            { 122, "ETT_HERO_INVENTORY" },
            { 123, "ETT_MAP_TRACKERINFO" },
            { 124, "ETT_WAS_EXITGATE_TRIGGERED" },
            { 125, "ETT_ARE_OPPORTUNITIES_ENABLED" },
            { 126, "ETT_CHARACTERS" },
            { 127, "ETT_CHARACTERINFO" },
            { 128, "ETT_DATABINDING" },
            { 129, "ETT_DATABINDING_ROOT" },
            { 130, "ETT_ALLOWED_UNLOCKABLES" },
            { 131, "ETT_HAS_FRAMERATE_OPTION" },
            { 132, "ETT_NVIDIAHIGHLIGHTSAVAILABLE" },
            { 133, "ETT_NVIDIAHIGHLIGHTSHASHIGHLIGHTS" },
            { 134, "ETT_DEBUG_ALLHITMANSUITS" }
        };

        (*enum_map)["ZMirrorEntity.EMirrorQuality"] = std::map<uint32_t, std::string>{
            { 0, "QUALITY_LOW" },
            { 1, "QUALITY_MEDIUM" },
            { 2, "QUALITY_HIGH" },
            { 3, "QUALITY_LEAVE" }
        };

        (*enum_map)["EWeaponAnimationCategory"] = std::map<uint32_t, std::string>{
            { 0, "eWAC_Undefined" },
            { 1, "eWAC_Pistol" },
            { 2, "eWAC_Revolver" },
            { 3, "eWAC_SMG_2H" },
            { 4, "eWAC_SMG_1H" },
            { 5, "eWAC_Rifle" },
            { 6, "eWAC_Sniper" },
            { 7, "eWAC_Shotgun_Pump" },
            { 8, "eWAC_Shotgun_Semi" }
        };

        (*enum_map)["EAILegalType"] = std::map<uint32_t, std::string>{
            { 0, "AILT_Weapon" },
            { 1, "AILT_Shooting" },
            { 2, "AILT_Aiming" },
            { 3, "AILT_CloseCombat" },
            { 4, "AILT_ChangeOutfit" },
            { 5, "AILT_Count" }
        };

        (*enum_map)["ZActorPickerFilterGroup.EEvaluationType"] = std::map<uint32_t, std::string>{
            { 0, "ALL" },
            { 1, "NONE" },
            { 2, "ANY" }
        };

        (*enum_map)["ZHM5HitmanHealthModifier.EHealthValue"] = std::map<uint32_t, std::string>{
            { 40, "eHealth40" },
            { 0, "eHealth0" },
            { 50, "eHealth50" },
            { 10, "eHealth10" },
            { 60, "eHealth60" },
            { 20, "eHealth20" },
            { 70, "eHealth70" },
            { 30, "eHealth30" },
            { 80, "eHealth80" },
            { 90, "eHealth90" },
            { 100, "eHealth100" }
        };

        (*enum_map)["EConversationID"] = std::map<uint32_t, std::string>{
            { 0, "eCI_GuardCivilian_Distraction_Investigation" },
            { 1, "eCI_HearBulletImpact_Distraction_Investigation" },
            { 2, "eCI_HearItemImpact_Distraction_Investigation" },
            { 3, "eCI_HearCarAlarm_Distraction_Investigation" },
            { 4, "eCI_SeeItemToss_Distraction_Investigation" },
            { 5, "eCI_HearRadio_Distraction_Investigation" },
            { 6, "eCI_HearPain_Distraction_Investigation" },
            { 7, "eCI_HearAccident_Distraction_Investigation" },
            { 8, "eCI_HearCuriousItemSound_Distraction_Investigation" },
            { 9, "eCI_HearCuriousSound_Distraction_Investigation" },
            { 10, "eCI_SeeSuspiciousPerceptible_Distraction_Investigation" },
            { 11, "eCI_SeeInterestingItem_Distraction_Investigation" },
            { 12, "eCI_HearFootSteps_Distraction_Investigation" },
            { 13, "eCI_HearAngryDialog_Distraction_Investigation" },
            { 14, "eCI_HearHelpDialog_Distraction_Investigation" },
            { 15, "eCI_HearWarning_Distraction_Investigation" },
            { 16, "eCI_SightInvestigation_Distraction_Investigation" },
            { 17, "eCI_HearBulletImpact_Distraction_StandDown" },
            { 18, "eCI_HearItemImpact_Distraction_StandDown" },
            { 19, "eCI_HearCarAlarm_Distraction_StandDown" },
            { 20, "eCI_SeeItemToss_Distraction_StandDown" },
            { 21, "eCI_HearRadio_Distraction_StandDown" },
            { 22, "eCI_HearPain_Distraction_StandDown" },
            { 23, "eCI_HearAccident_Distraction_StandDown" },
            { 24, "eCI_HearCuriousItemSound_Distraction_StandDown" },
            { 25, "eCI_HearCuriousSound_Distraction_StandDown" },
            { 26, "eCI_SeeSuspiciousPerceptible_Distraction_StandDown" },
            { 27, "eCI_SeeInterestingItem_Distraction_StandDown" },
            { 28, "eCI_HearFootSteps_Distraction_StandDown" },
            { 29, "eCI_HearAngryDialog_Distraction_StandDown" },
            { 30, "eCI_HearHelpDialog_Distraction_StandDown" },
            { 31, "eCI_HearWarning_Distraction_StandDown" },
            { 32, "eCI_SightInvestigation_Distraction_StandDown" },
            { 33, "eCI_Suitcase_Distraction_DeliverToGuard" },
            { 34, "eCI_Suitcase_Distraction_RadioRequestHelp" },
            { 35, "eCI_Count" }
        };

        (*enum_map)["ZDebugTextEntity.EHorizontalAlignment"] = std::map<uint32_t, std::string>{
            { 1, "HA_LEFT" },
            { 0, "HA_CENTER" },
            { 2, "HA_RIGHT" }
        };

        (*enum_map)["EClipSpawnAxis"] = std::map<uint32_t, std::string>{
            { 0, "eCSA_XPos" },
            { 1, "eCSA_XNeg" },
            { 2, "eCSA_YPos" },
            { 3, "eCSA_YNeg" },
            { 4, "eCSA_ZPos" },
            { 5, "eCSA_ZNeg" }
        };

        (*enum_map)["EMeBarState"] = std::map<uint32_t, std::string>{
            { 0, "ME_BAR_NONE" },
            { 1, "ME_BAR_TRESPASSING" },
            { 2, "ME_BAR_DEEPTRESPASSING" },
            { 3, "ME_BAR_VISIBLY_ARMED" },
            { 4, "ME_BAR_CHANGING_DISGUISE" },
            { 5, "ME_BAR_DISGUISE_BLOWN" },
            { 6, "ME_BAR_DISGUISE_SUSPICIOUS" },
            { 7, "ME_BAR_NEAR_BODY" }
        };

        (*enum_map)["ECameraCollisionMode"] = std::map<uint32_t, std::string>{
            { 0, "ECAMERACOLLISIONMODE_COLLIDE_ALWAYS" },
            { 1, "ECAMERACOLLISIONMODE_CAMERA_OBSTACLE" },
            { 2, "ECAMERACOLLISIONMODE_COLLIDE_NEVER" },
            { 3, "ECAMERACOLLISIONMODE_COLLIDE_DEFAULT" }
        };

        (*enum_map)["EClothStretchConstrainType"] = std::map<uint32_t, std::string>{
            { 0, "eClothBendConstrainType_Anchor" },
            { 1, "eClothBendConstrainType_LRA" },
            { 2, "eClothBendConstrainType_None" }
        };

        (*enum_map)["ENamedGameEvents"] = std::map<uint32_t, std::string>{
            { 0, "eEvent_IntroCutStart" },
            { 1, "eEvent_PlayingAfterLoad" },
            { 2, "eEvent_SetModePlaying" },
            { 3, "eNumNamedGameEvents" }
        };

        (*enum_map)["ZDebugTextEntity.EVerticalAlignment"] = std::map<uint32_t, std::string>{
            { 16, "VA_TOP" },
            { 0, "VA_CENTER" },
            { 32, "VA_BOTTOM" }
        };

        (*enum_map)["ZSetUIControlDisplayInfoEntity.EProjectionOp"] = std::map<uint32_t, std::string>{
            { 0, "NoOp" },
            { 1, "SetPerspective" }
        };

        (*enum_map)["ZDistanceCondition.EObjectType"] = std::map<uint32_t, std::string>{
            { 0, "DT_ToCam" },
            { 1, "DT_ToPlayer" }
        };

        (*enum_map)["EHM5SoundFootstepEmitterTarget"] = std::map<uint32_t, std::string>{
            { 0, "EFSAT_MAIN" },
            { 1, "EFSAT_NPC" }
        };

        (*enum_map)["EGuardPointType"] = std::map<uint32_t, std::string>{
            { 0, "GPT_Hold" },
            { 1, "GPT_Combat" },
            { 2, "GPT_CombatAndHold" },
            { 3, "GPT_VIPSafeRoom" }
        };

        (*enum_map)["ECharacterEvent"] = std::map<uint32_t, std::string>{
            { 0, "eCIET_PickUp" },
            { 1, "eCIET_LadderEnded" },
            { 2, "eCIET_AgilityEnded" },
            { 3, "eCIET_CCEnded" },
            { 4, "eCIET_Disguised" },
            { 5, "eCIET_HideBody" },
            { 6, "eCIET_WalkingOnStairs" },
            { 7, "eCIET_Sniping_Congratulate" },
            { 8, "eCIET_Sniping_Acknowledge" },
            { 9, "eCIET_FlavorIdleEnded" },
            { 10, "eCIET_StartedMoving" },
            { 11, "eCIET_StoppedMoving" },
            { 12, "eCIET_DragBody" },
            { 13, "eCIET_Takedown" },
            { 14, "eCIET_Damaged" },
            { 34, "eCIET_None" }
        };

        (*enum_map)["ECharacterIdleHandlerUpperBodyType"] = std::map<uint32_t, std::string>{
            { 0, "eCIHUBT_NothingToDoEmptySingleHand" },
            { 1, "eCIHUBT_NothingToDoEmptyHands" },
            { 2, "eCIHUBT_Event" },
            { 3, "eCIHUBT_Custom" }
        };

        (*enum_map)["ECharacterBodyType"] = std::map<uint32_t, std::string>{
            { 0, "eCT_MaleRegular" },
            { 1, "eCT_FemaleRegular" },
            { 2, "eCT_Male47" },
            { 3, "eCT_Undefined" }
        };

        (*enum_map)["EAimAssistActivationState"] = std::map<uint32_t, std::string>{
            { 0, "eAAAS_Activated" },
            { 1, "eAAAS_Activating" },
            { 2, "eAAAS_Deactivated" },
            { 3, "eAAAS_Deactivating" }
        };

        (*enum_map)["ZHM5BodyContainer.EBCAnimSet"] = std::map<uint32_t, std::string>{
            { 0, "BC_CONTAINER_STANDARD" },
            { 1, "BC_CONTAINER_WOODCHIPPER" },
            { 2, "BC_CAR_STANDARD_TRUNK" },
            { 3, "BC_CONTAINER_CLOSET" },
            { 4, "BC_CONTAINER_LAUNDRYCHUTE" }
        };

        (*enum_map)["ELedgeDismountDirection"] = std::map<uint32_t, std::string>{
            { 0, "eLDD_DontCare" },
            { 1, "eLDD_FaceLedge" },
            { 2, "eLDD_FaceLeft" },
            { 3, "eLDD_FaceRight" },
            { 4, "eLDD_Turn180" }
        };

        (*enum_map)["ETargetPrintDestination"] = std::map<uint32_t, std::string>{
            { 0, "eTPD_Screen" },
            { 1, "eTPD_TraceViewer" }
        };

        (*enum_map)["ZHUDWorldSpatialAttachEntity.EOrientation"] = std::map<uint32_t, std::string>{
            { 0, "ROTATE_WITH_SPATIAL" },
            { 1, "LOOK_AT_CAMERA_POSITION" },
            { 2, "LOOK_AT_CAMERA_POSITION_HORIZ_ONLY" },
            { 3, "LOOK_AT_CAMERAPARENT_POSITION" },
            { 4, "LOOK_AT_CAMERAPARENT_POSITION_HORIZ_ONLY" },
            { 5, "ALIGN_WITH_VIEW_PLANE" },
            { 6, "ALIGN_WITH_VIEW_PLANE_HORIZ_ONLY" }
        };

        (*enum_map)["eParticleEmitterBoxEntity_SpawnModes"] = std::map<uint32_t, std::string>{
            { 0, "BOX_SPAWNMODE_INSIDE" },
            { 1, "BOX_SPAWNMODE_EDGE" },
            { 2, "BOX_SPAWNMODE_FACE" },
            { 3, "BOX_SPAWNMODE_ELLIPSOID_INSIDE" },
            { 4, "BOX_SPAWNMODE_ELLIPSOID" }
        };

        (*enum_map)["EDeathType"] = std::map<uint32_t, std::string>{
            { 0, "eDT_UNDEFINED" },
            { 1, "eDT_PACIFY" },
            { 2, "eDT_KILL" },
            { 3, "eDT_BLOODY_KILL" }
        };

        (*enum_map)["ECommunicationBarMessage"] = std::map<uint32_t, std::string>{
            { 0, "ECBM_Unknown" },
            { 1, "ECBM_Spotted" },
            { 2, "ECBM_Frisking" },
            { 3, "ECBM_HostileArea" },
            { 4, "ECBM_InvestigatingArea" },
            { 5, "ECBM_Clear" },
            { 6, "ECBM_Suspicious" },
            { 7, "ECBM_Alerted" },
            { 8, "ECBM_Hunting" },
            { 9, "ECBM_Arresting" },
            { 10, "ECBM_Engaging" },
            { 11, "ECBM_Hostile" },
            { 12, "ECBM_IdentityKnown" },
            { 13, "ECBM_Agitated" },
            { 14, "ECBM_VipRunsToSafeArea" },
            { 15, "ECBM_VipEscaping" },
            { 16, "ECBM_BodyFound" },
            { 17, "ECBM_GunshotHeard" },
            { 18, "ECBM_CloseCombatHeard" },
            { 19, "ECBM_CrimeNoticed" },
            { 20, "ECBM_BulletImpactNoticed" },
            { 21, "ECBM_SpottedByCamera" },
            { 22, "ECBM_UnconsciousWitness" }
        };

        (*enum_map)["ZHUDCamera3DControllerEntity.EType"] = std::map<uint32_t, std::string>{
            { 0, "Menu" },
            { 1, "HUD" }
        };

        (*enum_map)["EThrowType"] = std::map<uint32_t, std::string>{
            { 0, "THROW_NONE" },
            { 1, "THROW_COIN" },
            { 2, "THROW_NORMAL" },
            { 3, "THROW_HEAVY" },
            { 4, "THROW_KNOCKDOWN_LIGHT" },
            { 5, "THROW_KNOCKDOWN_HEAVY" },
            { 6, "THROW_PACIFY_LIGHT" },
            { 7, "THROW_PACIFY_HEAVY" },
            { 8, "THROW_DEADLY_LIGHT" },
            { 9, "THROW_DEADLY_HEAVY" }
        };

        (*enum_map)["EDetectedNatType"] = std::map<uint32_t, std::string>{
            { 0, "NAT_UNSET" },
            { 1, "NAT_1" },
            { 2, "NAT_2" },
            { 3, "NAT_3" }
        };

        (*enum_map)["ILightEntity.ERoomLightFlow"] = std::map<uint32_t, std::string>{
            { 0, "AUTOMATIC" },
            { 1, "LOCAL" },
            { 2, "ALLROOMS" }
        };

        (*enum_map)["ECrossHairType"] = std::map<uint32_t, std::string>{
            { 0, "CROSSHAIR_NONE" },
            { 1, "CROSSHAIR_PISTOL" },
            { 2, "CROSSHAIR_REVOLVER" },
            { 3, "CROSSHAIR_SMG" },
            { 4, "CROSSHAIR_RIFLE" },
            { 5, "CROSSHAIR_SHOTGUN" },
            { 6, "CROSSHAIR_SNIPER" },
            { 7, "CROSSHAIR_HARDBALLER" },
            { 8, "CROSSHAIR_UNAIMED" },
            { 9, "CROSSHAIR_POINTER" },
            { 10, "CROSSHAIR_WORLDCENTERMARKER" },
            { 11, "CROSSHAIR_BLINDFIRE" },
            { 12, "CROSSHAIR_RANGEINDICATOR" },
            { 13, "CROSSHAIR_DOT" }
        };

        (*enum_map)["EMarkerClipAlgorithm"] = std::map<uint32_t, std::string>{
            { 0, "MCA_NONE" },
            { 1, "MCA_SIMPLE" },
            { 2, "MCA_ADVANCED" }
        };

        (*enum_map)["ZHeroBoneAttachEntity.EResetMode"] = std::map<uint32_t, std::string>{
            { 0, "eAtBoneOrigin" },
            { 1, "eKeepOffset" }
        };

        (*enum_map)["eBurstPatternType"] = std::map<uint32_t, std::string>{
            { 0, "eEBP_None" },
            { 1, "eEBP_Circular3Rings" },
            { 2, "eEBP_Box" }
        };

        (*enum_map)["EQueryMaskType"] = std::map<uint32_t, std::string>{
            { 1, "eQueryMaskType_CameraCollision" },
            { 2, "eQueryMaskType_CameraObstacle" },
            { 4, "eQueryMaskType_ExplosionRelevant" }
        };

        (*enum_map)["ZPointOfInterestEntity.EPOIType"] = std::map<uint32_t, std::string>{
            { 0, "ePOIT_Invalid" },
            { 1, "ePOIT_Glance" },
            { 2, "ePOIT_Inspect" },
            { 3, "ePOIT_Stare" }
        };

        (*enum_map)["EGait"] = std::map<uint32_t, std::string>{
            { 0, "eGait_Normal" },
            { 1, "eGait_Reposition" },
            { 2, "eGait_Alert" },
            { 3, "eGait_Scared" },
            { 4, "eGait_Prone" },
            { 5, "eGait_Dead" },
            { 6, "eGait_Angry" },
            { 7, "eGait_Applause" },
            { 8, "eGait_BeatUp_000cm_01_Dual_A" },
            { 9, "eGait_BeatUp_000cm_01_Dual_B" },
            { 10, "eGait_BeatUp_000cm_02_Dual_A" },
            { 11, "eGait_BeatUp_000cm_02_Dual_B" },
            { 12, "eGait_BeatUp_Wall_Dual_A" },
            { 13, "eGait_BeatUp_Wall_Dual_B" },
            { 14, "eGait_Chatting" },
            { 15, "eGait_Cheer" },
            { 16, "eGait_Clap" },
            { 17, "eGait_Climb_Up_Fall" },
            { 18, "eGait_Concerned" },
            { 19, "eGait_Couples_A" },
            { 20, "eGait_Couples_B" },
            { 21, "eGait_Couples_C" },
            { 22, "eGait_Dance_Drunk" },
            { 23, "eGait_Dance_Dual_A" },
            { 24, "eGait_Dance_Dual_B" },
            { 25, "eGait_Dance_Party" },
            { 26, "eGait_Dance_Party_A" },
            { 27, "eGait_Dance_Party_B" },
            { 28, "eGait_Dance_Party_C" },
            { 29, "eGait_Dance_Party_D" },
            { 30, "eGait_Dance_Party_E" },
            { 31, "eGait_Dance_Party_F" },
            { 32, "eGait_Dance_Rave" },
            { 33, "eGait_Dance_Rave_A" },
            { 34, "eGait_Dance_Rave_B" },
            { 35, "eGait_Dance_Rave_C" },
            { 36, "eGait_Dance_Rave_D" },
            { 37, "eGait_Dance_Rave_E" },
            { 38, "eGait_Dance_Rave_F" },
            { 39, "eGait_Dance_Rave_Zone_Out_A" },
            { 40, "eGait_Dance_Rave_Zone_Out_B" },
            { 41, "eGait_Dance_Rave_Zone_Out_C" },
            { 42, "eGait_Dance_Rave_Zone_Out_D" },
            { 43, "eGait_Dance_Rave_Zone_Out_E" },
            { 44, "eGait_Dance_Rave_Zone_Out_F" },
            { 45, "eGait_Dance_Rave_Zone_Out_G" },
            { 46, "eGait_Dance_Rave_Zone_Out_H" },
            { 47, "eGait_Excited" },
            { 48, "eGait_Fanatic_Fans" },
            { 49, "eGait_Freeze_A" },
            { 50, "eGait_Freeze_B" },
            { 51, "eGait_Freeze_C" },
            { 52, "eGait_Freeze_Lean_Wall" },
            { 53, "eGait_Haggle" },
            { 54, "eGait_Interested" },
            { 55, "eGait_Jeer" },
            { 56, "eGait_Lean_Rail" },
            { 57, "eGait_Lean_Wall" },
            { 58, "eGait_Lie_WritheInPain" },
            { 59, "eGait_LookAt_Race_A" },
            { 60, "eGait_LookAt_Race_B" },
            { 61, "eGait_LookUp_Excited" },
            { 62, "eGait_Mingle" },
            { 63, "eGait_Mingle_120cm" },
            { 64, "eGait_Mingle_Drunk" },
            { 65, "eGait_Mingle_Rave_A" },
            { 66, "eGait_Mingle_Rave_B" },
            { 67, "eGait_Mingle_Rave_C" },
            { 68, "eGait_Mingle_Mumbai_A" },
            { 69, "eGait_Mingle_Mumbai_B" },
            { 70, "eGait_Mingle_Race" },
            { 71, "eGait_Mingle_Race_Rail" },
            { 72, "eGait_Mingle_Wall" },
            { 73, "eGait_Pacing_Angry" },
            { 74, "eGait_Party" },
            { 75, "eGait_Protest" },
            { 76, "eGait_Push_Gate" },
            { 77, "eGait_Rally" },
            { 78, "eGait_Reaction_TearGas" },
            { 79, "eGait_Shocked" },
            { 80, "eGait_Shop" },
            { 81, "eGait_Sit_020cm" },
            { 82, "eGait_Sit_040cm" },
            { 83, "eGait_Sit_100cm" },
            { 84, "eGait_Sit_ChairBasic" },
            { 85, "eGait_Sit_ChairBasic_LeanForward" },
            { 86, "eGait_Sit_Chair_Rave_A" },
            { 87, "eGait_Sit_Chair_Rave_B" },
            { 88, "eGait_Sit_Chair_Rave_C" },
            { 89, "eGait_Sit_Ground_Hungover" },
            { 90, "eGait_Squat_Relaxed" },
            { 91, "eGait_Squat_Sorting" },
            { 92, "eGait_Throw_Rice" },
            { 93, "eGait_Wait_Excited" },
            { 94, "eGait_Wait_InLine" },
            { 95, "eGait_WarmHands_100cm" },
            { 96, "eGait_Wave_Mexican" },
            { 97, "eGait_Yell_Up" },
            { 98, "eGait_Custom0" },
            { 99, "eGait_Custom1" },
            { 100, "eGait_Custom2" },
            { 101, "eGait_Custom3" },
            { 102, "eGait_Custom4" },
            { 103, "eGait_Custom5" },
            { 104, "eGait_Custom6" },
            { 105, "eGait_Custom7" },
            { 106, "eGait_Custom8" },
            { 107, "eGait_Custom9" }
        };

        (*enum_map)["EAIGoal"] = std::map<uint32_t, std::string>{
            { 0, "AIG_Kill" },
            { 1, "AIG_Arrest" },
            { 2, "AIG_FightSniper" },
            { 3, "AIG_HelpCivilian" },
            { 4, "AIG_HelpCivilianWeapon" },
            { 5, "AIG_Investigate" },
            { 6, "AIG_InvestigateDisguise" },
            { 7, "AIG_StandOffInvestigateDisguise" },
            { 8, "AIG_InvestigateDeadBody" },
            { 9, "AIG_HandleTrespasser" },
            { 10, "AIG_InvestigateCautious" },
            { 11, "AIG_HandleDistraction" },
            { 12, "AIG_AssistDistraction" },
            { 13, "AIG_GuardAssistDistraction" },
            { 14, "AIG_HandleSuitcase" },
            { 15, "AIG_InvestigateWeapon" },
            { 16, "AIG_DetectedInPrivate" },
            { 17, "AIG_RecoverUnconscious" },
            { 18, "AIG_RecoverUnconsciousAlerted" },
            { 19, "AIG_DeliverWeapon" },
            { 20, "AIG_AvoidExplosion" },
            { 21, "AIG_Hunt" },
            { 22, "AIG_Frisk" },
            { 23, "AIG_GuardBody" },
            { 24, "AIG_DragBody" },
            { 25, "AIG_LookAtAccident" },
            { 26, "AIG_Sentry" },
            { 27, "AIG_HelpGeneric" },
            { 28, "AIG_Evacuate" },
            { 29, "AIG_Entourage" },
            { 30, "AIG_AvoidDangerousArea" },
            { 31, "AIG_Spectator" },
            { 32, "AIG_Infected" },
            { 33, "AIG_SC_Combat" },
            { 34, "AIG_SC_Alerted" },
            { 35, "AIG_None" }
        };

        (*enum_map)["ECharacterResourceType"] = std::map<uint32_t, std::string>{
            { 0, "eSMT_Invalid" },
            { 1, "eSMT_FullBody" },
            { 2, "eSMT_RightHand" },
            { 3, "eSMT_LeftHand" }
        };

        (*enum_map)["ZHM5WeaponBasicConfigEntity.EWeaponBasicConfigAudioExoticType"] = std::map<uint32_t, std::string>{
            { 0, "eWBC_AudioExoticType_MiniPistol" },
            { 32768, "eWBC_AudioExoticType_None" },
            { 1, "eWBC_AudioExoticType_OneHandSpinReloadShotgun" },
            { 2, "eWBC_AudioExoticType_TranquilizerDartPistol" },
            { 3, "eWBC_AudioExoticType_Walter2000SniperRifle" },
            { 4, "eWBC_AudioExoticType_ShootingRange_NPCAssaultRifle" }
        };

        (*enum_map)["EDisturbanceType"] = std::map<uint32_t, std::string>{
            { 0, "DT_Generic" },
            { 1, "DT_SeeWeapon" },
            { 2, "DT_BodyNeedsBagging" },
            { 3, "DT_SoundCloseCombat" },
            { 4, "DT_SoundGunSilenced" },
            { 5, "DT_SoundBulletImpact" },
            { 6, "DT_SoundNeckBreak" },
            { 7, "DT_SoundSpeakPain" },
            { 8, "DT_SoundGunshot" },
            { 9, "DT_SoundExplosion" },
            { 10, "DT_SeeExplosion" },
            { 11, "DT_DeadBody" },
            { 12, "DT_HitmanNearDeadBody" },
            { 13, "DT_HitmanNearPacifiedBody" },
            { 14, "DT_SeeTakeKeyCard" },
            { 15, "DT_SeeTakeSecurityTape" },
            { 16, "DT_SeeChangeOutfit" },
            { 17, "DT_SeeTrespasser" },
            { 18, "DT_SeeBrokenDisguise" },
            { 19, "DT_SeeIllegalWeapon" },
            { 20, "DT_SeeDraggingBody" },
            { 21, "DT_SeeDeath" },
            { 22, "DT_SeeKilling" },
            { 23, "DT_UnconsciousVictim" },
            { 24, "DT_HuntTarget" },
            { 25, "DT_CrowdAlerted" },
            { 26, "DT_SeeGunshotVictim" },
            { 27, "DT_SoundThrownItem" },
            { 28, "DT_SoundThrownItemSeenThrowing" },
            { 29, "DT_HitByItemOrCoin" },
            { 30, "DT_PrivateAreaViolated" },
            { 31, "DT_SetpieceInteraction" },
            { 32, "DT_IllegalAction" },
            { 33, "DT_KnockdownMelee" },
            { 34, "DT_KnockdownThrow" },
            { 35, "DT_BumpedTooMuch" },
            { 36, "DT_Murder" },
            { 37, "DT_Strangle" },
            { 38, "DT_GotShot" },
            { 39, "DT_MurderAttempt" },
            { 40, "DT_LongTermMemory" },
            { 41, "DT_Accident" },
            { 42, "DT_MAX" }
        };

        (*enum_map)["IRenderMaterialEntity.EModifierOperation"] = std::map<uint32_t, std::string>{
            { 0, "eLeave" },
            { 128, "eColor" },
            { 1, "eReplace" },
            { 2, "eModulate" },
            { 3, "eAdd" }
        };

        (*enum_map)["ZBoxReflectionEntity.EType"] = std::map<uint32_t, std::string>{
            { 0, "eLocal" },
            { 1, "eFallback" }
        };

        (*enum_map)["ZGetHelpGroup.EInvestigateGroupState"] = std::map<uint32_t, std::string>{
            { 0, "IGS_Acknowledge" },
            { 1, "IGS_WaitForAckDialog" },
            { 2, "IGS_RunToGuard" },
            { 3, "IGS_ReportToHitman" },
            { 4, "IGS_ReportToGuard" },
            { 5, "IGS_StartMoveBack" },
            { 6, "IGS_MoveBack" },
            { 7, "IGS_Investigate" },
            { 8, "IGS_Completed" },
            { 9, "IGS_Max" }
        };

        (*enum_map)["ECollisionNotifyGroup"] = std::map<uint32_t, std::string>{
            { 0, "eCollisionNotifyGroup_Default" },
            { 1, "eCollisionNotifyGroup_CollisionListener" },
            { 2, "eCollisionNotifyGroup_PhysicsEntity" },
            { 3, "eCollisionNotifyGroup_DontNotify" },
            { 4, "eCollisionNotifyGroup_ForceNotify" }
        };

        (*enum_map)["ECharSetCharacterType"] = std::map<uint32_t, std::string>{
            { 0, "ECSCT_Actor" },
            { 1, "ECSCT_Nude" },
            { 2, "ECSCT_HeroA" }
        };

        (*enum_map)["EActorGroup"] = std::map<uint32_t, std::string>{
            { 0, "eAG_Group_A" },
            { 1, "eAG_Group_B" },
            { 2, "eAG_Group_C" },
            { 3, "eAG_Group_D" },
            { 4, "eAG_Count" }
        };

        (*enum_map)["InputControlNamesp.eHM5InputAction"] = std::map<uint32_t, std::string>{
            { 0, "eIAButtonFaceDown_Downedge" },
            { 1, "eIAButtonFaceDown_FastTap" },
            { 2, "eIAButtonFaceDown_HldDwnOnce" },
            { 3, "eIAButtonFaceDown_Hold" },
            { 4, "eIAButtonFaceDown_HoldDown" },
            { 5, "eIAButtonFaceLeft_Downedge" },
            { 6, "eIAButtonFaceLeft_HldDwnOnce" },
            { 7, "eIAButtonFaceLeft_Hold" },
            { 8, "eIAButtonFaceLeft_HoldDown" },
            { 9, "eIAButtonFaceLeft_FastTap" },
            { 10, "eIAButtonFaceLeft_Tap" },
            { 11, "eIAButtonFaceLeft_Release" },
            { 12, "eIAButtonFaceLeft_Repeat" },
            { 13, "eIAButtonFaceRight_Downedge" },
            { 14, "eIAButtonFaceRight_HldDwnOnce" },
            { 15, "eIAButtonFaceRight_Hold" },
            { 16, "eIAButtonFaceRight_HoldDown" },
            { 17, "eIAButtonFaceRight_FastTap" },
            { 18, "eIAButtonFaceRight_Tap" },
            { 19, "eIAButtonFaceRight_Release" },
            { 20, "eIAButtonFaceRight_Repeat" },
            { 21, "eIAButtonFaceUp_Downedge" },
            { 22, "eIAButtonFaceUp_FastTap" },
            { 23, "eIAButtonFaceUp_HldDwnOnce" },
            { 24, "eIAButtonFaceUp_Hold" },
            { 25, "eIAButtonFaceUp_HoldDown" },
            { 26, "eIABumperLeft_FastTap" },
            { 27, "eIABumperLeft_Downedge" },
            { 28, "eIABumperLeft_Hold" },
            { 29, "eIABumperLeft_Holddown" },
            { 30, "eIABumperLeft_Release" },
            { 31, "eIABumperRight_FastTap" },
            { 32, "eIABumperRight_Downedge" },
            { 33, "eIABumperRight_Hold" },
            { 34, "eIABumperRight_Holddown" },
            { 35, "eIABumperRight_Release" },
            { 36, "eIADpadUp_Downedge" },
            { 37, "eIADpadUp_HldDwnOnce" },
            { 38, "eIADpadUp_Hold" },
            { 39, "eIADpadDown_Hold" },
            { 40, "eIADpadUp_FastTap" },
            { 41, "eIADpadDown_Downedge" },
            { 42, "eIADpadDown_Release" },
            { 43, "eIADpadDown_FastTap" },
            { 44, "eIADpadDown_HldDwnOnce" },
            { 45, "eIADpadLeft_Downedge" },
            { 46, "eIADpadRight_Downedge" },
            { 47, "eIAThumbRight_Hold" },
            { 48, "eIAThumbRight_Downedge" },
            { 49, "eIAThumbLeft_Hold" },
            { 50, "eIAThumbLeft_DownEdge" },
            { 51, "eIATriggerLeft_Downedge" },
            { 52, "eIATriggerLeft_FastTap" },
            { 53, "eIATriggerLeft_Hold" },
            { 54, "eIATriggerLeft_Holddown" },
            { 55, "eIATriggerLeft_Release" },
            { 56, "eIATriggerRight_Downedge" },
            { 57, "eIATriggerRight_FastTap" },
            { 58, "eIATriggerRight_Hold" },
            { 59, "eIATriggerRight_Holddown" },
            { 60, "eIATriggerRight_Release" },
            { 61, "eIAGenericStart_Hold" },
            { 62, "eIAGCMeleeUse_Downedge" },
            { 63, "eIAGCMeleeUse_Hold" },
            { 64, "eIAGCMeleeUse_HoldDown" },
            { 65, "eIAGCMeleeUse_Release" },
            { 66, "eIAGCMeleeUse_HldDwnOnce" },
            { 67, "eIAGCMeleeUse_Repeat" },
            { 68, "eIAGCMeleeUse_Tap" },
            { 69, "eIAGCMeleeUse_FastTap" },
            { 70, "eIAGCAgilityDisguise_Downedge" },
            { 71, "eIAGCAgilityDisguise_Hold" },
            { 72, "eIAGCAgilityDisguise_HoldDown" },
            { 73, "eIAGCAgilityDisguise_Release" },
            { 74, "eIAGCAgilityDisguise_HldDwnOnce" },
            { 75, "eIAGCAgilityDisguise_Repeat" },
            { 76, "eIAGCAgilityDisguise_Tap" },
            { 77, "eIAGCAgilityDisguise_FastTap" },
            { 78, "eIAGCCoverDragBody_Downedge" },
            { 79, "eIAGCCoverDragBody_Hold" },
            { 80, "eIAGCCoverDragBody_HoldDown" },
            { 81, "eIAGCCoverDragBody_Release" },
            { 82, "eIAGCCoverDragBody_HldDwnOnce" },
            { 83, "eIAGCCoverDragBody_Repeat" },
            { 84, "eIAGCCoverDragBody_Tap" },
            { 85, "eIAGCCoverDragBody_FastTap" },
            { 86, "eIAGCInteractPickup_Downedge" },
            { 87, "eIAGCInteractPickup_Hold" },
            { 88, "eIAGCInteractPickup_HoldDown" },
            { 89, "eIAGCInteractPickup_Release" },
            { 90, "eIAGCInteractPickup_HldDwnOnce" },
            { 91, "eIAGCInteractPickup_Repeat" },
            { 92, "eIAGCInteractPickup_Tap" },
            { 93, "eIAGCInteractPickup_FastTap" },
            { 94, "eIAGCReloadInstinct_Downedge" },
            { 95, "eIAGCReloadInstinct_Hold" },
            { 96, "eIAGCReloadInstinct_HoldDown" },
            { 97, "eIAGCReloadInstinct_Release" },
            { 98, "eIAGCReloadInstinct_HldDwnOnce" },
            { 99, "eIAGCReloadInstinct_Repeat" },
            { 100, "eIAGCReloadInstinct_Tap" },
            { 101, "eIAGCReloadInstinct_FastTap" },
            { 102, "eIAGCRun_Downedge" },
            { 103, "eIAGCRun_Hold" },
            { 104, "eIAGCRun_HoldDown" },
            { 105, "eIAGCRun_Release" },
            { 106, "eIAGCRun_HldDwnOnce" },
            { 107, "eIAGCRun_Repeat" },
            { 108, "eIAGCRun_Tap" },
            { 109, "eIAGCRun_FastTap" },
            { 110, "eIAGCHolster_Downedge" },
            { 111, "eIAGCHolster_Hold" },
            { 112, "eIAGCHolster_HoldDown" },
            { 113, "eIAGCHolster_Release" },
            { 114, "eIAGCHolster_HldDwnOnce" },
            { 115, "eIAGCHolster_Repeat" },
            { 116, "eIAGCHolster_Tap" },
            { 117, "eIAGCHolster_FastTap" },
            { 118, "eIAGCInventory_Downedge" },
            { 119, "eIAGCInventory_Hold" },
            { 120, "eIAGCInventory_HoldDown" },
            { 121, "eIAGCInventory_Release" },
            { 122, "eIAGCInventory_HldDwnOnce" },
            { 123, "eIAGCInventory_Repeat" },
            { 124, "eIAGCInventory_Tap" },
            { 125, "eIAGCInventory_FastTap" },
            { 126, "eIAGCEmotes_Downedge" },
            { 127, "eIAGCEmotes_Hold" },
            { 128, "eIAGCEmotes_HoldDown" },
            { 129, "eIAGCEmotes_Release" },
            { 130, "eIAGCEmotes_HldDwnOnce" },
            { 131, "eIAGCEmotes_Repeat" },
            { 132, "eIAGCEmotes_Tap" },
            { 133, "eIAGCEmotes_FastTap" },
            { 134, "eIAGCDropItem_Downedge" },
            { 135, "eIAGCDropItem_Hold" },
            { 136, "eIAGCDropItem_HoldDown" },
            { 137, "eIAGCDropItem_Release" },
            { 138, "eIAGCDropItem_HldDwnOnce" },
            { 139, "eIAGCDropItem_Repeat" },
            { 140, "eIAGCDropItem_Tap" },
            { 141, "eIAGCDropItem_FastTap" },
            { 142, "eIAGCCrouch_Downedge" },
            { 143, "eIAGCCrouch_Hold" },
            { 144, "eIAGCCrouch_HoldDown" },
            { 145, "eIAGCCrouch_Release" },
            { 146, "eIAGCCrouch_HldDwnOnce" },
            { 147, "eIAGCCrouch_Repeat" },
            { 148, "eIAGCCrouch_Tap" },
            { 149, "eIAGCCrouch_FastTap" },
            { 150, "eIAGCCameraShoulder_Downedge" },
            { 151, "eIAGCCameraShoulder_Hold" },
            { 152, "eIAGCCameraShoulder_HoldDown" },
            { 153, "eIAGCCameraShoulder_Release" },
            { 154, "eIAGCCameraShoulder_HldDwnOnce" },
            { 155, "eIAGCCameraShoulder_Repeat" },
            { 156, "eIAGCCameraShoulder_Tap" },
            { 157, "eIAGCCameraShoulder_FastTap" },
            { 158, "eIAKBMMoveUp" },
            { 159, "eIAKBMMoveDown" },
            { 160, "eIAKBMMoveLeft" },
            { 161, "eIAKBMMoveRight" },
            { 162, "eIAKBMMarkTarget" },
            { 163, "eIAKBMNextAmmo" },
            { 164, "eIAKBMPreviousAmmo" },
            { 165, "eIAKBMUse" },
            { 166, "eIAKBMUse2" },
            { 167, "eIAKBMUse2Hold" },
            { 168, "eIAKBMSlideDown" },
            { 169, "eIAKBMPickup" },
            { 170, "eIAKBMCover" },
            { 171, "eIAKBMDragBody" },
            { 172, "eIAKBMMelee" },
            { 173, "eIAKBMMelee_Repeat" },
            { 174, "eIAKBMActivateProp" },
            { 175, "eIAKBMActivateProp_HldDwnOnce" },
            { 176, "eIAKBMActivateProp_Hold" },
            { 177, "eIAKBMActivatePropSecondary" },
            { 178, "eIAKBMConcealRetrieve" },
            { 179, "eIAKBMTakeDisguise" },
            { 180, "eIAKBMRun" },
            { 181, "eIAKBMWalkSlow" },
            { 182, "eIAKBMAim" },
            { 183, "eIAKBMShoot" },
            { 184, "eIAKBMShoot_Downedge" },
            { 185, "eIAKBMShoot_Release" },
            { 186, "eIAKBMReload" },
            { 187, "eIAKBMInstinct" },
            { 188, "eIAKBMCamSwitch" },
            { 189, "eIAKBMSneak" },
            { 190, "eIAKBMHolster" },
            { 191, "eIAKBMFireMode" },
            { 192, "eIAKBMNotebook" },
            { 193, "eIAKBMNotebookMap" },
            { 194, "eIAKBMPause" },
            { 195, "eIAKBMWeapon1" },
            { 196, "eIAKBMWeapon2" },
            { 197, "eIAKBMWeapon3" },
            { 198, "eIAKBMWeapon4" },
            { 199, "eIAKBMWeapon5" },
            { 200, "eIAKBMWeapon6" },
            { 201, "eIAKBMWeapon7" },
            { 202, "eIAKBMPrecisionAim" },
            { 203, "eIAKBMZoomIn" },
            { 204, "eIAKBMZoomOut" },
            { 205, "eIAKBMDropItem" },
            { 206, "eIAKBMInventory" },
            { 207, "eIAKBMEmotes" },
            { 208, "eIAKBMSurrender" },
            { 209, "eIAKBMSurrender_Hold" },
            { 210, "eIAKBMSurrender_HoldDown" },
            { 211, "eIAKBMSurrender_HoldDownOnce" },
            { 212, "eIAKBMTogglePlacement" },
            { 213, "eIAKBMOkay" },
            { 214, "eIAKBMCancel" },
            { 215, "eIAKBMAccept" },
            { 216, "eIATriggerLeft_Analog" },
            { 217, "eIATriggerRight_Analog" },
            { 218, "eIAStickLeftHorizontal_Analog" },
            { 219, "eIAStickLeftVertical_Analog" },
            { 220, "eIAStickRightHorizontal_Analog" },
            { 221, "eIAStickRightVertical_Analog" },
            { 222, "eIAStickLeftHorizontal_Raw" },
            { 223, "eIAStickLeftVertical_Raw" },
            { 224, "eIAStickRightHorizontal_Raw" },
            { 225, "eIAStickRightVertical_Raw" },
            { 226, "eIABumperLeft_Analog" },
            { 227, "eIABumperRight_Analog" },
            { 228, "eIAKBMMoveHorizontal" },
            { 229, "eIAKBMMoveVertical" },
            { 230, "eIAKBMLookHorizontal" },
            { 231, "eIAKBMLookVertical" },
            { 232, "eIActionsNUM" }
        };

        (*enum_map)["EAISharedEventType"] = std::map<uint32_t, std::string>{
            { 0, "AISET_None" },
            { 399, "AISET_DynamicEnforcer_PacifyingThrowVictim" },
            { 1, "AISET_StandOffCrowdReactionDone" },
            { 2, "AISET_Shoot" },
            { 393, "AISET_DYNAMIC_ENFORCER_FLAG_INCA_BEGIN" },
            { 3, "AISET_ShootBlind" },
            { 4, "AISET_Scared" },
            { 387, "AISET_DynamicEnforcer_SeenItemThrown" },
            { 5, "AISET_CloseCombatRunIn" },
            { 6, "AISET_CloseCombatArrest" },
            { 397, "AISET_DynamicEnforcer_CloseCombatSound" },
            { 7, "AISET_ReadyForCloseCombatArrest" },
            { 8, "AISET_DeadByAccident" },
            { 407, "AISET_DYNAMIC_ENFORCER_FLAG_END" },
            { 9, "AISET_SituationConversation" },
            { 10, "AISET_LAST_BOOLEAN_EVENT" },
            { 401, "AISET_DynamicEnforcer_SeenMurderVictimHeardGunshot" },
            { 11, "AISET_IllegalBehavior" },
            { 12, "AISET_BeingWatched" },
            { 395, "AISET_DynamicEnforcer_GunshotSound" },
            { 13, "AISET_Sneaking" },
            { 14, "AISET_Squatting" },
            { 405, "AISET_DynamicEnforcer_Hunt" },
            { 15, "AISET_Agility" },
            { 16, "AISET_IllegalAgility" },
            { 415, "AISET_DynamicEnforcer_ArrestReason_Murder" },
            { 17, "AISET_ChangingOutfit" },
            { 18, "AISET_ChangedOutfit" },
            { 409, "AISET_DynamicEnforcer_Disable_CivilianInInvestigateCautious" },
            { 19, "AISET_CoverToCover" },
            { 20, "AISET_Trespassing" },
            { 403, "AISET_DynamicEnforcer_DeadBody" },
            { 21, "AISET_InTrespassZone" },
            { 22, "AISET_InDisguiseZone" },
            { 413, "AISET_DYNAMIC_ENFORCER_CIVILIAN_DISGUISE_ONLY_FLAG_END" },
            { 23, "AISET_DraggingBody" },
            { 24, "AISET_KillingWithFiberwire" },
            { 25, "AISET_KillingWithPull" },
            { 26, "AISET_KillingWithPush" },
            { 27, "AISET_KillingWithKick" },
            { 28, "AISET_KillingWithKnife" },
            { 411, "AISET_DYNAMIC_ENFORCER_CIVILIAN_DISGUISE_ONLY_FLAG_BEGIN" },
            { 29, "AISET_KillingWithDeadlyThrow" },
            { 30, "AISET_PullingFromLedge" },
            { 31, "AISET_PacifyingWithPull" },
            { 32, "AISET_PacifyingWithPush" },
            { 33, "AISET_PacifyingWithKick" },
            { 34, "AISET_PacifyingWithKnockDownThrow" },
            { 425, "AISET_DynamicEnforcer_ArrestReason_PrivateArea" },
            { 35, "AISET_PacifyingWithStrangle" },
            { 36, "AISET_PacifyingWithKnife" },
            { 419, "AISET_DynamicEnforcer_ArrestReason_NearPacifiedBody" },
            { 37, "AISET_ThrowingSomething" },
            { 38, "AISET_ThrowingExplosives" },
            { 429, "AISET_DynamicEnforcer_ArrestReason_SillyHitman" },
            { 39, "AISET_KnockingDown" },
            { 40, "AISET_InCloset" },
            { 41, "AISET_InNpcUnreachableArea" },
            { 42, "AISET_Peeking" },
            { 43, "AISET_EnteringCloset" },
            { 44, "AISET_ExitingCloset" },
            { 427, "AISET_DynamicEnforcer_ArrestReason_Setpiece" },
            { 45, "AISET_InCover" },
            { 46, "AISET_MovingInCover" },
            { 47, "AISET_HiddenInCover" },
            { 48, "AISET_HasWeapon" },
            { 49, "AISET_WeaponEquipped" },
            { 50, "AISET_WeaponFired" },
            { 51, "AISET_WeaponReloading" },
            { 52, "AISET_Aiming" },
            { 53, "AISET_GuardDisguise" },
            { 54, "AISET_PulledByHitmanOnLedge" },
            { 55, "AISET_CombatNoHold" },
            { 56, "AISET_CombatFireAtWill" },
            { 391, "AISET_DynamicEnforcer_PrivateArea" },
            { 57, "AISET_InOrderedPosition" },
            { 58, "AISET_ORDERS_BEGIN" },
            { 385, "AISET_DynamicEnforcer_SharedHitmanSensor" },
            { 59, "AISET_Order_Act" },
            { 60, "AISET_Order_Investigate" },
            { 61, "AISET_Order_SearchPosition" },
            { 62, "AISET_Order_Approach" },
            { 389, "AISET_DynamicEnforcer_Setpiece" },
            { 63, "AISET_Order_GetHelp" },
            { 64, "AISET_Order_StandDown" },
            { 65, "AISET_Order_Escalate" },
            { 66, "AISET_Order_Arrest" },
            { 67, "AISET_Order_Avoid" },
            { 68, "AISET_Order_Flee" },
            { 69, "AISET_Order_Patrol" },
            { 70, "AISET_Order_Reposition" },
            { 71, "AISET_Order_Chase" },
            { 72, "AISET_Order_AimFromPosition" },
            { 73, "AISET_Order_HoldPosition" },
            { 74, "AISET_Order_HoldCoverPosition" },
            { 75, "AISET_Order_FightFromCover" },
            { 76, "AISET_Order_FightFromPosition" },
            { 77, "AISET_Order_ShootFromPosition" },
            { 78, "AISET_Order_GuardPosition" },
            { 79, "AISET_Order_GuardBody" },
            { 80, "AISET_Order_WakeUpUnconscious" },
            { 81, "AISET_Order_DragBody" },
            { 82, "AISET_Order_TriggerAlarm" },
            { 83, "AISET_Order_DispatchSuspectInfo" },
            { 84, "AISET_Order_RecieveSuspectInfo" },
            { 85, "AISET_Order_RequestBackup" },
            { 86, "AISET_Order_ThrowFlashGrenade" },
            { 87, "AISET_Order_CheckLastPosition" },
            { 88, "AISET_Order_StandoffLastWarning" },
            { 89, "AISET_Order_Evacuate" },
            { 90, "AISET_Order_AgitatedBystander" },
            { 91, "AISET_Order_CuriousBystander" },
            { 92, "AISET_Order_Face" },
            { 93, "AISET_Order_Holster" },
            { 94, "AISET_Order_Unholster" },
            { 95, "AISET_Order_MoveTo" },
            { 96, "AISET_Order_JumpTo" },
            { 97, "AISET_Order_FormationMove" },
            { 98, "AISET_Order_SentryFrisk" },
            { 99, "AISET_Order_SentryWarning" },
            { 100, "AISET_Order_SentryGreeting" },
            { 101, "AISET_Order_SentryCheckItem" },
            { 102, "AISET_Order_SentryDemandPickUpItem" },
            { 103, "AISET_Order_DeliverWeapon" },
            { 104, "AISET_Order_RecoverUnconscious" },
            { 105, "AISET_Order_GetOutfit" },
            { 106, "AISET_Order_LockdownWarning" },
            { 107, "AISET_Order_VIPEvacuateIdle" },
            { 108, "AISET_Order_VIPSafeRoom" },
            { 109, "AISET_Order_VIPSafeRoomTrespasser" },
            { 110, "AISET_Order_VIPScared" },
            { 111, "AISET_Order_DefendVIP" },
            { 112, "AISET_Order_CautiousVIP" },
            { 113, "AISET_Order_CautiousGuardVIP" },
            { 114, "AISET_Order_EscortOut" },
            { 115, "AISET_Order_MoveInCover" },
            { 116, "AISET_Order_MoveToCover" },
            { 117, "AISET_Order_Conversation" },
            { 118, "AISET_Order_AttentionToPerson" },
            { 119, "AISET_Order_WaitForItemHandled" },
            { 120, "AISET_ORDERS_END" },
            { 121, "AISET_IMPULSES_BEGIN" },
            { 122, "AISET_Impulse_ReceiveHuntTarget" },
            { 123, "AISET_Impulse_AckHuntTarget" },
            { 124, "AISET_Impulse_TransmitHuntTarget" },
            { 125, "AISET_Impulse_ActiveRadioCall" },
            { 126, "AISET_IMPULSES_END" },
            { 127, "AISET_ForceFullAttention" },
            { 128, "AISET_InCameraView2D" },
            { 129, "AISET_BeingLookedAt" },
            { 314, "AISET_SoundHitmanGunShot" },
            { 130, "AISET_BeingAimedAt" },
            { 131, "AISET_WasAimedAtRecently" },
            { 260, "AISET_RequestShowItem" },
            { 132, "AISET_BeingShotAt" },
            { 133, "AISET_BeingActuallyShotAt" },
            { 318, "AISET_ManhuntMember" },
            { 134, "AISET_BeingFlanked" },
            { 135, "AISET_BeingTouched" },
            { 312, "AISET_SilentlyKilling" },
            { 136, "AISET_BeingBumped" },
            { 137, "AISET_BeingEscortedOut" },
            { 258, "AISET_ToldToEvacuate" },
            { 138, "AISET_EscortingOut" },
            { 139, "AISET_EscortOutFarewell" },
            { 268, "AISET_Attached" },
            { 140, "AISET_HMTrespassing" },
            { 141, "AISET_HMInEntryArea" },
            { 262, "AISET_WokenByNPC" },
            { 142, "AISET_HMTakingDamage" },
            { 143, "AISET_HumanShield" },
            { 256, "AISET_ReportedDisturbance" },
            { 144, "AISET_IllegalAction" },
            { 145, "AISET_IllegalActionSevere" },
            { 266, "AISET_DontMakeFunPlease" },
            { 146, "AISET_TakingKeyCard" },
            { 147, "AISET_TakingSecurityTape" },
            { 276, "AISET_Pacified" },
            { 148, "AISET_DumpingBody" },
            { 149, "AISET_Shooting" },
            { 270, "AISET_Sentry" },
            { 150, "AISET_BlindFire" },
            { 151, "AISET_ShootingBallet" },
            { 264, "AISET_TriggerLockDownAlarm" },
            { 152, "AISET_Running" },
            { 153, "AISET_Walking" },
            { 274, "AISET_CloseCombatFinisher" },
            { 154, "AISET_UsingKeyCard" },
            { 155, "AISET_WeaponDropped" },
            { 284, "AISET_DraggingBodyBag" },
            { 156, "AISET_WeaponPickedUp" },
            { 157, "AISET_ItemPickedUp" },
            { 278, "AISET_FlashpointSeen" },
            { 158, "AISET_ItemDropped" },
            { 159, "AISET_Surrendering" },
            { 272, "AISET_FriskingSentry" },
            { 160, "AISET_RequestSurrender" },
            { 161, "AISET_StandOffEscalated" },
            { 162, "AISET_StandOffInvestigateSevere" },
            { 163, "AISET_Standing" },
            { 292, "AISET_Combat_CloseToGuards" },
            { 164, "AISET_Enforcer" },
            { 165, "AISET_DisguiseEnforcer" },
            { 166, "AISET_Dead" },
            { 167, "AISET_DeadByUnnoticed" },
            { 168, "AISET_DeadBySniper" },
            { 169, "AISET_InMorgue" },
            { 290, "AISET_Combat_PrimaryRange" },
            { 170, "AISET_InAccident" },
            { 171, "AISET_Dying" },
            { 300, "AISET_Wounded" },
            { 172, "AISET_InInvestigateDisguise" },
            { 173, "AISET_InArrest" },
            { 294, "AISET_Combat_TargetSeenAtLeastOnce" },
            { 174, "AISET_Arrestor" },
            { 175, "AISET_InCombat" },
            { 288, "AISET_Combat_Brave" },
            { 176, "AISET_InCombatWithGuards" },
            { 177, "AISET_Hunted" },
            { 298, "AISET_InSentrySituation" },
            { 178, "AISET_ForceHunted" },
            { 179, "AISET_CloseCombat" },
            { 308, "AISET_InvestigateDisguiseGroupLeader" },
            { 180, "AISET_CloseCombatSlashing" },
            { 181, "AISET_TakingProjectileDamage" },
            { 302, "AISET_ReactedToTossAlerted" },
            { 182, "AISET_TakingHeadshotDamage" },
            { 183, "AISET_TakingExplosionDamage" },
            { 296, "AISET_InTheAir" },
            { 184, "AISET_TakingThrowDamage" },
            { 185, "AISET_ForceKillHM" },
            { 306, "AISET_InvestigatingMurder" },
            { 186, "AISET_Armed" },
            { 187, "AISET_BeingHumanShield" },
            { 316, "AISET_ChangingOutfitFromBody" },
            { 188, "AISET_BeingPulledOutOfWindow" },
            { 189, "AISET_BeingPushedOverRail" },
            { 310, "AISET_BodyguardVIPDead" },
            { 190, "AISET_BeingFiberwired" },
            { 191, "AISET_BeingPulledFromLedge" },
            { 304, "AISET_GettingHelp" },
            { 192, "AISET_BeingDragged" },
            { 193, "AISET_OnScreen" },
            { 378, "AISET_Infected_Stage2" },
            { 194, "AISET_ExplosionImminent" },
            { 195, "AISET_DisguiseSafeZone" },
            { 324, "AISET_StrangleVictim" },
            { 196, "AISET_DisguiseCanBlendIn" },
            { 197, "AISET_DisguiseBlendIn" },
            { 382, "AISET_Enforcer_Disable_HideInPlainSight" },
            { 198, "AISET_DisguiseBroken" },
            { 199, "AISET_InWarZone" },
            { 376, "AISET_SC_Investigator" },
            { 200, "AISET_HearLockdownAlarm" },
            { 201, "AISET_InCrowd" },
            { 322, "AISET_HasAmbientEscort" },
            { 202, "AISET_InCrowdHighDensity" },
            { 203, "AISET_InCrowdAlert" },
            { 332, "AISET_RadioCallHelpArrived" },
            { 204, "AISET_InCrowdPanic" },
            { 205, "AISET_HiddenByCrowd" },
            { 326, "AISET_Jumping" },
            { 206, "AISET_CrowdDisguise" },
            { 207, "AISET_BeingStrangled" },
            { 320, "AISET_EvacuateAfterRecover" },
            { 208, "AISET_BeingSnapNecked" },
            { 209, "AISET_BeingKnockedDown" },
            { 330, "AISET_RadioCallHelpComming" },
            { 210, "AISET_BeingContextKilled" },
            { 211, "AISET_CloseCombatTakedown" },
            { 340, "AISET_LongTermMemorySetGoal" },
            { 212, "AISET_ContextKill" },
            { 213, "AISET_ExecutePistol" },
            { 334, "AISET_VIPAtEscapeNode" },
            { 214, "AISET_PickingUpGuardedItem" },
            { 215, "AISET_SedativePoison" },
            { 328, "AISET_VIPHandleOwnCuriousInvestigation" },
            { 216, "AISET_SpawnedActor" },
            { 217, "AISET_SoundRadio" },
            { 338, "AISET_HMHasVictim" },
            { 218, "AISET_Weapon" },
            { 219, "AISET_Item" },
            { 348, "AISET_HMStunnedByNPCFlashGrenade" },
            { 220, "AISET_ItemThrown" },
            { 221, "AISET_OwnedByHM" },
            { 342, "AISET_Dropped" },
            { 222, "AISET_Coin" },
            { 223, "AISET_InLimitVisionAreaZone" },
            { 336, "AISET_InterestingLocation" },
            { 224, "AISET_InsideLimitVisionArea" },
            { 225, "AISET_BlockedByLimitVisionArea" },
            { 282, "AISET_LockedInAgility" },
            { 226, "AISET_AmbientOnly" },
            { 227, "AISET_HasBeenInCombat" },
            { 356, "AISET_Distraction_State_LeaderSelected" },
            { 228, "AISET_HitByItem" },
            { 229, "AISET_CloseCombatAllowed" },
            { 286, "AISET_VaultingOverObstacle" },
            { 230, "AISET_CollateralTarget" },
            { 231, "AISET_HuntAct" },
            { 280, "AISET_WeaponContextKnown" },
            { 232, "AISET_Sick" },
            { 233, "AISET_Stunned" },
            { 354, "AISET_ForcedLeader" },
            { 234, "AISET_Naked" },
            { 235, "AISET_Investigated" },
            { 364, "AISET_Arrest_ArrestReason" },
            { 236, "AISET_FreshBody" },
            { 237, "AISET_SeenDeadlyThrowVictim" },
            { 358, "AISET_Distraction_State_Resolved" },
            { 238, "AISET_IWillBeDeadlyThrowVictim" },
            { 239, "AISET_Suspect" },
            { 352, "AISET_ReadytoPlayInvestigationConversation" },
            { 240, "AISET_BeingFrisked" },
            { 241, "AISET_RequestFrisk" },
            { 362, "AISET_Arrest_State_Escorting" },
            { 242, "AISET_FriskComplying" },
            { 243, "AISET_FriskApproachReady" },
            { 372, "AISET_SC_PreventShooting" },
            { 244, "AISET_FriskReady" },
            { 245, "AISET_Frisked" },
            { 366, "AISET_Arrest_Warning_2" },
            { 246, "AISET_FriskFailed" },
            { 247, "AISET_FriskFoundWeapon" },
            { 360, "AISET_Distraction_Target_BeingHandled" },
            { 248, "AISET_AntiSocial" },
            { 249, "AISET_Introvert" },
            { 370, "AISET_SC_InCombatPosition" },
            { 250, "AISET_InLockdownZone" },
            { 251, "AISET_EnteringLockdownZone" },
            { 380, "AISET_Infection_Cured" },
            { 252, "AISET_InSentryZone" },
            { 253, "AISET_InSentryWarningZone" },
            { 374, "AISET_SC_CombatAct" },
            { 254, "AISET_InSentryFriskWarningZone" },
            { 255, "AISET_SuppressTrespassing" },
            { 368, "AISET_Arrest_Warning_Delay" },
            { 257, "AISET_InLockdown" },
            { 259, "AISET_CurrentTarget" },
            { 261, "AISET_RequestComplying" },
            { 263, "AISET_Silly" },
            { 265, "AISET_CombatFromEvacuate" },
            { 267, "AISET_TempCrowdKnowledge" },
            { 269, "AISET_DontReact" },
            { 271, "AISET_HardSentry" },
            { 273, "AISET_Guarded" },
            { 275, "AISET_WasInCloseCombatSawAggressor" },
            { 277, "AISET_SomeoneIsSniping" },
            { 279, "AISET_SniperLocation" },
            { 281, "AISET_Distracted" },
            { 283, "AISET_ActorAlreadyAlive" },
            { 285, "AISET_UncosciousBlamingFailed" },
            { 287, "AISET_ClimbingWindow" },
            { 289, "AISET_Combat_OneTimeBrave" },
            { 291, "AISET_Combat_ThrewFlashGrenade" },
            { 293, "AISET_Combat_RecentlySeen" },
            { 295, "AISET_InTheAirDeadly" },
            { 297, "AISET_Landing" },
            { 299, "AISET_LeftDramaWithHM" },
            { 301, "AISET_RecoveredFromWounded" },
            { 303, "AISET_ReactedToHitAlerted" },
            { 305, "AISET_HelpingCivilian" },
            { 307, "AISET_InvestigatingMurderOrGunshot" },
            { 309, "AISET_InvestigateDisguiseMovingBack" },
            { 311, "AISET_SilentlyKilled" },
            { 313, "AISET_SoundGunShot" },
            { 315, "AISET_SoundHitmanGunShotSilenced" },
            { 317, "AISET_SpeedBoost" },
            { 319, "AISET_InvestigateDisguiseMember" },
            { 321, "AISET_Woozy" },
            { 323, "AISET_IsEscortingVIP" },
            { 325, "AISET_DontStandDownIfIdling" },
            { 327, "AISET_GetHelpDone" },
            { 329, "AISET_InDangerousArea" },
            { 331, "AISET_RadioCallHelpFailed" },
            { 333, "AISET_SocialCooldown" },
            { 335, "AISET_VIPEscaping" },
            { 337, "AISET_SeeDeathExplained" },
            { 339, "AISET_PrivateSituationLeader" },
            { 341, "AISET_ThrownByHitman" },
            { 343, "AISET_InterestingItem" },
            { 344, "AISET_InEvacuateTrespassGroup" },
            { 345, "AISET_InRecoverUnconsciousGroup" },
            { 346, "AISET_Revived" },
            { 347, "AISET_IsBeggingForLife" },
            { 349, "AISET_Situation_Role_Leader" },
            { 350, "AISET_Situation_Role_Assistant" },
            { 351, "AISET_Situation_Role_AssitingGuard" },
            { 353, "AISET_ReadytoPlayStandDownConversation" },
            { 355, "AISET_PropagatedByCivilian" },
            { 357, "AISET_Distraction_State_Investigated" },
            { 359, "AISET_Distraction_State_Canceled" },
            { 361, "AISET_Arrest_State_Approaching" },
            { 363, "AISET_Arrest_State_Guarding" },
            { 365, "AISET_Arrest_Warning_1" },
            { 367, "AISET_Arrest_Warning_3" },
            { 369, "AISET_SC_Escalating" },
            { 371, "AISET_SC_Shoot" },
            { 373, "AISET_SC_PreventChangeCombatPosition" },
            { 375, "AISET_SC_InCombat" },
            { 377, "AISET_Infected" },
            { 379, "AISET_Infected_Stage3" },
            { 381, "AISET_ENFORCER_DISABLE_FLAG_BEGIN" },
            { 383, "AISET_ENFORCER_DISABLE_FLAG_END" },
            { 384, "AISET_DYNAMIC_ENFORCER_FLAG_BEGIN" },
            { 386, "AISET_DynamicEnforcer_NakedActor" },
            { 388, "AISET_DynamicEnforcer_HitByItemOrCoin" },
            { 390, "AISET_DynamicEnforcer_SillyHitman" },
            { 392, "AISET_DynamicEnforcer_SuspiciousMovement" },
            { 394, "AISET_DynamicEnforcer_Explosion" },
            { 396, "AISET_DynamicEnforcer_BulletImpactSound" },
            { 398, "AISET_DynamicEnforcer_DeadlyThrowVictim" },
            { 400, "AISET_DynamicEnforcer_SeenMurderVictimNotHeardGunshot" },
            { 402, "AISET_DynamicEnforcer_MurderAttempt" },
            { 404, "AISET_DynamicEnforcer_PacifiedBody" },
            { 406, "AISET_DYNAMIC_ENFORCER_FLAG_INCA_END" },
            { 408, "AISET_DYNAMIC_ENFORCER_DISABLE_FLAG_BEGIN" },
            { 410, "AISET_DYNAMIC_ENFORCER_DISABLE_FLAG_END" },
            { 412, "AISET_DynamicEnforcer_InvestigateWeapon" },
            { 414, "AISET_DYNAMIC_ENFORCER_ARREST_REASON_BEGIN" },
            { 416, "AISET_DynamicEnforcer_ArrestReason_MurderAttempt" },
            { 417, "AISET_DynamicEnforcer_ArrestReason_HitByItemOrCoin" },
            { 418, "AISET_DynamicEnforcer_ArrestReason_NearDeadBody" },
            { 420, "AISET_DynamicEnforcer_ArrestReason_GunshotSound" },
            { 421, "AISET_DynamicEnforcer_ArrestReason_CloseCombatSound" },
            { 422, "AISET_DynamicEnforcer_ArrestReason_BulletImpactSound" },
            { 423, "AISET_DynamicEnforcer_ArrestReason_Hunt" },
            { 424, "AISET_DynamicEnforcer_ArrestReason_Explosion" },
            { 426, "AISET_DynamicEnforcer_ArrestReason_Weapon" },
            { 428, "AISET_DynamicEnforcer_ArrestReason_KnockdownThrow" },
            { 430, "AISET_DYNAMIC_ENFORCER_ARREST_REASON_END" }
        };

        (*enum_map)["ZPathfinderDebugEntity.EDebugTestType"] = std::map<uint32_t, std::string>{
            { 0, "PATHFINDER" },
            { 1, "TRACELINE" },
            { 2, "MAP_LOCATION" },
            { 3, "REGION_MASK_WITHIN_RADIUS" }
        };

        (*enum_map)["ECombatZoneState"] = std::map<uint32_t, std::string>{
            { 0, "CZS_Inactive" },
            { 1, "CZS_Triggered" },
            { 2, "CZS_Engaged" }
        };

        (*enum_map)["ELocale"] = std::map<uint32_t, std::string>{
            { 0, "Locale_En" },
            { 1, "Locale_Fr" },
            { 2, "Locale_It" },
            { 3, "Locale_De" },
            { 4, "Locale_Es" },
            { 5, "Locale_Ru" },
            { 6, "Locale_Mx" },
            { 7, "Locale_Br" },
            { 8, "Locale_Pl" },
            { 9, "Locale_Cn" },
            { 10, "Locale_Jp" },
            { 11, "Locale_Tc" }
        };

        (*enum_map)["EGameUIMenu"] = std::map<uint32_t, std::string>{
            { 0, "eUIMenu_IntroMenu" },
            { 1, "eUIMenu_PauseMenu" },
            { 2, "eUIMenu_PhoneMenu" },
            { 3, "eUIMenu_MapMenu" },
            { 4, "eUIMenu_ModalDialogMenu" },
            { 5, "eUIMenu_GameplayMenu" },
            { 6, "eUIMenu_InventorySelectorMenu" },
            { 7, "eUIMenu_ActionSelectorMenu" },
            { 8, "eUIMenu_ActionSelectorInventoryMenu" },
            { 9, "eUIMenu_IntelSelectorMenu" },
            { 10, "eUIMenu_NotebookMenu" },
            { 11, "eUIMenu_LogbookMenu" },
            { 12, "eUIMenu_TutorialMenu" },
            { 13, "eUIMenu_SplashHintMenu" },
            { 14, "eUIMenu_EmoteMenu" },
            { 15, "eUIMenu_Count" }
        };

        (*enum_map)["EAutoScanMode"] = std::map<uint32_t, std::string>{
            { 0, "ASM_GEOMETRY" },
            { 1, "ASM_VOLUMEBOX" }
        };

        (*enum_map)["ENetRole"] = std::map<uint32_t, std::string>{
            { 0, "None" },
            { 1, "Master" },
            { 2, "Client" }
        };

        (*enum_map)["EActorCCPreset"] = std::map<uint32_t, std::string>{
            { 0, "ACCP_CivilianFemale" },
            { 1, "ACCP_CivilianeMale" },
            { 2, "ACCP_GuardMale" },
            { 3, "ACCP_GuardMaleElite" },
            { 4, "ACCP_GuardMaleSuperElite" },
            { 5, "ACCP_Ignore" },
            { 6, "ACCP_None" }
        };

        (*enum_map)["EItemRequirement"] = std::map<uint32_t, std::string>{
            { 0, "eIR_NoRequirement" },
            { 1, "eIR_RequireEmpty" },
            { 2, "eIR_RequireItem" },
            { 3, "eIR_RequireNoBigItem" }
        };

        (*enum_map)["EClothColliderFilter"] = std::map<uint32_t, std::string>{
            { 0, "EClothColliderFilter_Environment" },
            { 1, "EClothColliderFilter_Character" },
            { 2, "EClothColliderFilter_Character_Environment" }
        };

        (*enum_map)["EVsGenericEvent"] = std::map<uint32_t, std::string>{
            { 0, "eVSGE_MATCH_WON" },
            { 1, "eVSGE_MATCH_LOST" },
            { 2, "eVSGE_MATCH_DRAW" },
            { 3, "eVSGE_TARGET_LEAVING" },
            { 4, "eVSGE_TARGET_LEFT" },
            { 5, "eVSGE_TARGET_ARRIVING" },
            { 6, "eVSGE_TARGET_ARRIVED" },
            { 7, "eVSGE_RESPAWNED" },
            { 8, "eVSGE_OPPONENT_DEAD" },
            { 9, "eVSGE_OPPONENT_RESPAWNED" },
            { 10, "eVSGE_OPPONENT_GOT_WEAPON" },
            { 11, "eVSGE_OPPONENT_GOT_ITEM" },
            { 12, "eVSGE_OPPONENT_DISGUISED" },
            { 13, "eVSGE_TARGET_MISSED" },
            { 14, "eVSGE_NEXT_TARGET_MISSED" },
            { 15, "eVSGE_OPPONENT_TARGET_MISSED" },
            { 16, "eVSGE_OPPONENT_NEXT_TARGET_MISSED" },
            { 17, "eVSGE_OPPONENT_FAR_FROM_TARGET" },
            { 18, "eVSGE_OPPONENT_CLOSE_FROM_TARGET" },
            { 19, "eVSGE_OPPONENT_SCOPES_TARGET" },
            { 20, "eVSGE_MAX" }
        };

        (*enum_map)["ESmartBehaviorOrder"] = std::map<uint32_t, std::string>{
            { 0, "SBO_Stand" },
            { 1, "SBO_Move" },
            { 2, "SBO_React" },
            { 3, "SBO_Act" },
            { 4, "SBO_MoveToAct" },
            { 5, "SBO_MoveToCover" },
            { 6, "SBO_ShootFromCover" },
            { 7, "SBO_Death" },
            { 8, "SBO_Teleport" }
        };

        (*enum_map)["ZRagdollAttacherEntity.EResetMode"] = std::map<uint32_t, std::string>{
            { 0, "eAtBoneOrigin" },
            { 1, "eKeepOffset" }
        };
    }
}

std::string DirectXFormatsStrings[116] = { "DXGI_FORMAT_UNKNOWN",
"DXGI_FORMAT_R32G32B32A32_TYPELESS",
"DXGI_FORMAT_R32G32B32A32_FLOAT",
"DXGI_FORMAT_R32G32B32A32_UINT",
"DXGI_FORMAT_R32G32B32A32_SINT",
"DXGI_FORMAT_R32G32B32_TYPELESS",
"DXGI_FORMAT_R32G32B32_FLOAT",
"DXGI_FORMAT_R32G32B32_UINT",
"DXGI_FORMAT_R32G32B32_SINT",
"DXGI_FORMAT_R16G16B16A16_TYPELESS",
"DXGI_FORMAT_R16G16B16A16_FLOAT",
"DXGI_FORMAT_R16G16B16A16_UNORM",
"DXGI_FORMAT_R16G16B16A16_UINT",
"DXGI_FORMAT_R16G16B16A16_SNORM",
"DXGI_FORMAT_R16G16B16A16_SINT",
"DXGI_FORMAT_R32G32_TYPELESS",
"DXGI_FORMAT_R32G32_FLOAT",
"DXGI_FORMAT_R32G32_UINT",
"DXGI_FORMAT_R32G32_SINT",
"DXGI_FORMAT_R32G8X24_TYPELESS",
"DXGI_FORMAT_D32_FLOAT_S8X24_UINT",
"DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS",
"DXGI_FORMAT_X32_TYPELESS_G8X24_UINT",
"DXGI_FORMAT_R10G10B10A2_TYPELESS",
"DXGI_FORMAT_R10G10B10A2_UNORM",
"DXGI_FORMAT_R10G10B10A2_UINT",
"DXGI_FORMAT_R11G11B10_FLOAT",
"DXGI_FORMAT_R8G8B8A8_TYPELESS",
"DXGI_FORMAT_R8G8B8A8_UNORM",
"DXGI_FORMAT_R8G8B8A8_UNORM_SRGB",
"DXGI_FORMAT_R8G8B8A8_UINT",
"DXGI_FORMAT_R8G8B8A8_SNORM",
"DXGI_FORMAT_R8G8B8A8_SINT",
"DXGI_FORMAT_R16G16_TYPELESS",
"DXGI_FORMAT_R16G16_FLOAT",
"DXGI_FORMAT_R16G16_UNORM",
"DXGI_FORMAT_R16G16_UINT",
"DXGI_FORMAT_R16G16_SNORM",
"DXGI_FORMAT_R16G16_SINT",
"DXGI_FORMAT_R32_TYPELESS",
"DXGI_FORMAT_D32_FLOAT",
"DXGI_FORMAT_R32_FLOAT",
"DXGI_FORMAT_R32_UINT",
"DXGI_FORMAT_R32_SINT",
"DXGI_FORMAT_R24G8_TYPELESS",
"DXGI_FORMAT_D24_UNORM_S8_UINT",
"DXGI_FORMAT_R24_UNORM_X8_TYPELESS",
"DXGI_FORMAT_X24_TYPELESS_G8_UINT",
"DXGI_FORMAT_R8G8_TYPELESS",
"DXGI_FORMAT_R8G8_UNORM",
"DXGI_FORMAT_R8G8_UINT",
"DXGI_FORMAT_R8G8_SNORM",
"DXGI_FORMAT_R8G8_SINT",
"DXGI_FORMAT_R16_TYPELESS",
"DXGI_FORMAT_R16_FLOAT",
"DXGI_FORMAT_D16_UNORM",
"DXGI_FORMAT_R16_UNORM",
"DXGI_FORMAT_R16_UINT",
"DXGI_FORMAT_R16_SNORM",
"DXGI_FORMAT_R16_SINT",
"DXGI_FORMAT_R8_TYPELESS",
"DXGI_FORMAT_R8_UNORM",
"DXGI_FORMAT_R8_UINT",
"DXGI_FORMAT_R8_SNORM",
"DXGI_FORMAT_R8_SINT",
"DXGI_FORMAT_A8_UNORM",
"DXGI_FORMAT_R1_UNORM",
"DXGI_FORMAT_R9G9B9E5_SHAREDEXP",
"DXGI_FORMAT_R8G8_B8G8_UNORM",
"DXGI_FORMAT_G8R8_G8B8_UNORM",
"DXGI_FORMAT_BC1_TYPELESS",
"DXGI_FORMAT_BC1_UNORM",
"DXGI_FORMAT_BC1_UNORM_SRGB",
"DXGI_FORMAT_BC2_TYPELESS",
"DXGI_FORMAT_BC2_UNORM",
"DXGI_FORMAT_BC2_UNORM_SRGB",
"DXGI_FORMAT_BC3_TYPELESS",
"DXGI_FORMAT_BC3_UNORM",
"DXGI_FORMAT_BC3_UNORM_SRGB",
"DXGI_FORMAT_BC4_TYPELESS",
"DXGI_FORMAT_BC4_UNORM",
"DXGI_FORMAT_BC4_SNORM",
"DXGI_FORMAT_BC5_TYPELESS",
"DXGI_FORMAT_BC5_UNORM",
"DXGI_FORMAT_BC5_SNORM",
"DXGI_FORMAT_B5G6R5_UNORM",
"DXGI_FORMAT_B5G5R5A1_UNORM",
"DXGI_FORMAT_B8G8R8A8_UNORM",
"DXGI_FORMAT_B8G8R8X8_UNORM",
"DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM",
"DXGI_FORMAT_B8G8R8A8_TYPELESS",
"DXGI_FORMAT_B8G8R8A8_UNORM_SRGB",
"DXGI_FORMAT_B8G8R8X8_TYPELESS",
"DXGI_FORMAT_B8G8R8X8_UNORM_SRGB",
"DXGI_FORMAT_BC6H_TYPELESS",
"DXGI_FORMAT_BC6H_UF16",
"DXGI_FORMAT_BC6H_SF16",
"DXGI_FORMAT_BC7_TYPELESS",
"DXGI_FORMAT_BC7_UNORM",
"DXGI_FORMAT_BC7_UNORM_SRGB",
"DXGI_FORMAT_AYUV",
"DXGI_FORMAT_Y410",
"DXGI_FORMAT_Y416",
"DXGI_FORMAT_NV12",
"DXGI_FORMAT_P010",
"DXGI_FORMAT_P016",
"DXGI_FORMAT_420_OPAQUE",
"DXGI_FORMAT_YUY2",
"DXGI_FORMAT_Y210",
"DXGI_FORMAT_Y216",
"DXGI_FORMAT_NV11",
"DXGI_FORMAT_AI44",
"DXGI_FORMAT_IA44",
"DXGI_FORMAT_P8",
"DXGI_FORMAT_A8P8",
"DXGI_FORMAT_B4G4R4A4_UNORM"
};