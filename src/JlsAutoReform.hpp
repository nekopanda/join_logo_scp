//
// join_logo_scp : Auto�nCM�\����������
//
#pragma once

class JlsDataset;
class JlsCmdArg;


class JlsAutoReform
{
private:
	//--- �\���\�z�p ---
	static const int SIZE_SETFORM_KEEP = 6;		// CM�\�����o���̗���ێ���
	//--- ���S�Ȃ�CM�\�������p ---
	enum AutoCMStateType {			// �i���S�Ȃ�CM�\�������p�j���
		AUTOCM_ST_S0N_FIRST,		// �擪CM
		AUTOCM_ST_S1L_LOGO,			// �ʏ탍�S
		AUTOCM_ST_S2N_CM,			// �ʏ�CM
		AUTOCM_ST_S3L_DEFER,		// �������f�摗�胍�S
		AUTOCM_ST_S4N_JUDGE,		// �s�m��摗��n�_��CM���f
		AUTOCM_NUM_SMAX				// ��Ԑ��ő�
	};
	enum AutoCMCommandType {		// �i���S�Ȃ�CM�\�������p�j�R�}���h���
		AUTOCM_T_INIT,				// �R�}���h�F������
		AUTOCM_T_SFT,				// �R�}���h�F���s�ړ�
		AUTOCM_T_MRG				// �R�}���h�F����
	};
	enum AutoFirstType {			// �擪���S�̃J�b�g����
		AUTOFIRST_LAX,				// �ɂ�
		AUTOFIRST_STRICT,			// ������
		AUTOFIRST_NOCUT				// �J�b�g�Ȃ�
	};

	struct FormAutoCMArg {			// AutoCM���s�Ɏg�p������͏��
		int  levelCmDet;			// CM���o���x��
		Msec msecTailareaKeep;		// �Z���Ԃ�CM�����p������Ōォ��̈ʒu
		Msec msecTailareaUnit;		// 60/90/120�b�\����CM��������Ōォ��̈ʒu
		Sec  secMaxCm;				// �w��b�𒴂���\����15�b�P�ʂł�CM�����ɂ��Ȃ�
		RangeMsec rmsecHeadTail;	// �擪�����ʒu
	};
	struct AutoCMCount {			// �i���S�Ȃ�CM�\�������p�j�J�E���^
		RangeNsc rnsc;				// �J�n�I���ʒu�V�[���`�F���W�ԍ�
		int det;					// �\�����i���v�j�J�E���g
		Sec sec;					// �b���i���v�j�J�E���g
		int d15;					// �\�����i15�b�j�J�E���g
		int d30;					// �\�����i30�b�j�J�E���g
		int dov15;					// �\�����i15�b�ȏ�j�J�E���g
	};
	struct FormFirstInfo {			// �擪�J�b�g���ʏ��
		AutoFirstType limitHead;	// �擪����J�b�g���j�i0:�ɂ� 1:������ 2:�Ȃ��j
		Nsc nscTreat;				// �擪�̊m��ʒu�ԍ�
	};
	struct FormFirstLoc {			// �擪�J�b�g�������̈ʒu���
		Nsc  nscDecide;				// �\���m��ʒu�̃V�[���`�F���W�ԍ�
		Nsc  nscLogo1st;			// ���S�ɑΉ�����V�[���`�F���W�ԍ�
		Msec msecDecide;			// �\���m��ʒu
		Msec msecLogo1st;			// ���S�J�n�ʒu
		RangeNsc rnscLogo1st;		// ���S�Ƃ��Č�������͈̓V�[���`�F���W�ԍ�
		int  lvPos1stSel;			// �i�ݒ�j�J�n�}�[�W���ݒ�
		Msec msec1stSel;			// �i�ݒ�j�J�n�}�[�W���ʒu
		Msec msecEnd1st;			// �i�ݒ�j�����I���ʒu
		Msec msec1stZone;			// �i�ݒ�j�擪����w����ԂɑS�̂��܂܂��\����CM
		Msec msecWcomp1st;			// �i�ݒ�j��ԍŏ��̍\��CM������
		Msec msecLgCut1st;			// �i�ݒ�j��ԍŏ��Ƀ��S���莞�̍\��CM������
	};
	struct FormCMByLogo {			// CM�\�����o�Ŏg�p����f�[�^
		Msec msecTarget;			// �������S�ʒu
		Msec msecLimit;				// �␳�\���E�n�_
		Msec msecCmSide;			// �����͈̓��S�ʒu�iCM���j
		Msec msecLogoSide;			// �����͈̓��S�ʒu�i���S���j
		int  revDelEdge;			// ���S�[CM�폜�i0=�\������ʒu 1=���S�ʒu 2=���S�ʒu�ōŏ��� 3=�폜�Ȃ��j
		SearchDirType dr;			// �����iSEARCH_DIR_PREV/SEARCH_DIR_NEXT�j
	};
	struct FormCMEdgeSide {
		bool endSide;				// �[���ifalse=�J�n��, true=�I�����j
		Nsc  nscDetect;				// ���ʒu
		Nsc  nscFixed;				// �m��ʒu
		Nsc  nscOther;				// ���̊m��ʒu�Ƃ͔��Α��̊m��ʒu
		bool logoModePrev;			// �O���̃��S�L��
		bool logoModeNext;			// �㑤�̃��S�L��
	};
	struct FormLogoLevelExt {		// ���S���x���ɑΉ�����F�����Ԃ̔���p
		bool en_long;				// �����Ԃ�30�b�P�ʍ\�����܂�
		bool longmod;				// �����Ԃ�5�b/15�b�P�ʂ��܂�
		bool mid5s;					// ���S���x��MIDL��5�b�P�ʂŌ��o
	};
	struct TraceInterpolar {		// �Q�_�ԍ\���������̈ꎞ�ێ��f�[�^
		int keepType;				// �ێ��\���̎�ށi0=�ێ��Ȃ� 1=��₠�� 2=����̑傫����₠��j
		int keepGap;				// �ێ��\���ꏊ�̌덷�~���b
		Nsc nscKeep;				// �ێ��\���ԍ�
		Nsc nscBase;				// �m��\���ԍ�
		bool keep15s;				// ����15�b�P�ʍ\��
	};
	struct TraceChap {					// chap�������̈ꎞ�ێ��f�[�^
		int numDa;						// ���o�́F�ێ��f�[�^���i�p�����j
		int numDb;						// ���o�́F�ێ��f�[�^���i�V���j
		Nsc nscDa[SIZE_SETFORM_KEEP];	// ���o�́F�ێ��f�[�^�̌p�����ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
		Nsc nscDb[SIZE_SETFORM_KEEP];	// ���o�́F�ێ��f�[�^�̐V���ʒu�i�T�C�Y=SIZE_SETFORM_KEEP�j
	};

public:
	JlsAutoReform(JlsDataset *pdata);
	void mkReformAll(JlsCmdArg &cmdarg);
	bool startAutoCM(JlsCmdArg &cmdarg);
	void mkReformTarget(Nsc nsc_from, Nsc nsc_to, bool logoon, int restruct);

private:
	void mkReformAllLogo();
	bool mkReformAllLogoUnit(Msec &msec_lg_remain_st, NrfCurrent &logopt);
	void mkReformAllLogoUpdate(RangeMsec rmsec_new);
	void mkReformAllNologo(JlsCmdArg &cmdarg);
	void setReformParam(FormAutoCMArg &param_autocm, JlsCmdArg &cmdarg, bool reform_all);
// �\���F����̒ǉ�����
	void addLogoComponent(int lvlogo);
	void addLogoEdge();
	void addLogoEdgeUpdate(RangeWideMsec area_nologo, int msec_recmin);
	void addLogoMix();
	void addLogoBorder();
	void addLogoBorderSet();
	Msec addLogoBorderSub(RangeMsec component, RangeNrf rnrf_logo, bool enable_st);
	Msec addLogoBorderSubUpdate(RangeMsec component, RangeMsec logo_targets, bool rise_logost, bool enable_st);
// ���S�Ȃ����̍\���F��
	bool detectCM(FormAutoCMArg param_autocm);
	void detectCMCount(AutoCMCount *cntset, AutoCMCommandType type, AutoCMStateType st, AutoCMStateType ed);
	bool detectCMSetLogo(RangeNsc rnsc);
	bool detectCMCancelShort(RangeNsc rnsc);
	void detectCMAssistLogo(int nsc_det1st, int nsc_detend);
// �w���Ԃ𐄑����s���č\���F��
	bool mkRangeInterLogo(RangeNsc nscbounds, bool cm_inter);
	bool mkRangeCM(RangeMsec &rmsec_result, RangeFixMsec scope_org, bool logo1st, int interfill);
	bool mkRangeCMGetLogoEdge(WideMsec &wmsec, int msec_target, bool flag_fix, LogoEdgeType edge);
// �S�̂ɂ�����擪�ƍŌ�̍\���F������
	bool setFirstArea(FormFirstInfo &info_first);
	bool setFirstAreaUpdate(FormFirstInfo &info_first, FormFirstLoc locinfo);
	bool setFinalArea();
// 2�_�Ԃ̍\���m�菈��
	bool setInterLogo(RangeNsc nscbounds, bool cm_inter);
	bool setInterLogoUpdate(Nsc nsc_fin, RangeNsc cmterm, bool shortcm);
	bool setInterMultiCM(RangeNsc nscbounds, int interfill);
	bool setInterpolar(RangeNsc rnsc_target, bool logomode);
	void setInterpolarClearChap(Nsc nsc_n);
	bool setInterpolarDetect(TraceInterpolar &trace, Nsc nsc_cur, RangeNscMsec target, bool logomode, int nside);
	bool setInterpolarExtra(RangeNscMsec target, bool logomode);
// �w����ԓ��Ɋ܂܂��CM�\�������o
	bool setCMForm(RangeMsec &bounds, RangeWideMsec cmscope, bool logo1st, bool logointer);
	bool setCMFormDetect(Msec &msec_stpoint, Nsc nsc_base, RangeWideMsec findscope, bool logoon_st);
	void setCMFormDetRevise(RangeMsec &bounds, bool fix_mode);
	bool setCMFormByLogo(RangeMsec &bounds, RangeWideMsec cmscope);
	int  setCMFormByLogoLimit(Msec msec_point, SearchDirType dr);
	bool setCMFormByLogoAdd(Msec &msec_result, FormCMByLogo form);
	bool setCMFormEdge(RangeMsec &rmsec_result, RangeWideMsec cmscope, bool logo1st);
	bool setCMFormEdgeSideInfo(FormCMEdgeSide &sidesel, Msec msec_bounds, WideMsec wmsec_scope, ScpChapType chap_cmp, bool endside);
	bool setCMFormEdgeSetSide(FormCMEdgeSide &sidesel, int level);
	bool setCMFormEdgeSetBoth(RangeNsc rnsc_detect, RangeNsc rnsc_scope, int level, bool logo1st);
// �e��؂�̃X�R�A����э\�������o
	void setScore(RangeMsec scope);
	void setScore(RangeFixMsec fixscope);
	void setChap(RangeFixMsec fixscope);
	void setChapUpdate(TraceChap &trace, Nsc nsc_cur);
	void setChapGuess(TraceChap &trace, Nsc nsc_cur);
	int  setChapMaxscore(Nsc nsc_target);
	int  setChapGetDistanceScore(Nsc nsc, Msec msec_dif);
	bool setChapFixed(TraceChap &trace);
	bool setChapFixedLimit(TraceChap &trace);
	void setChapThinOverlap(RangeFixMsec fixscope);
	int  getScore(Msec msec_target, RangeMsec scope);
	int  getScore(Msec msec_target, RangeWideMsec range_wide_scope);
	int  getScoreDist(Nsc &nsc_cur, Msec msec_cmp);
	int  getScoreTarget(Msec msec_dist, int sec_width, bool smute_det, bool still_det);
// ���ʏ���
	int  checkMuteMany(RangeNsc rnsc);
	void addCommonComponentOne(Nsc nsc_target, Nsc nsc_side, bool enable_short);
	int  calcDifSelect(CalcDifInfo &calcdif, Msec msec_src, Msec msec_dst);
	int  calcDifMod0515(CalcModInfo &calcmod, Msec msec_dif);
	RangeMsec getRangeMsec(RangeNsc range_nsc);
	bool isCmLengthMsec(Msec msec_target);
	bool isLengthLogoLevel(Msec msec_target, FormLogoLevelExt extype);

private:
	//--- �f�[�^�֐��|�C���^ ---
	JlsDataset *pdata;

};

