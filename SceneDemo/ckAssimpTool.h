#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
///这个文件用来负责将assimp中相关数据进行转换成自己所需要的数据结构

struct ck_aiVectorKey
{
	aiVector3D mValue;//当前属性关键帧的值
	float mTime;//当前关键帧时间点
};

struct ck_aiQuaternionKey//四元数旋转表示的关键帧值
{
	aiQuaternion mValue;
	float mTime;//当前关键帧时间点
};
struct ck_aiAnimNode
{
	std::string mNodeAnimName;//当前骨骼节点的名称
	std::vector<ck_aiVectorKey> mScalingKeys;
	std::vector<ck_aiVectorKey> mPositionKeys;
	std::vector<ck_aiQuaternionKey> mRotationKeys;
};
struct ck_aiAnimation
{
	float mTicksPerSecond;//当前动画一秒钟等于多少ticks数
	float mDuration;//当前动画持续的时间(以ticks数量表示)
	std::vector<ck_aiAnimNode*> mChannelVec;//当前动画是有多少个骨骼运动影响的
};

struct ck_aiNode
{
	std::vector<ck_aiNode*> mChildrenVec;
	std::string mNodeName;
	aiMatrix4x4 mTransform;
};

struct ck_aiScene
{
	std::vector<ck_aiAnimation*> mAnimationVec;
	ck_aiNode* mRootNode;
};
