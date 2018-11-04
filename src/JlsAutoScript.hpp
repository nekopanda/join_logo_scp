//
// Auto�nJL�R�}���h���s�p
//
#pragma once

class JlsCmdArg;
class JlsCmdLimit;
class JlsCmdSet;
class JlsAutoReform;
class JlsDataset;


///////////////////////////////////////////////////////////////////////
//
// Auto�n�p�����[�^�ێ��N���X
//
///////////////////////////////////////////////////////////////////////
class JlsAutoArg
{
public:
	void	setParam(JlsCmdArg &cmdarg, JlcmdAutoType cmdtype);
	int		getParam(JlParamAuto type);

private:
	void	clearAll();
	void	setVal(JlParamAuto type, int val);
	void	setParamCutTR(JlsCmdArg &cmdarg);
	void	setParamCutEC(JlsCmdArg &cmdarg);
	void	setParamAdd(JlsCmdArg &cmdarg);
	void	setParamEdge(JlsCmdArg &cmdarg);
	void	setParamInsDel(JlsCmdArg &cmdarg);
private:
	// �R�}���h���
	JlcmdAutoType m_cmdtype;
	// �i�[
	int m_enable_prm[SIZE_PARAM_AUTO];
	int m_val_prm[SIZE_PARAM_AUTO];
};


///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g�̎��������R�}���h
//
///////////////////////////////////////////////////////////////////////
class JlsAutoScript
{
private:
// AutoAdd�Ŕ��f����w��ʒu�̑O���Ԃ���D�揇�ʎZ�o���f�[�^�擾
struct AddLocInfo {
	int typeLogo;			// ���S����̏�ԁi0-9:���S�Ȃ� 10-19:���S���� +100:���S�J�n�� +1000:�O���j
	int typeTr;				// �\������̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O�� 3:�ŏ��̗\���ʒu 4:�\���Ԍ��ԁj
	int typeSp;				// �ԑg�񋟂���̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O�� 3:�����j
	int typeEc;				// �G���h�J�[�h����̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O���j
	Sec secDifLogo;			// ���S����̎���
	Sec secDifTr;			// �\������̎���
	Sec secDifSp;			// �ԑg�񋟂���̎���
	Sec secDifEc;			// �G���h�J�[�h����̎��ԁi�O���̎��̂݌v���j
	int typeNolast;			// �\���ł͂Ȃ���ɓ��e������ꍇ=1
	int typeEndlogo;		// �͈͓��Ɋ��S�Ɋ܂܂��Ō�̃��S����̏�ԁi0:�Y�����S�Ȃ� 1:���S�ȍ~ 2:���S��O���j
	Sec secDifSc;			// �P�O�̔ԑg�\������̎���
};
// AutoAdd�ō\���擾���̏��
struct AddExistInfo {
	bool trailer;			// �\�������݂��邩
	bool sponsor;			// �ԑg�񋟂����݂��邩
	Sec  sec_tr;			// �\�����ԍ��v
	bool divideCm;			// CM�\�����������s��
};

public:
	JlsAutoScript(JlsDataset *pdata);
	virtual ~JlsAutoScript();
	bool startCmd(JlsCmdSet &cmdset, bool setup_only);

private:
	void checkFirstAct(JlsCmdArg &cmdarg);
	bool exeCmdMain(JlsCmdSet &cmdset);
	jlscmd::JlcmdAutoType exeCmdParam(JlsCmdArg &cmdarg);
	bool startAutoUp();
	bool startAutoBorder(RangeMsec autoscope);
	bool startAutoChg(JlsCmdLimit &cmdlimit);
	Nsc  getNscElgFromNrf(Nrf nrf_base);
	bool startAutoIns(JlsCmdLimit &cmdlimit);
	bool startAutoDel(JlsCmdLimit &cmdlimit);
	bool subInsDelGetRange(Nsc &nsc_target, Nsc &nsc_base, JlsCmdLimit &cmdlimit);
	Nsc  subInsDelGetBase(JlsCmdLimit &cmdlimit);
	bool startAutoCutTR(RangeMsec autoscope);
	Nsc  subCutTRGetLocSt(RangeMsec autoscope);
	void subCutTRGetLocStSub(Nsc *r_nsc_cand, bool *r_flag_cand, RangeMsec autoscope, ElgCurrent elg);
	void subCutTRSetCut(Term &term);
	bool startAutoCutEC(RangeMsec autoscope);
	int  subCutECGetLocSt(vector<int> &local_cntcut, int *r_ovw_force, RangeMsec autoscope);
	bool subCutECCheckScope(Sec sec_dif, int prm_c_sel, int prm_period, int prm_maxprd);
	bool startAutoAddSP(RangeMsec autoscope);
	bool startAutoAddEC(RangeMsec autoscope);
	bool startAutoAddTR(RangeMsec autoscope);
	int  subAddGetLimit(AddExistInfo &exist_info, JlcmdAutoType cmdtype, RangeMsec autoscope);
	Nsc  subAddSearch(JlcmdAutoType cmdtype, AddExistInfo exist_info, RangeMsec autoscope);
	void subAddGetLocInfo(AddLocInfo &locinfo, Term target, RangeMsec autoscope);
	int  subAddGetPriority(AddLocInfo &locinfo, JlcmdAutoType cmdtype, ScpArType arstat_cur, ScpArExtType arext_cur, AddExistInfo exist_info);
	bool subAddCheckSec(Nsc nsc_cur, Sec difsec);
	void subAddCancelCut(Nsc nsc_prior, RangeMsec autoscope);
	void subAddReviseUnitCm(Nsc nsc_target);
	bool startAutoEdge(JlsCmdLimit &cmdlimit);
	bool startAutoEdgeMain(Nsc nsc_elg, LogoEdgeType edge_elg);
	bool subEdgeExec(Nsc nsc_logo, LogoEdgeType edge_logo, SearchDirType dr);
	bool getElgNextKeep(ElgCurrent &elg);
	int  getConfig(ConfigVarType tp);
	int  getAutoParam(JlParamAuto tp);
	void setTermEndtype(Term &term, ScpEndType endtype);
	bool getTermNext(Term &term);
	bool getTermPrev(Term &term);
	ScpArType getScpArstat(Term term);
	ScpArExtType getScpArext(Term term);
	void setScpArstat(Term term, ScpArType arstat);
	void setScpArext(Term term, ScpArExtType arext);
	bool isScpArstatCmUnit(Term term);
	bool checkScopeTerm(Term term, RangeMsec scope);
	bool checkScopeRange(RangeMsec bounds, RangeMsec scope);
	Msec calcDifGap(Msec msec1, Msec msec2);

private:
	//--- �f�[�^�֐��|�C���^ ---
	JlsDataset 		*pdata;

	//--- �ێ��f�[�^ ---
	JlsAutoArg		m_autoArg;				// JL�R�}���h�iAuto�n�j���ݍs�̓��e�f�[�^
};
