//
// join_logo_scp �f�[�^�p��`
//
#pragma once

//---------------------------------------------------------------------
// �f�[�^�i�[�p
//---------------------------------------------------------------------
namespace jlsd
{
	using Msec = int;
	using Sec = int;
	using Nsc = int;
	using Nrf = int;
	using Nlg = int;

	//--- Auto�\����؂�(scp.chap) ---
	enum ScpChapType {
		SCP_CHAP_DUPE = -1,		// �d���ӏ�
		SCP_CHAP_NONE,			// �������
		SCP_CHAP_CPOSIT,		// 5�b�P�ʉ\���ێ�
		SCP_CHAP_CPOSQ,			// 15�b�P�ʉ\���ێ�
		SCP_CHAP_CDET,			// 15�b�P�ʕێ�
		SCP_CHAP_DINT,			// �\�����������b�P�ʋ�؂�
		SCP_CHAP_DBORDER,		// �����V�[���`�F���W�Ȃ���؂�
		SCP_CHAP_DFIX,			// �\����؂�
		SCP_CHAP_DFORCE,		// ������؂�ݒ�ӏ�
		SCP_CHAP_DUNIT			// Trim��������؂�
	};
	static const ScpChapType SCP_CHAP_DECIDE = SCP_CHAP_DINT;		// �m��ӏ�臒l
	inline bool isScpChapTypeDecide(ScpChapType type){
		return (type >= SCP_CHAP_DECIDE)? true : false;
	}

	//--- Auto�\��������{���e(scp.arstat) ---
	enum ScpArType {
		SCP_AR_UNKNOWN,			// �s��
		SCP_AR_L_UNIT,			// ���S�L �P�T�b�P��
		SCP_AR_L_OTHER,			// ���S�L ���̑�
		SCP_AR_L_MIXED,			// ���S�L ���S��������
		SCP_AR_N_UNIT,			// ���S�� �P�T�b�P��
		SCP_AR_N_OTHER,			// ���S�� ���̑�
		SCP_AR_N_AUNIT,			// ���S�� �����łP�T�b�̒��Ԓn�_
		SCP_AR_N_BUNIT,			// ���S�� �����łP�T�b�̒[
		SCP_AR_B_UNIT,			// ���S���E �P�T�b�P��
		SCP_AR_B_OTHER			// ���S���E ���̑�
	};
	static const int SCP_ARR_L_LOW  = SCP_AR_L_UNIT;		// ���S�L�̉����l
	static const int SCP_ARR_L_HIGH = SCP_AR_L_MIXED;		// ���S�L�̏���l
	inline bool isScpArTypeLogo(ScpArType type){
		return (type >= SCP_ARR_L_LOW && type <= SCP_ARR_L_HIGH)? true : false;
	}
	inline bool isScpArTypeBorder(ScpArType type){
		return (type >= SCP_AR_B_UNIT && type <= SCP_AR_B_OTHER)? true : false;
	}
	inline bool isScpArTypeLogoBorder(ScpArType type){
		return ((type >= SCP_ARR_L_LOW && type <= SCP_ARR_L_HIGH) ||
				(type >= SCP_AR_B_UNIT && type <= SCP_AR_B_OTHER))? true : false;
	}
	//--- Auto�\�������g�����e(scp.arext) ---
	enum ScpArExtType {
		SCP_AREXT_NONE,			// �ǉ��\���Ȃ�
		SCP_AREXT_L_TRKEEP,		// ���S�L �c���\��
		SCP_AREXT_L_TRCUT,		// ���S�L �J�b�g�Ԑ�
		SCP_AREXT_L_TRRAW,		// ���S�L �G���h�J�[�h���f�O
		SCP_AREXT_L_ECCUT,		// ���S�L �J�b�g�Ԑ�
		SCP_AREXT_L_EC,			// ���S�L �G���h�J�[�h
		SCP_AREXT_L_SP,			// ���S�L �ԑg��
		SCP_AREXT_L_LGCUT,		// ���S�L ���S�[�����J�b�g
		SCP_AREXT_L_LGADD,		// ���S�L ���S�[�����c��
		SCP_AREXT_N_TRCUT,		// ���S�� �J�b�g�Ԑ�
		SCP_AREXT_N_LGCUT,		// ���S�� ���S�[�����J�b�g
		SCP_AREXT_N_LGADD		// ���S�� ���S�[�����c��
	};
	

	//--- �ݒ�l�ێ� ---
	enum ConfigVarType {
		CONFIG_VAR_msecWLogoTRMax,			// AutoCut�R�}���h�ŃJ�b�g�ΏۂƂ��郍�S���ԍő�t���[������
		CONFIG_VAR_msecWCompTRMax,			// AutoCut�R�}���hTR�ŗ\���ƔF������\���ő�t���[������
		CONFIG_VAR_msecWLogoSftMrg,			// Auto�R�}���h�O�����Ń��S�؂�ւ��̂���������t���[������
		CONFIG_VAR_msecWCompFirst,			// �擪�\���J�b�g�����ɂ���\���ő�t���[������
		CONFIG_VAR_msecWCompLast,			// �Ō�\���J�b�g�����ɂ���\���ő�t���[������
		CONFIG_VAR_msecWLogoSumMin,			// ���S���v���Ԃ��w��t���[�������̎��̓��S�Ȃ��Ƃ��Ĉ���
		CONFIG_VAR_msecWLogoLgMin,			// CM�������Ƀ��S�L���m��ƔF������ŏ����S����
		CONFIG_VAR_msecWLogoCmMin,			// CM�������Ƀ��S�����m��ƔF������ŏ����S����
		CONFIG_VAR_msecWLogoRevMin,			// ���S���␳���鎞�ɖ{�҂ƔF������ŏ�����
		CONFIG_VAR_msecMgnCmDetect,			// CM�\����15�b�P�ʂł͂Ȃ��\���ƔF������덷�t���[������
		CONFIG_VAR_msecMgnCmDivide,			// CM�\���������������P�b�P�ʂ���̌덷�t���[������
		CONFIG_VAR_secWCompSPMin,			// Auto�R�}���h�ԑg�񋟂ŕW���ŏ��b��
		CONFIG_VAR_secWCompSPMax,			// Auto�R�}���h�ԑg�񋟂ŕW���ő�b��
		CONFIG_VAR_flagCutTR,				// 15�b�ȏ�Ԑ���J�b�g����ꍇ��1���Z�b�g
		CONFIG_VAR_flagCutSP,				// �ԑg�񋟂��J�b�g����ꍇ��1���Z�b�g
		CONFIG_VAR_flagAddLogo,				// ���S����ʏ�\�����c���ꍇ��1���Z�b�g�i���݂͖��g�p�j
		CONFIG_VAR_flagAddUC,				// ���S�Ȃ��s���\�����c���ꍇ��1���Z�b�g
		CONFIG_VAR_typeNoSc,				// �V�[���`�F���W�Ȃ������ʒu��CM���f�i0:���� 1:�Ȃ� 2:����j
		CONFIG_VAR_cancelCntSc,				// �����������\���𕪗����Ȃ�������1�̎��͎g�p���Ȃ�
		CONFIG_VAR_LogoLevel,				// ���S�g�p���x��
		CONFIG_VAR_LogoRevise,				// ���S����̕␳
		CONFIG_VAR_AutoCmSub,				// ���S�Ȃ����̕⏕�ݒ�
		CONFIG_VAR_msecPosFirst,			// ���S�J�n�ʒu���o�ݒ����
		CONFIG_VAR_msecLgCutFirst,			// ���S���ŏ����炠�鎞�ɃJ�b�g�����ɂ���\���ő�t���[������
		CONFIG_VAR_msecZoneFirst,			// ���S�����Ƃ���J�n�ʒu���o�ݒ����
		CONFIG_VAR_msecZoneLast,			// ���S�����Ƃ���I���ʒu���o�ݒ����
		CONFIG_VAR_priorityPosFirst,		// �ŏ��̈ʒu�ݒ�D��x�i0:����Ȃ� 1:���S���� 2:�ʒu�D�� 3:Select�D��j
		SIZE_CONFIG_VAR
	};
	enum ConfigActType {					// �ݒ�l�𓮍�ʂɎ擾�p
		CONFIG_ACT_LogoDelEdge,				// ���S�[��CM���f
		CONFIG_ACT_LogoDelMid,				// ���S����15�b�P��CM��
		CONFIG_ACT_LogoDelWide,				// �L�惍�S�Ȃ��폜
		CONFIG_ACT_LogoUCRemain,			// ���S�Ȃ��s���������c��
		CONFIG_ACT_LogoUCGapCm,				// CM�P�ʂ���덷���傫���\�����c��
		CONFIG_ACT_MuteNoSc					// �V�[���`�F���W�Ȃ������ʒu��CM���f�i1:�g�p���Ȃ� 2:�g�p����j
	};

	//--- �\�����D�揇��(scp.stat) ---
	enum ScpPriorType {
		SCP_PRIOR_DUPE = -1,	// �Ԉ���
		SCP_PRIOR_NONE,			// �������
		SCP_PRIOR_LV1,			// ���
		SCP_PRIOR_DECIDE		// ����
	};

	//--- �\�����D�揇��(logo.stat_*) ---
	enum LogoPriorType {
		LOGO_PRIOR_DUPE = -1,	// �Ԉ���
		LOGO_PRIOR_NONE,		// �������
		LOGO_PRIOR_LV1,			// ���
		LOGO_PRIOR_DECIDE		// ����
	};
	//--- ���S�����\�����(logo.unit_*) ---
	enum LogoUnitType {
		LOGO_UNIT_NORMAL,		// �ʏ�
		LOGO_UNIT_DIVIDE		// ���S����
	};
	//--- ���S���ʊm����(logo.flag_*) ---
	enum LogoResultType {
		LOGO_RESULT_NONE,		// �������
		LOGO_RESULT_DECIDE,		// �m��
		LOGO_RESULT_ABORT		// abort�j���m��
	};

	//--- �I����� ---
	enum SearchDirType {
		SEARCH_DIR_PREV,
		SEARCH_DIR_NEXT
	};

	//--- ���S�̑I���G�b�W ---
	enum LogoEdgeType {
		LOGO_EDGE_RISE,			// ���S�̗����オ��G�b�W
		LOGO_EDGE_FALL,			// ���S�̗���������G�b�W
		LOGO_EDGE_BOTH			// ���S�̗��G�b�W
	};
	//--- ���S�̑I�� ---
	enum LogoSelectType {
		LOGO_SELECT_ALL,		// �S�I��
		LOGO_SELECT_VALID		// �L���̂ݑI��
	};
	//--- �V�[���`�F���W�őS�̂̐擪�Ō�������I�� ---
	enum ScpEndType {
		SCP_END_EDGEIN,			// �V�[���`�F���W�ԍ��̐擪�Ō�܂�
		SCP_END_NOEDGE			// �V�[���`�F���W�ԍ��̐擪�Ō㏜��
	};

	//--- ���S�̃G�b�W�����F�� ---
	inline bool isLogoEdgeRise(LogoEdgeType type){
		return (type == LOGO_EDGE_RISE || type == LOGO_EDGE_BOTH)? true : false;
	}
	inline bool isLogoEdgeFall(LogoEdgeType type){
		return (type == LOGO_EDGE_FALL || type == LOGO_EDGE_BOTH)? true : false;
	}
	inline LogoEdgeType edgeInvert(LogoEdgeType type){
		return (type == LOGO_EDGE_FALL)? LOGO_EDGE_RISE : LOGO_EDGE_FALL;
	}
	inline bool isLogoEdgeRiseFromNrf(int nrf){
		return (nrf % 2 == 0)? true : false;
	}
	//--- ���S�ԍ��ϊ��inrf - nlg�j ---
	inline LogoEdgeType edgeFromNrf(int nrf){
		return (nrf % 2 == 0)? LOGO_EDGE_RISE : LOGO_EDGE_FALL;
	}
	inline LogoEdgeType edgeInvertFromNrf(int nrf){
		return (nrf % 2 == 0)? LOGO_EDGE_FALL : LOGO_EDGE_RISE;
	}
	inline int nlgFromNrf(int nrf){
		return nrf / 2;
	}
	inline int nrfFromNlg(int nlg, LogoEdgeType edge){
		return (edge == LOGO_EDGE_FALL)? nlg*2+1 : nlg*2;
	}
	inline int nrfFromNlgRise(int nlg){
		return nlg*2;
	}
	inline int nrfFromNlgFall(int nlg){
		return nlg*2+1;
	}
	//--- �D�揇�ʂ̕ϊ� ---
	inline LogoPriorType priorLogoFromScp(ScpPriorType n){
		return (LogoPriorType) n;
	}

	//--- �ێ��ݒ�l�̃t���O�l��` ---
	enum ConfigBitType {
		CONFIG_LOGO_LEVEL_DEFAULT   = 0,
		CONFIG_LOGO_LEVEL_UNUSE_ALL = 1,
		CONFIG_LOGO_LEVEL_UNUSE_EX1 = 2,
		CONFIG_LOGO_LEVEL_UNUSE_EX2 = 3,
		CONFIG_LOGO_LEVEL_USE_LOW   = 4,
		CONFIG_LOGO_LEVEL_USE_MIDL  = 5,
		CONFIG_LOGO_LEVEL_USE_MIDH  = 6,
		CONFIG_LOGO_LEVEL_USE_HIGH  = 7,
		CONFIG_LOGO_LEVEL_USE_MAX   = 8
	};

	//--- �\���� ---
	struct RangeNsc {
		Nsc st;
		Nsc ed;
	};
	struct RangeNrf {
		Nrf st;
		Nrf ed;
	};
	struct RangeMsec {
		Msec st;
		Msec ed;
	};
	struct WideMsec {
		Msec just;
		Msec early;
		Msec late;
	};
	struct RangeFixMsec {
		Msec st;
		Msec ed;
		bool fixSt;				// true=�m��J�n�n�_
		bool fixEd;				// true=�m��I���n�_
	};
	struct RangeWideMsec {
		WideMsec st;
		WideMsec ed;
		bool fixSt;				// true=�m��J�n�n�_
		bool fixEd;				// true=�m��I���n�_
		bool logomode;			// false=CM����  true=���S����
	};
	struct RangeNscMsec {
		RangeNsc  nsc;
		RangeMsec msec;
	};
	struct Term {
		bool valid;
		bool endfix;
		ScpEndType endtype;
		Nsc ini;
		RangeNsc nsc;
		RangeMsec msec;
	};
	struct NrfCurrent {
		bool valid;				// 0=�f�[�^�i�[�Ȃ�  1=�f�[�^�i�[����
		Nrf nrfRise;
		Nrf nrfFall;
		Nrf nrfLastRise;
		Nrf nrfLastFall;
		Msec msecRise;
		Msec msecFall;
		Msec msecLastRise;
		Msec msecLastFall;
	};
	struct ElgCurrent {
		bool valid;				// 0=�f�[�^�i�[�Ȃ�  1=�f�[�^�i�[����
		bool border;			// 0=border�܂߂Ȃ�  1=border�܂�
		bool outflag;			// 0=�������� 1=�ŏI�o�͓���
		Nsc nscRise;
		Nsc nscFall;
		Nsc nscLastRise;
		Nsc nscLastFall;
		Msec msecRise;
		Msec msecFall;
		Msec msecLastRise;
		Msec msecLastFall;
	};
	struct CalcDifInfo {
		int  sgn;
		Sec  sec;
		Msec gap;
	};
	struct CalcModInfo {
		Msec mod15;
		Msec mod05;
	};

}

//---------------------------------------------------------------------
// JL�X�N���v�g�̈����֘A�ێ�
//---------------------------------------------------------------------
namespace jlscmd
{
	//--- JL�X�N���v�g���� ---
	enum JlcmdSelType {
		JLCMD_SEL_Nop,
		JLCMD_SEL_If,
		JLCMD_SEL_EndIf,
		JLCMD_SEL_Else,
		JLCMD_SEL_ElsIf,
		JLCMD_SEL_Call,
		JLCMD_SEL_Repeat,
		JLCMD_SEL_EndRepeat,
		JLCMD_SEL_Echo,
		JLCMD_SEL_LogoOff,
		JLCMD_SEL_OldAdjust,
		JLCMD_SEL_Set,
		JLCMD_SEL_Default,
		JLCMD_SEL_EvalFrame,
		JLCMD_SEL_EvalTime,
		JLCMD_SEL_EvalNum,
		JLCMD_SEL_SetParam,
		JLCMD_SEL_OptSet,
		JLCMD_SEL_OptDefault,
		JLCMD_SEL_UnitSec,
		JLCMD_SEL_AutoCut,
		JLCMD_SEL_AutoAdd,
		JLCMD_SEL_AutoEdge,
		JLCMD_SEL_AutoCM,
		JLCMD_SEL_AutoUp,
		JLCMD_SEL_AutoBorder,
		JLCMD_SEL_AutoIns,
		JLCMD_SEL_AutoDel,
		JLCMD_SEL_Find,
		JLCMD_SEL_MkLogo,
		JLCMD_SEL_DivLogo,
		JLCMD_SEL_Select,
		JLCMD_SEL_Force,
		JLCMD_SEL_Abort,
		JLCMD_SEL_GetPos,
		JLCMD_SEL_GetList,
		JLCMD_SEL_NextTail,
		SIZE_JLCMD_SEL
	};
	//--- JL�X�N���v�g���ߎ�� ---
	enum JlcmdCategoryType {
		JLCMD_CAT_NONE,
		JLCMD_CAT_COND,
		JLCMD_CAT_CALL,
		JLCMD_CAT_REP,
		JLCMD_CAT_SYS,
		JLCMD_CAT_REG,
		JLCMD_CAT_NEXT,
		JLCMD_CAT_LOGO,
		JLCMD_CAT_AUTOLOGO,
		JLCMD_CAT_AUTOEACH,
		JLCMD_CAT_AUTO
	};
	//--- JL�X�N���v�g�I�v�V������� ---
	enum JlOptionCategoryType{
		JLOPT_CAT_NONE,
		JLOPT_CAT_NUMLOGO,
		JLOPT_CAT_FRAME,
		JLOPT_CAT_SC,
		JLOPT_CAT_MSECM1,
		JLOPT_CAT_SEC,
		JLOPT_CAT_NUM,
		JLOPT_CAT_FLAG
	};
	//--- JL�X�N���v�g�I�v�V�����i�[�ϐ� ---
	enum JlOptionDataType {
		JLOPT_DATA_DUMMY,
		JLOPT_DATA_TypeNLogo,
		JLOPT_DATA_TypeFrame,
		JLOPT_DATA_MsecFrameLeft,
		JLOPT_DATA_MsecFrameRight,
		JLOPT_DATA_MsecEndlenC,
		JLOPT_DATA_MsecEndlenL,
		JLOPT_DATA_MsecEndlenR,
		JLOPT_DATA_MsecSftC,
		JLOPT_DATA_MsecSftL,
		JLOPT_DATA_MsecSftR,
		JLOPT_DATA_MsecTLHoldL,
		JLOPT_DATA_MsecTLHoldR,
		JLOPT_DATA_MsecLenPMin,
		JLOPT_DATA_MsecLenPMax,
		JLOPT_DATA_MsecLenNMin,
		JLOPT_DATA_MsecLenNMax,
		JLOPT_DATA_MsecFromAbs,
		JLOPT_DATA_MsecFromHead,
		JLOPT_DATA_MsecFromTail,
		JLOPT_DATA_MsecLogoExtL,
		JLOPT_DATA_MsecLogoExtR,
		JLOPT_DATA_MsecEndAbs,
		JLOPT_DATA_AutopCode,
		JLOPT_DATA_AutopLimit,
		JLOPT_DATA_AutopScope,
		JLOPT_DATA_AutopScopeN,
		JLOPT_DATA_AutopScopeX,
		JLOPT_DATA_AutopPeriod,
		JLOPT_DATA_AutopMaxPrd,
		JLOPT_DATA_AutopSecNext,
		JLOPT_DATA_AutopSecPrev,
		JLOPT_DATA_AutopTrScope,
		JLOPT_DATA_AutopTrSumPrd,
		JLOPT_DATA_AutopTr1stPrd,
		JLOPT_DATA_FlagWide,
		JLOPT_DATA_FlagFromLast,
		JLOPT_DATA_FlagWithP,
		JLOPT_DATA_FlagWithN,
		JLOPT_DATA_FlagNoEdge,
		JLOPT_DATA_FlagOverlap,
		JLOPT_DATA_FlagConfirm,
		JLOPT_DATA_FlagUnit,
		JLOPT_DATA_FlagElse,
		JLOPT_DATA_FlagCont,
		JLOPT_DATA_FlagReset,
		JLOPT_DATA_FlagFlat,
		JLOPT_DATA_FlagForce,
		JLOPT_DATA_FlagNoForce,
		JLOPT_DATA_FlagAutoChg,
		JLOPT_DATA_FlagAutoEach,
		JLOPT_DATA_FlagEndHead,
		JLOPT_DATA_FlagEndTail,
		JLOPT_DATA_FlagEndHold,
		JLOPT_DATA_FlagRelative,
		SIZE_JLOPT_DATA
	};
	enum JlOptionArgScType {
		CMDARG_SC_NONE,
		CMDARG_SC_SC,
		CMDARG_SC_NOSC,
		CMDARG_SC_SM,
		CMDARG_SC_NOSM,
		CMDARG_SC_SMA,
		CMDARG_SC_NOSMA,
		CMDARG_SC_AC,
		CMDARG_SC_NOAC
	};
	enum JlOptionArgLgType {
		CMDARG_LG_NONE,
		CMDARG_LG_N,
		CMDARG_LG_NR,
		CMDARG_LG_NLOGO,
		CMDARG_LG_NAUTO
	};
	enum JlOptionArgFrType {
		CMDARG_FR_F,
		CMDARG_FR_FR,
		CMDARG_FR_FHEAD,
		CMDARG_FR_FTAIL,
		CMDARG_FR_FMID,
		CMDARG_FR_FHEADX,
		CMDARG_FR_FTAILX,
		CMDARG_FR_FMIDX
	};

	//--- JL�X�N���v�g���߃T�u�I�� ---
	enum JlcmdSubType {
		JLCMD_SUB_TR,
		JLCMD_SUB_SP,
		JLCMD_SUB_EC
	};
	//--- JL�X�N���v�g�f�R�[�h���ʃG���[ ---
	enum JlcmdErrType {
		JLCMD_ERR_None,
		JLCMD_ERR_ErrOpt,				// �R�}���h�ُ�i�I�v�V�����j
		JLCMD_ERR_ErrRange,				// �R�}���h�ُ�i�͈́j
		JLCMD_ERR_ErrSEB,				// �R�}���h�ُ�iS/E/B�I���j
		JLCMD_ERR_ErrVar,				// �R�}���h�ُ�i�ϐ��֘A�j
		JLCMD_ERR_ErrTR,				// �R�}���h�ُ�iTR/SP/ED�I���j
		JLCMD_ERR_ErrCmd				// �R�}���h�ُ�i�R�}���h�j
	};
	//--- JL�X�N���v�gAuto�n�R�}���h ---
	enum JlcmdAutoType {
		JLCMD_AUTO_None,
		JLCMD_AUTO_CUTTR,
		JLCMD_AUTO_CUTEC,
		JLCMD_AUTO_ADDTR,
		JLCMD_AUTO_ADDSP,
		JLCMD_AUTO_ADDEC,
		JLCMD_AUTO_EDGE,
		JLCMD_AUTO_ATCM,
		JLCMD_AUTO_ATUP,
		JLCMD_AUTO_ATBORDER,
		JLCMD_AUTO_ATCHG,
		JLCMD_AUTO_INS,
		JLCMD_AUTO_DEL
	};
	//--- Auto�R�}���h�p�����[�^ ---
	enum JlParamAuto {
		// code�p�����[�^
		PARAM_AUTO_c_exe,			// 0:�R�}���h���s�Ȃ� 1:�R�}���h���s
		PARAM_AUTO_c_search,		// ��������͈͂�I��
		PARAM_AUTO_c_wmin,			// �\�����Ԃ̍ŏ��l�b��
		PARAM_AUTO_c_wmax,			// �\�����Ԃ̍ő�l�b��
		PARAM_AUTO_c_w15,			// 1:�ԑg�\����15�b������
		PARAM_AUTO_c_lgprev,		// 0:���S�E�\���̑O����ΏۊO
		PARAM_AUTO_c_lgpost,		// 0:���S�E�\���̌㑤��ΏۊO
		PARAM_AUTO_c_lgintr,		// 1:�\���Ɣԑg�񋟂̊Ԃ̂ݑΏۂƂ���
		PARAM_AUTO_c_lgsp,			// 1:�ԑg�񋟂�����ɂ���ꍇ�̂ݑΏ�
		PARAM_AUTO_c_cutskip,		// 1:�\���J�b�g�ȍ~���ΏۂƂ���
		PARAM_AUTO_c_in1,			// 1:�\���ʒu�ɔԑg�񋟂�����
		PARAM_AUTO_c_chklast,		// 1:�{�̍\������ɂ���ΑΏۊO�Ƃ���
		PARAM_AUTO_c_lgy,			// 1:���S����ΏۂƂ���
		PARAM_AUTO_c_lgn,			// 1:���S�O��ΏۂƂ���
		PARAM_AUTO_c_lgbn,			// 1:���ׂ��܂߃��S�O�̏ꍇ��ΏۂƂ���
		PARAM_AUTO_c_limloc,		// 1:�W�����Ԃ̌��ʒu�݂̂Ɍ���
		PARAM_AUTO_c_limtrsum,		// 1:�\�����Ԃɂ�薳��������
		PARAM_AUTO_c_unitcmoff,		// 1:CM���������\���̌��o����������
		PARAM_AUTO_c_unitcmon,		// 1:CM���������\���̌��o�������ݒ�
		PARAM_AUTO_c_wdefmin,		// �W���̍\�����Ԃ̍ŏ��l�b��
		PARAM_AUTO_c_wdefmax,		// �W���̍\�����Ԃ̍ő�l�b��
		// autocut�p
		PARAM_AUTO_c_from,			// cuttr
		PARAM_AUTO_c_cutst,			// cuttr
		PARAM_AUTO_c_lgpre,			// cuttr
		PARAM_AUTO_c_sel,			// cutec
		PARAM_AUTO_c_cutla,			// cutec
		PARAM_AUTO_c_cutlp,			// cutec
		PARAM_AUTO_c_cut30,			// cutec
		PARAM_AUTO_c_cutsp,			// cutec
		// edge�p
		PARAM_AUTO_c_cmpart,
		PARAM_AUTO_c_add,
		PARAM_AUTO_c_allcom,
		PARAM_AUTO_c_noedge,
		// autoins,autodel�p
		PARAM_AUTO_c_restruct,
		// ���l�p�����[�^
		PARAM_AUTO_v_limit,
		PARAM_AUTO_v_scope,
		PARAM_AUTO_v_scopen,
		PARAM_AUTO_v_period,
		PARAM_AUTO_v_maxprd,
		PARAM_AUTO_v_trsumprd,
		PARAM_AUTO_v_secprev,
		PARAM_AUTO_v_secnext,
		// autocut�p
		PARAM_AUTO_v_trscope,
		PARAM_AUTO_v_tr1stprd,
		// ���v��
		SIZE_PARAM_AUTO
	};
}


