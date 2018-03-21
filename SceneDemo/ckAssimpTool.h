#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
///����ļ���������assimp��������ݽ���ת�����Լ�����Ҫ�����ݽṹ

struct ck_aiVectorKey
{
	aiVector3D mValue;//��ǰ���Թؼ�֡��ֵ
	float mTime;//��ǰ�ؼ�֡ʱ���
};

struct ck_aiQuaternionKey//��Ԫ����ת��ʾ�Ĺؼ�ֵ֡
{
	aiQuaternion mValue;
	float mTime;//��ǰ�ؼ�֡ʱ���
};
struct ck_aiAnimNode
{
	std::string mNodeAnimName;//��ǰ�����ڵ������
	std::vector<ck_aiVectorKey> mScalingKeys;
	std::vector<ck_aiVectorKey> mPositionKeys;
	std::vector<ck_aiQuaternionKey> mRotationKeys;
};
struct ck_aiAnimation
{
	float mTicksPerSecond;//��ǰ����һ���ӵ��ڶ���ticks��
	float mDuration;//��ǰ����������ʱ��(��ticks������ʾ)
	std::vector<ck_aiAnimNode*> mChannelVec;//��ǰ�������ж��ٸ������˶�Ӱ���
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
