#pragma once


//------------------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//

class MkHashStr;
class MkSceneNode;

class MkSceneNodeFamilyDefinition
{
public:

	enum eWindowEvent
	{
		// MkBaseWindowNode
		eEnable = 0,
		eDisable,
		eCursorLeftPress,
		eCursorMiddlePress,
		eCursorRightPress,
		eCursorLeftRelease,
		eCursorMiddleRelease,
		eCursorRightRelease,
		eCursorLeftClick,
		eCursorLeftDoubleClick,
		eCursorMiddleDoubleClick,
		eCursorRightDoubleClick,
		eCursorWheelDecrease,
		eCursorWheelIncrease,

		// MkSpreadButtonNode
		eOpenList,
		eSetTargetItem,

		// MkCheckButtonNode
		eCheckOn,
		eCheckOff
	};

	// MkSceneNode
	class Scene
	{
	public:
		static const MkHashStr TemplateName;
		static const MkHashStr PositionKey; // MkVec3
		static const MkHashStr RotationKey; // float
		static const MkHashStr ScaleKey; // float
		static const MkHashStr AlphaKey; // float
		static const MkHashStr VisibleKey; // bool
	};

	// MkBaseWindowNode
	class BaseWindow
	{
	public:
		static const MkHashStr TemplateName;
		static const MkHashStr EnableKey; // bool
		static const MkHashStr PresetThemeNameKey; // MkStr
		static const MkHashStr PresetComponentKey; // MkStr
		static const MkHashStr PresetBodySizeKey; // MkInt2
		static const MkHashStr PresetFullSizeKey; // MkInt2
		static const MkHashStr AttributeKey; // unsigned int
	};

	// MkSpreadButtonNode
	class SpreadButton
	{
	public:
		static const MkHashStr TemplateName;
		static const MkHashStr ButtonTypeKey; // unsigned int
		static const MkHashStr OpeningDirKey; // unsigned int
		static const MkHashStr IconPathKey; // MkStr
		static const MkHashStr IconSubsetKey; // MkStr
		static const MkHashStr CaptionKey; // MkArray<MkStr>
		static const MkHashStr ItemSeqKey; // MkArray<MkStr>
		static const MkHashStr TargetButtonKey; // MkStr
	};


	// MkCheckButtonNode
	class CheckButton
	{
	public:
		static const MkHashStr TemplateName;
		static const MkHashStr CaptionKey; // MkArray<MkStr>
		static const MkHashStr OnCheckKey; // bool
	};

	// alloc
	static MkSceneNode* Alloc(const MkHashStr& templateName, const MkHashStr& nodeName);
};
