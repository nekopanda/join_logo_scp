//
// JL�R�}���h���s�p
//
///////////////////////////////////////////////////////////////////////
#pragma once

class JlsReformData;
class JlsAutoScript;
class JlsCmdArg;
class JlsCmdLimit;
class JlsCmdSet;
class JlsDataset;


///////////////////////////////////////////////////////////////////////
//
// �ϐ��i�[�N���X
//
///////////////////////////////////////////////////////////////////////
class JlsRegFile
{
public:
	bool setRegVar(const string &strName, const string &strVal, bool overwrite);
	int  getRegVar(string &strVal, const string &strCandName, bool exact);

private:
	int  getRegNameVal(string &strName, string &strVal, const string &strPair);

private:
	static const int SIZE_VARNUM_MAX = 2048;		// �ϐ��̍ő吔��O�̂��ߐݒ�
	vector<string> m_strListVar;

};


///////////////////////////////////////////////////////////////////////
//
// �X�N���v�g����
//
///////////////////////////////////////////////////////////////////////
class JlsScriptState
{
private:
	static const int SIZE_REPLINE  = 4096;		// �L���b�V���ێ��ő�s��

	enum CondIfState {			// If���
		COND_IF_FINISHED,		// ���s�ς�
		COND_IF_PREPARE,		// �����s
		COND_IF_RUNNING			// ���s��
	};
	struct RepDepthHold {		// Repeat�e�l�X�g�̏��
		int  lineStart;			// �J�n�s
		int  countLoop;			// �J��Ԃ��c���
	};

public:
	JlsScriptState();
	void clear();
	int  ifBegin(bool flag_cond);
	int  ifEnd();
	int  ifElse(bool flag_cond);
	int  repeatBegin(int num);
	int  repeatEnd();
	int  isRemainNest();
	bool readCmdCache(string &strBufOrg);
	bool addCmdCache(string &strBufOrg);
	bool isValidCmdLine(bool category_if, bool category_repeat);
	bool isSkipCmd();

private:
	//--- IF������ ---
	bool					m_ifSkip;			// IF�����O�i0=�ʏ�  1=�����O�Ŏ��s���Ȃ��j
	vector <CondIfState>	m_listIfState;		// �eIF�l�X�g�̏�ԁi���s�ς� �����s ���s���j
	//--- Repeat������ ---
	bool					m_repSkip;			// Repeat���s�i0=�ʏ�  1=�J��Ԃ��O��Ŏ��s�Ȃ��j
	int						m_repLineReadCache;	// �ǂݏo���L���b�V���s
	vector <string>			m_listRepCmdCache;	// repeat���̃R�}���h������L���b�V��
	vector <RepDepthHold>	m_listRepDepth;		// �J��Ԃ���ԕێ�
};


///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g���s�N���X
//
///////////////////////////////////////////////////////////////////////
class JlsScript
{
private:
	static const int SIZE_CALL_LOOP = 10;		// Call�R�}���h�̍ċA�ő��

	// ���߃Z�b�g�\��
	enum ConfigStrValType {
		CONFIG_STR_MSEC,			// �~���b�擾
		CONFIG_STR_MSECM1,			// �~���b�擾�i�}�C�i�X�P�͂��̂܂܎c���j
		CONFIG_STR_SEC,				// �b�擾�i�������͕͂b�Ƃ��Ĉ����j
		CONFIG_STR_NUM				// ���l�擾
	};
	struct LogoMarkCount {			// ���S�ԍ����J�E���g
		int abs;					// -N
		int rel;					// -NR
		int lg;						// -LG
	};
	struct LogoMarkExist {			// ���S�ԍ��J�E���g�p
		bool abs;
		bool rel;
		bool lg;
	};
	struct JlscrCmdRecord {
		JlcmdSelType		cmdsel;		// �I���R�}���h
		JlcmdCategoryType	category;	// �R�}���h���
		int		mustchar;			// �K�{�I�v�V�����P�i0=�Ȃ�  1=S/E/B  2=TR/SP/EC�j
		int		mustrange;			// �K�{�I�v�V�����Q�i0=�Ȃ�  1=center  3=center+left+right�j
		int		needopt;			// �ǉ��I�v�V�����i0=�Ȃ�  1=�ǂݍ��݁j
		int		floatbase;			// ��ʒu�̓^�[�Q�b�g��
		char	cmdname[12];		// �R�}���h������
	};
	struct JlscrCmdAlias {
		JlcmdSelType		cmdsel;		// �I���R�}���h
		char	cmdname[12];			// �R�}���h�ʖ�������
	};
	struct JlOptionRecord {
		JlOptionCategoryType	category;	// �I�v�V�����̎��
		int						num;		// �ݒ萔�܂��͐ݒ萔�l
		int						sort;		// �������ёւ�(12=1��2, 23=2��3)
		JlOptionDataType		data1;		// ���̓f�[�^�̎��
		char	optname[12];				// �R�}���h������
	};
	struct ConfigDataRecord {
		ConfigVarType		prmsel;
		ConfigStrValType	valsel;
		char	namestr[18];
	};

//--- �R�}���h���X�g ---
const JlscrCmdRecord  CmdDefine[SIZE_JLCMD_SEL] = {
	{ JLCMD_SEL_Nop,        JLCMD_CAT_NONE,     0,0,0,0, "Nop"        },
	{ JLCMD_SEL_If,         JLCMD_CAT_COND,     0,0,0,0, "If"         },
	{ JLCMD_SEL_EndIf,      JLCMD_CAT_COND,     0,0,0,0, "EndIf"      },
	{ JLCMD_SEL_Else,       JLCMD_CAT_COND,     0,0,0,0, "Else"       },
	{ JLCMD_SEL_ElsIf,      JLCMD_CAT_COND,     0,0,0,0, "ElsIf"      },
	{ JLCMD_SEL_Call,       JLCMD_CAT_CALL,     0,0,0,0, "Call"       },
	{ JLCMD_SEL_Repeat,     JLCMD_CAT_REP,      0,0,0,0, "Repeat"     },
	{ JLCMD_SEL_EndRepeat,  JLCMD_CAT_REP,      0,0,0,0, "EndRepeat"  },
	{ JLCMD_SEL_Echo,       JLCMD_CAT_SYS,      0,0,0,0, "Echo"       },
	{ JLCMD_SEL_LogoOff,    JLCMD_CAT_SYS,      0,0,0,0, "LogoOff"    },
	{ JLCMD_SEL_OldAdjust,  JLCMD_CAT_SYS,      0,0,0,0, "OldAdjust"  },
	{ JLCMD_SEL_Set,        JLCMD_CAT_REG,      0,0,0,0, "Set"        },
	{ JLCMD_SEL_Default,    JLCMD_CAT_REG,      0,0,0,0, "Default"    },
	{ JLCMD_SEL_EvalFrame,  JLCMD_CAT_REG,      0,0,0,0, "EvalFrame"  },
	{ JLCMD_SEL_EvalTime,   JLCMD_CAT_REG,      0,0,0,0, "EvalTime"   },
	{ JLCMD_SEL_EvalNum,    JLCMD_CAT_REG,      0,0,0,0, "EvalNum"    },
	{ JLCMD_SEL_SetParam,   JLCMD_CAT_REG,      0,0,0,0, "SetParam"   },
	{ JLCMD_SEL_OptSet,     JLCMD_CAT_REG,      0,0,0,0, "OptSet"     },
	{ JLCMD_SEL_OptDefault, JLCMD_CAT_REG,      0,0,0,0, "OptDefault" },
	{ JLCMD_SEL_UnitSec,    JLCMD_CAT_REG,      0,0,0,0, "UnitSec"    },
	{ JLCMD_SEL_AutoCut,    JLCMD_CAT_AUTO    , 2,0,1,0, "AutoCut"    },
	{ JLCMD_SEL_AutoAdd,    JLCMD_CAT_AUTO    , 2,0,1,0, "AutoAdd"    },
	{ JLCMD_SEL_AutoEdge,   JLCMD_CAT_AUTOLOGO, 1,0,1,0, "AutoEdge"   },
	{ JLCMD_SEL_AutoCM,     JLCMD_CAT_AUTO,     3,0,1,0, "AutoCM"     },
	{ JLCMD_SEL_AutoUp,     JLCMD_CAT_AUTO,     3,0,1,0, "AutoUp"     },
	{ JLCMD_SEL_AutoBorder, JLCMD_CAT_AUTO,     3,0,1,0, "AutoBorder" },
	{ JLCMD_SEL_AutoIns,    JLCMD_CAT_AUTOLOGO, 1,3,1,0, "AutoIns"    },
	{ JLCMD_SEL_AutoDel,    JLCMD_CAT_AUTOLOGO, 1,3,1,0, "AutoDel"    },
	{ JLCMD_SEL_Find,       JLCMD_CAT_LOGO,     1,3,1,0, "Find"       },
	{ JLCMD_SEL_MkLogo,     JLCMD_CAT_LOGO,     1,3,1,0, "MkLogo"     },
	{ JLCMD_SEL_DivLogo,    JLCMD_CAT_LOGO,     1,3,1,0, "DivLogo"    },
	{ JLCMD_SEL_Select,     JLCMD_CAT_LOGO,     1,3,1,1, "Select"     },
	{ JLCMD_SEL_Force,      JLCMD_CAT_LOGO,     1,1,1,0, "Force"      },
	{ JLCMD_SEL_Abort,      JLCMD_CAT_LOGO,     1,0,1,0, "Abort"      },
	{ JLCMD_SEL_GetPos,     JLCMD_CAT_LOGO,     1,3,1,0, "GetPos"     },
	{ JLCMD_SEL_GetList,    JLCMD_CAT_LOGO,     1,3,1,0, "GetList"    },
	{ JLCMD_SEL_NextTail,   JLCMD_CAT_NEXT,     1,3,1,1, "NextTail"   },
	{ JLCMD_SEL_DivFile,    JLCMD_CAT_LOGO,     1,1,1,0, "DivFile" }
};
//--- �ʖ��ݒ� ---
static const int SIZE_JLSCR_CMDALIAS = 2;
const JlscrCmdAlias CmdAlias[SIZE_JLSCR_CMDALIAS] = {
	{ JLCMD_SEL_AutoIns,    "AutoInsert" },
	{ JLCMD_SEL_AutoDel,    "AutoDelete" }
};

//--- �R�}���h�I�v�V���� ---
static const int SIZE_JLOPT_DEFINE = 70;		// OptDefine�̍��ڐ���ݒ�i���ڐ��ύX���͕ύX�K�{�j
const JlOptionRecord  OptDefine[SIZE_JLOPT_DEFINE] = {
	{ JLOPT_CAT_NUMLOGO, CMDARG_LG_N,        0,  JLOPT_DATA_DUMMY,         "-N"        },
	{ JLOPT_CAT_NUMLOGO, CMDARG_LG_NR,       0,  JLOPT_DATA_DUMMY,         "-NR"       },
	{ JLOPT_CAT_NUMLOGO, CMDARG_LG_NLOGO,    0,  JLOPT_DATA_DUMMY,         "-Nlogo"    },
	{ JLOPT_CAT_NUMLOGO, CMDARG_LG_NAUTO,    0,  JLOPT_DATA_DUMMY,         "-Nauto"    },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_F,        12, JLOPT_DATA_MsecFrameLeft, "-F"        },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FR,       12, JLOPT_DATA_MsecFrameLeft, "-FR"       },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FHEAD,    12, JLOPT_DATA_MsecFrameLeft, "-FHead"    },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FTAIL,    12, JLOPT_DATA_MsecFrameLeft, "-FTail"    },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FMID,     0,  JLOPT_DATA_MsecFrameLeft, "-FMid"     },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FHEADX,   12, JLOPT_DATA_MsecFrameLeft, "-FHeadX"   },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FTAILX,   12, JLOPT_DATA_MsecFrameLeft, "-FTailX"   },
	{ JLOPT_CAT_FRAME,   CMDARG_FR_FMIDX,    0,  JLOPT_DATA_MsecFrameLeft, "-FMidX"    },
	{ JLOPT_CAT_SC,      CMDARG_SC_SC,       12, JLOPT_DATA_DUMMY,         "-SC"       },
	{ JLOPT_CAT_SC,      CMDARG_SC_NOSC,     12, JLOPT_DATA_DUMMY,         "-NoSC"     },
	{ JLOPT_CAT_SC,      CMDARG_SC_SM,       12, JLOPT_DATA_DUMMY,         "-SM"       },
	{ JLOPT_CAT_SC,      CMDARG_SC_NOSM,     12, JLOPT_DATA_DUMMY,         "-NoSM"     },
	{ JLOPT_CAT_SC,      CMDARG_SC_SMA,      12, JLOPT_DATA_DUMMY,         "-SMA"      },
	{ JLOPT_CAT_SC,      CMDARG_SC_NOSMA,    12, JLOPT_DATA_DUMMY,         "-NoSMA"    },
	{ JLOPT_CAT_SC,      CMDARG_SC_AC,       12, JLOPT_DATA_DUMMY,         "-AC"       },
	{ JLOPT_CAT_SC,      CMDARG_SC_NOAC,     12, JLOPT_DATA_DUMMY,         "-NoAC"     },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_SC,    12, JLOPT_DATA_DUMMY,         "-RSC"      },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_NOSC,  12, JLOPT_DATA_DUMMY,         "-RNoSC"    },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_SM,    12, JLOPT_DATA_DUMMY,         "-RSM"      },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_NOSM,  12, JLOPT_DATA_DUMMY,         "-RNoSM"    },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_SMA,   12, JLOPT_DATA_DUMMY,         "-RSMA"     },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_NOSMA, 12, JLOPT_DATA_DUMMY,         "-RNoSMA"   },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_AC,    12, JLOPT_DATA_DUMMY,         "-RAC"      },
	{ JLOPT_CAT_SC,      16+CMDARG_SC_NOAC,  12, JLOPT_DATA_DUMMY,         "-RNoAC"    },
	{ JLOPT_CAT_MSECM1,  3,                  23, JLOPT_DATA_MsecEndlenC,   "-EndLen"   },
	{ JLOPT_CAT_MSECM1,  3,                  23, JLOPT_DATA_MsecSftC,      "-Shift"    },
	{ JLOPT_CAT_MSECM1,  2,                  12, JLOPT_DATA_MsecTLHoldL,   "-TgtLimit" },
	{ JLOPT_CAT_MSECM1,  2,                  12, JLOPT_DATA_MsecLenPMin,   "-LenP"     },
	{ JLOPT_CAT_MSECM1,  2,                  12, JLOPT_DATA_MsecLenNMin,   "-LenN"     },
	{ JLOPT_CAT_MSECM1,  1,                  0,  JLOPT_DATA_MsecFromAbs,   "-fromabs"  },
	{ JLOPT_CAT_MSECM1,  1,                  0,  JLOPT_DATA_MsecFromHead,  "-fromhead" },
	{ JLOPT_CAT_MSECM1,  1,                  0,  JLOPT_DATA_MsecFromTail,  "-fromtail" },
	{ JLOPT_CAT_MSECM1,  2,                  12, JLOPT_DATA_MsecLogoExtL,  "-logoext"  },
	{ JLOPT_CAT_MSECM1,  1,                  0,  JLOPT_DATA_MsecEndAbs,    "-EndAbs"   },
	{ JLOPT_CAT_NUM,     1,                  0,  JLOPT_DATA_AutopCode,     "-code"     },
	{ JLOPT_CAT_NUM,     1,                  0,  JLOPT_DATA_AutopLimit,    "-limit"    },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopScope,    "-scope"    },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopScopeN,   "-scopen"   },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopScopeX,   "-scopex"   },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopPeriod,   "-period"   },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopMaxPrd,   "-maxprd"   },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopSecNext,  "-secnext"  },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopSecPrev,  "-secprev"  },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopTrScope,  "-trscope"  },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopTrSumPrd, "-trsumprd" },
	{ JLOPT_CAT_SEC,     1,                  0,  JLOPT_DATA_AutopTr1stPrd, "-tr1stprd" },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagWide,      "-wide"     },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagFromLast,  "-fromlast" },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagWithP,     "-withp"    },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagWithN,     "-withn"    },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagNoEdge,    "-noedge"   },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagOverlap,   "-overlap"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagConfirm,   "-confirm"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagUnit,      "-unit"     },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagElse,      "-else"     },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagCont,      "-cont"     },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagReset,     "-reset"    },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagFlat,      "-flat"     },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagForce,     "-force"    },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagNoForce,   "-noforce"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagAutoChg,   "-autochg"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagAutoEach,  "-autoeach" },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagEndHead,   "-EndHead"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagEndTail,   "-EndTail"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagEndHold,   "-EndHold"  },
	{ JLOPT_CAT_FLAG,    0,                  0,  JLOPT_DATA_FlagRelative,  "-relative" }
};

//--- setParam�R�}���h�ɂ��ݒ� �i�f�[�^�����l��dataset�Œ�`�j ---
const ConfigDataRecord ConfigDefine[SIZE_CONFIG_VAR] = {
	{ CONFIG_VAR_msecWLogoTRMax,      CONFIG_STR_MSEC,   "WLogoTRMax"  },
	{ CONFIG_VAR_msecWCompTRMax,      CONFIG_STR_MSEC,   "WCompTRMax"  },
	{ CONFIG_VAR_msecWLogoSftMrg,     CONFIG_STR_MSEC,   "WLogoSftMrg" },
	{ CONFIG_VAR_msecWCompFirst,      CONFIG_STR_MSEC,   "WCompFirst"  },
	{ CONFIG_VAR_msecWCompLast,       CONFIG_STR_MSEC,   "WCompLast"   },
	{ CONFIG_VAR_msecWLogoSumMin,     CONFIG_STR_MSEC,   "WLogoSumMin" },
	{ CONFIG_VAR_msecWLogoLgMin,      CONFIG_STR_MSEC,   "WLogoLgMin"  },
	{ CONFIG_VAR_msecWLogoCmMin,      CONFIG_STR_MSEC,   "WLogoCmMin"  },
	{ CONFIG_VAR_msecWLogoRevMin,     CONFIG_STR_MSEC,   "WLogoRevMin" },
	{ CONFIG_VAR_msecMgnCmDetect,     CONFIG_STR_MSEC,   "MgnCmDetect" },
	{ CONFIG_VAR_msecMgnCmDivide,     CONFIG_STR_MSEC,   "MgnCmDivide" },
	{ CONFIG_VAR_secWCompSPMin,       CONFIG_STR_SEC,    "WCompSpMin"  },
	{ CONFIG_VAR_secWCompSPMax,       CONFIG_STR_SEC,    "WCompSpMax"  },
	{ CONFIG_VAR_flagCutTR,           CONFIG_STR_NUM,    "CutTR"       },
	{ CONFIG_VAR_flagCutSP,           CONFIG_STR_NUM,    "CutSP"       },
	{ CONFIG_VAR_flagAddLogo,         CONFIG_STR_NUM,    "AddLogo"     },
	{ CONFIG_VAR_flagAddUC,           CONFIG_STR_NUM,    "AddUC"       },
	{ CONFIG_VAR_typeNoSc,            CONFIG_STR_NUM,    "TypeNoSc"    },
	{ CONFIG_VAR_cancelCntSc,         CONFIG_STR_NUM,    "CancelCntSc" },
	{ CONFIG_VAR_LogoLevel,           CONFIG_STR_NUM,    "LogoLevel"   },
	{ CONFIG_VAR_LogoRevise,          CONFIG_STR_NUM,    "LogoRevise"  },
	{ CONFIG_VAR_AutoCmSub,           CONFIG_STR_NUM,    "AutoCmSub"   },
	{ CONFIG_VAR_msecPosFirst,        CONFIG_STR_MSECM1, "PosFirst"    },
	{ CONFIG_VAR_msecLgCutFirst,      CONFIG_STR_MSECM1, "LgCutFirst"  },
	{ CONFIG_VAR_msecZoneFirst,       CONFIG_STR_MSECM1, "ZoneFirst"   },
	{ CONFIG_VAR_msecZoneLast,        CONFIG_STR_MSECM1, "ZoneLast"    },
	{ CONFIG_VAR_priorityPosFirst,    CONFIG_STR_NUM,    "LvPosFirst"  }
};


public:
	JlsScript(JlsDataset *pdata);
	virtual ~JlsScript();
	int  setOptionsGetOne(int argrest, const char* strv, const char* str1, const char* str2, bool overwrite);
	int  startCmd(const string &fname);

private:
	bool setJlsRegVar(const string &strName, const string &strVal, bool overwrite);
	int  getJlsRegVar(string &strVal, const string &strCandName, bool exact);
	Msec setOptionsCnvCutMrg(const char* str);
	bool setInputReg(const char *name, const char *val, bool overwrite);
	bool setInputFlags(const char *flags, bool overwrite);
	void checkInitial();
	int  startCmdLoop(const string &fname, int loop);
	bool startCmdGetLine(ifstream &ifs, string &strBufOrg, JlsScriptState &state);
	void startCmdDispErr(const string &strBuf, int errval);
	int  replaceBufVar(string &dstBuf, const string &srcBuf, int nowarn);
	int  replaceRegVarInBuf(string &strVal, const string &strBuf, int pos);
	int  getRegVarFromBuf(string &strVal, const string &strBuf, int pos);
	int  decodeCmd(JlsCmdArg &cmdarg, const string &strBuf);
	int  decodeCmdId(const string &cstr);
	int  decodeCmdArgMust(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos, int tpc, int tpw);
	int  decodeCmdArgOpt(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos);
	void decodeCmdTackOpt(JlsCmdArg &cmdarg);
	int  getCondFlag(const string &strBuf, int pos);
	void getCondFlagConnectWord(string &strCalc, const string &strItem);
	void setSystemRegInit();
	void setSystemRegUpdate();
	void setSystemRegHeadtail(int headframe, int tailframe);
	void setSystemRegPoshold(int msec_pos);
	void setSystemRegListhold(int msec_pos, bool clear);
	void setSystemRegNologo(bool need_check);
	void setSystemRegLastexe(bool exe_command);
	int  setSystemRegOptions(const string &strBuf, int pos, bool overwrite);
	void setCmdCondIf(JlcmdSelType cmdsel, bool flag_cond, JlsScriptState &state);
	int  setCmdCall(JlcmdSelType cmdsel, const string &strBuf, int pos, int loop);
	int  setCmdRepeat(JlcmdSelType cmdsel, const string &strBuf, int pos, JlsScriptState &state);
	int  setCmdSys(JlcmdSelType cmdsel, const string &strBuf, int pos);
	int  setCmdReg(JlcmdSelType cmdsel, const string &strBuf, int pos);
	int  setCmdRegParam(const string &strName, const string &strVal);
	bool exeCmd(JlsCmdSet &cmdset);
	bool exeCmdCallAutoScript(JlsCmdSet &cmdset);
	bool exeCmdCallAutoSetup(JlsCmdSet &cmdset);
	bool exeCmdCallAutoMain(JlsCmdSet &cmdset, bool setup_only);
	bool exeCmdAutoEach(JlsCmdSet &cmdset);
	bool exeCmdLogo(JlsCmdSet &cmdset);
	bool exeCmdLogoCheckTarget(JlsCmdSet &cmdset, int nlist);
	bool exeCmdLogoTarget(JlsCmdSet &cmdset);
	bool exeCmdNextTail(JlsCmdSet &cmdset);
	void limitHeadTail(JlsCmdSet &cmdset);
	void limitHeadTailImm(JlsCmdSet &cmdset, RangeMsec rmsec);
	void limitWindow(JlsCmdSet &cmdset);
	void limitListForTarget(JlsCmdSet &cmdset);
	int  limitLogoList(JlsCmdSet &cmdset);
	bool limitLogoListSub(JlsCmdArg &cmdarg, LogoMarkExist exist, LogoMarkCount ncur, LogoMarkCount nmax);
	bool limitTargetLogo(JlsCmdSet &cmdset, int nlist);
	bool limitTargetLogoGet(JlsCmdSet &cmdset, int nlist);
	bool limitTargetLogoCheck(JlsCmdSet &cmdset);
	bool limitTargetRangeByLogo(JlsCmdSet &cmdset);
	void limitTargetRangeByImm(JlsCmdSet &cmdset, WideMsec wmsec, bool force);
	void getTargetPoint(JlsCmdSet &cmdset);
	Nsc  getTargetPointEndlen(JlsCmdArg &cmdarg, int msec_base);
	Nsc  getTargetPointEndArg(JlsCmdArg &cmdarg, int msec_base);
	void getTargetPointSetScpEnable(JlsCmdSet &cmdset);
	void getPrevNextLogo(WideMsec &wmsec_lg, int nrf, int nsc, LogoEdgeType edge);
	bool checkOptScpFromMsec(JlsCmdArg &cmdarg, int msec_base, LogoEdgeType edge, bool chk_base, bool chk_rel);
	void sortTwoValM1(int &val_a, int &val_b);


private:
	//--- �֐� ---
	JlsDataset *pdata;									// ���̓f�[�^�A�N�Z�X
	unique_ptr <JlsAutoScript>	m_funcAutoScript;		// �����\�������X�N���v�g

	//--- �ێ��f�[�^ ---
	JlsRegFile   m_regvar;				// set/default�R�}���h�ɂ��ϐ��l�̕ێ�
	string       m_pathNameJL;			// JL�X�N���v�g��Path

	//--- ���s���� ---
	bool m_lastexe;						// ���O�s�̎��s�L��
	bool m_exe1st;						// ���s����̐ݒ�p
};

