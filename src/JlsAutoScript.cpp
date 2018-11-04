//
// join_logo_scp Auto�R�}���h����
//
#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsAutoScript.hpp"
#include "JlsAutoReform.hpp"
#include "JlsCmdSet.hpp"
#include "JlsDataset.hpp"


///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�gAuto�n�p�����[�^�ێ�
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// �p�����[�^�ݒ�
//---------------------------------------------------------------------
void JlsAutoArg::setParam(JlsCmdArg &cmdarg, JlcmdAutoType cmdtype){
	clearAll();
	m_cmdtype = cmdtype;
	switch(cmdtype){
		case JLCMD_AUTO_CUTTR :
			setParamCutTR(cmdarg);
			break;
		case JLCMD_AUTO_CUTEC :
			setParamCutEC(cmdarg);
			break;
		case JLCMD_AUTO_ADDTR :
		case JLCMD_AUTO_ADDSP :
		case JLCMD_AUTO_ADDEC :
			setParamAdd(cmdarg);
			break;
		case JLCMD_AUTO_EDGE :
			setParamEdge(cmdarg);
			break;
		case JLCMD_AUTO_ATUP :
		case JLCMD_AUTO_ATBORDER :
		case JLCMD_AUTO_INS :
		case JLCMD_AUTO_DEL :
			setParamInsDel(cmdarg);
			break;
		default :
			break;
	}
}


//---------------------------------------------------------------------
// �p�����[�^�擾
//---------------------------------------------------------------------
int JlsAutoArg::getParam(JlParamAuto type){
	if (type >= 0 && type < SIZE_PARAM_AUTO){
		if (m_enable_prm[type] > 0){
			return m_val_prm[type];
		}
	}
	cerr << "unexpected error JlsAutoArg::GetParam cmdtype=" << m_cmdtype << " type=" << type << endl;
	return 0;
}


//---------------------------------------------------------------------
// �p�����[�^������
//---------------------------------------------------------------------
void JlsAutoArg::clearAll(){
	for(int i=0; i<SIZE_PARAM_AUTO; i++){
		m_enable_prm[i] = 0;
		m_val_prm[i]    = 0;
	}
}

//---------------------------------------------------------------------
// ���[�h�ʂ̃p�����[�^�ݒ�
//---------------------------------------------------------------------
void JlsAutoArg::setVal(JlParamAuto type, int val){
	if (type >= 0 && type < SIZE_PARAM_AUTO){
		m_enable_prm[type] = 1;
		m_val_prm[type]    = val;
	}
}

void JlsAutoArg::setParamCutTR(JlsCmdArg &cmdarg){
	//--- �p�����[�^�擾 ---
	int autop_code     = cmdarg.getOpt(JLOPT_DATA_AutopCode);
	int tmp_trscope    = cmdarg.getOpt(JLOPT_DATA_AutopTrScope);
	int tmp_trsumprd   = cmdarg.getOpt(JLOPT_DATA_AutopTrSumPrd);
	int prm_v_limit    = cmdarg.getOpt(JLOPT_DATA_AutopLimit);
	int prm_v_tr1stprd = cmdarg.getOpt(JLOPT_DATA_AutopTr1stPrd);
	int prm_c_from     = autop_code % 10;
	int prm_c_cutst    = (autop_code / 10) % 10;
	int prm_c_lgpre    = (autop_code / 100) % 10;
	int prm_c_noedge   = 1;
	//--- ���s���f ---
	int prm_c_exe      = (prm_c_from != 0)? 1 : 0;
	//--- �f�t�H���g�l�t���ݒ� ---
	int prm_v_trscope  = (tmp_trscope  == 0)? 30 : tmp_trscope;
	int prm_v_trsumprd = (tmp_trsumprd == 0)?  3 : tmp_trsumprd;

	setVal(PARAM_AUTO_v_limit    , prm_v_limit    );
	setVal(PARAM_AUTO_v_tr1stprd , prm_v_tr1stprd );
	setVal(PARAM_AUTO_c_from     , prm_c_from     );
	setVal(PARAM_AUTO_c_cutst    , prm_c_cutst    );
	setVal(PARAM_AUTO_c_lgpre    , prm_c_lgpre    );
	setVal(PARAM_AUTO_c_exe      , prm_c_exe      );
	setVal(PARAM_AUTO_v_trscope  , prm_v_trscope  );
	setVal(PARAM_AUTO_v_trsumprd , prm_v_trsumprd );
	setVal(PARAM_AUTO_c_noedge   , prm_c_noedge   );
}


void JlsAutoArg::setParamCutEC(JlsCmdArg &cmdarg){
	//--- �p�����[�^�擾 ---
	int autop_code     = cmdarg.getOpt(JLOPT_DATA_AutopCode);
	int tmp_period   = cmdarg.getOpt(JLOPT_DATA_AutopPeriod);
	int tmp_maxprd   = cmdarg.getOpt(JLOPT_DATA_AutopMaxPrd);
	int prm_v_limit  = cmdarg.getOpt(JLOPT_DATA_AutopLimit);
	int prm_c_sel    = cmdarg.getOpt(JLOPT_DATA_AutopCode) % 10;
	int prm_c_cutla  = ((((autop_code / 10) % 10) & 0x1) != 0)? 1 : 0;
	int prm_c_cutlp  = ((((autop_code / 10) % 10) & 0x2) != 0)? 1 : 0;
	int prm_c_cut30  = 0;
	int prm_c_cutsp  = ((((autop_code / 100) % 10) & 0x1) != 0)? 1 : 0;
	int prm_c_noedge   = 1;
	//--- �f�t�H���g�l�t���ݒ� ---
	int prm_v_period = (tmp_period == 0)?   5 : tmp_period;
	int prm_v_maxprd = (tmp_maxprd == 0)?  13 : tmp_maxprd;

	setVal(PARAM_AUTO_v_limit  , prm_v_limit );
	setVal(PARAM_AUTO_c_sel    , prm_c_sel );
	setVal(PARAM_AUTO_c_cutla  , prm_c_cutla );
	setVal(PARAM_AUTO_c_cutlp  , prm_c_cutlp );
	setVal(PARAM_AUTO_c_cut30  , prm_c_cut30 );
	setVal(PARAM_AUTO_c_cutsp  , prm_c_cutsp );
	setVal(PARAM_AUTO_v_period , prm_v_period );
	setVal(PARAM_AUTO_v_maxprd , prm_v_maxprd );
	setVal(PARAM_AUTO_c_noedge , prm_c_noedge );
}


void JlsAutoArg::setParamAdd(JlsCmdArg &cmdarg){
	int default_c_wmin, default_c_wmax;
	int default_scope, default_period, default_search;
	int enable_w15, enable_in1;

	//--- �R�}���h�ɂ��Ⴂ���� ---
	if (m_cmdtype == JLCMD_AUTO_ADDSP){		// �ԑg��
		//--- �f�t�H���g�l ---
		default_c_wmin = 6;					// �ŏ����ԕb��
		default_c_wmax = 13;				// �ő���ԕb��
		default_scope  = 90;				// �����͈͕b��
		default_period = 5;					// �ݒ���ԕb��
		default_search = 1;					// �����͈͐ݒ�
		//--- �t���O�ݒ� ---
		enable_w15     = 1;					// 15�b�̌���
		enable_in1     = 1;					// �\���ɑ}��
	}
	else if (m_cmdtype == JLCMD_AUTO_ADDEC){	// �G���h�J�[�h
		//--- �f�t�H���g�l ---
		default_c_wmin = 1;					// �ŏ����ԕb��
		default_c_wmax = 13;				// �ő���ԕb��
		default_scope  = 90;				// �����͈͕b��
		default_period = 5;					// �ݒ���ԕb��
		default_search = 1;					// �����͈͐ݒ�
		//--- �t���O�ݒ� ---
		enable_w15     = 0;					// 15�b�̌���
		enable_in1     = 0;					// �\���ɑ}��
	}
	else{
		//--- �f�t�H���g�l ---
		default_c_wmin = 1;					// �ŏ����ԕb��
		default_c_wmax = 13;				// �ő���ԕb��
		default_scope  = 90;				// �����͈͕b��
		default_period = 5;					// �ݒ���ԕb��
		default_search = 1;					// �����͈͐ݒ�
		//--- �t���O�ݒ� ---
		enable_w15     = 0;					// 15�b�̌���
		enable_in1     = 0;					// �\���ɑ}��
	}

	//--- �p�����[�^�擾 ---
	int autop_code     = cmdarg.getOpt(JLOPT_DATA_AutopCode);
	int tmp_c_w        = autop_code % 10;				// �����b������p���Ԓl
	int tmp_c_sea      = (autop_code / 10) % 10;		// �����͈�
	int tmp_c_lg       = (autop_code / 100) % 10;		// ���S��Ԍ���
	int tmp_c_p        = (autop_code / 1000) % 10;
	int tmp_c_lim      = (autop_code / 10000) % 10;
	int tmp_c_unit     = (autop_code / 100000) % 10;
	int tmp_period     = cmdarg.getOpt(JLOPT_DATA_AutopPeriod);
	int tmp_maxprd     = cmdarg.getOpt(JLOPT_DATA_AutopMaxPrd);
	int tmp_scope      = cmdarg.getOpt(JLOPT_DATA_AutopScope);
	int prm_v_scopen   = cmdarg.getOpt(JLOPT_DATA_AutopScopeN);
	int prm_v_limit    = cmdarg.getOpt(JLOPT_DATA_AutopLimit);
	int prm_v_secnext  = cmdarg.getOpt(JLOPT_DATA_AutopSecNext);
	int prm_v_secprev  = cmdarg.getOpt(JLOPT_DATA_AutopSecPrev);
	int prm_v_trsumprd = cmdarg.getOpt(JLOPT_DATA_AutopTrSumPrd);
	int prm_c_noedge   = 1;
	bool is_scope       = cmdarg.isSetOpt(JLOPT_DATA_AutopScope);
	bool is_period      = cmdarg.isSetOpt(JLOPT_DATA_AutopPeriod);
	bool is_maxprd      = cmdarg.isSetOpt(JLOPT_DATA_AutopMaxPrd);

	//--- ���s���f ---
	int prm_c_exe = (tmp_c_w != 0)? 1 : 0;
	//--- �f�t�H���g�l�t���ݒ� ---
	int prm_v_scope    = ( !is_scope     )? default_scope  : tmp_scope;
	int prm_v_period   = ( !is_period    )? default_period : tmp_period;
	int prm_v_maxprd   = ( !is_maxprd    )? default_c_wmax : tmp_maxprd;
	int prm_c_search   = (tmp_c_sea  == 0)? default_search : tmp_c_sea;
	//--- ���S���� ---
	int prm_c_lgy     = ((tmp_c_lg & 0x3) != 1)? 1 : 0;	// ���S�t������
	int prm_c_lgn     = ((tmp_c_lg & 0x3) != 2)? 1 : 0;	// ���S�Ȃ�����
	int prm_c_lgbn    = ((tmp_c_lg & 0x3) == 3)? 1 : 0;	// ���ׂ��܂߃��S�Ȃ�����
	int prm_c_cutskip = ((tmp_c_lg & 0x4) != 0)? 1 : 0;	// �\���J�b�g�ȍ~���L��
	//--- �ʒu���� ---
	int prm_c_lgprev = (tmp_c_p == 1 || tmp_c_p == 3 || (enable_in1 > 0 && tmp_c_p >= 4))? 0 : 1;	// �O����ΏۊO
	int prm_c_lgpost = (tmp_c_p == 2 || tmp_c_p == 3 || (enable_in1 > 0 && tmp_c_p >= 4))? 0 : 1;	// �㑤��ΏۊO
	int prm_c_lgintr = (tmp_c_p == 3)? 1 : 0;	// �Ԃ��c��
	int prm_c_lgsp   = (tmp_c_p == 4 && enable_in1 == 0)? 1 : 0;	// �ԑg�񋟁E�G���h�J�[�h������ɂ���ꍇ�̂ݑΏ�
	int prm_c_limloc    = ((tmp_c_lim  & 0x1) > 0)? 1 : 0;
	int prm_c_limtrsum  = ((tmp_c_lim  & 0x2) > 0)? 1 : 0;
	int prm_c_unitcmoff = ((tmp_c_unit & 0x1) > 0)? 1 : 0;
	int prm_c_unitcmon  = ((tmp_c_unit & 0x2) > 0)? 1 : 0;
	int prm_c_wdefmin   = default_c_wmin;
	int prm_c_wdefmax   = default_c_wmax;

	setVal(PARAM_AUTO_v_scopen    , prm_v_scopen );
	setVal(PARAM_AUTO_v_limit     , prm_v_limit );
	setVal(PARAM_AUTO_v_secnext   , prm_v_secnext );
	setVal(PARAM_AUTO_v_secprev   , prm_v_secprev );
	setVal(PARAM_AUTO_v_trsumprd  , prm_v_trsumprd );
	setVal(PARAM_AUTO_c_exe       , prm_c_exe );
	setVal(PARAM_AUTO_v_scope     , prm_v_scope );
	setVal(PARAM_AUTO_v_period    , prm_v_period );
	setVal(PARAM_AUTO_v_maxprd    , prm_v_maxprd );
	setVal(PARAM_AUTO_c_search    , prm_c_search );
	setVal(PARAM_AUTO_c_lgy       , prm_c_lgy );
	setVal(PARAM_AUTO_c_lgn       , prm_c_lgn );
	setVal(PARAM_AUTO_c_lgbn      , prm_c_lgbn );
	setVal(PARAM_AUTO_c_cutskip   , prm_c_cutskip );
	setVal(PARAM_AUTO_c_lgprev    , prm_c_lgprev );
	setVal(PARAM_AUTO_c_lgpost    , prm_c_lgpost );
	setVal(PARAM_AUTO_c_lgintr    , prm_c_lgintr );
	setVal(PARAM_AUTO_c_lgsp      , prm_c_lgsp );
	setVal(PARAM_AUTO_c_limloc    , prm_c_limloc );
	setVal(PARAM_AUTO_c_limtrsum  , prm_c_limtrsum );
	setVal(PARAM_AUTO_c_unitcmoff , prm_c_unitcmoff );
	setVal(PARAM_AUTO_c_unitcmon  , prm_c_unitcmon );
	setVal(PARAM_AUTO_c_wdefmin   , prm_c_wdefmin );
	setVal(PARAM_AUTO_c_wdefmax   , prm_c_wdefmax );
	setVal(PARAM_AUTO_c_noedge    , prm_c_noedge );

	//--- 15�b����ݒ�itrailer�ŃJ�b�g���ꂽ���������ɓ����j ---
	int prm_c_w15 = 0;
	if (enable_w15 > 0){
		prm_c_w15  = ((tmp_c_w == 6) ||
					  ((tmp_c_w == 3) && (prm_v_period % 15 == 0)))? 1 : 0;
	}
	//--- �\�������ŏ��ɔԑg�񋟐ݒ�A�{�ґO�̃G���h�J�[�h�������� ---
	int prm_c_in1     = 0;
	int prm_c_chklast = 1;
	if (enable_in1 > 0){
		prm_c_in1     = (tmp_c_p >= 4)? tmp_c_p - 3 : 0;
		prm_c_chklast = 0;
	}
	//--- ���������b�� ---
	int prm_c_wmin = default_c_wmin;		// �W���ݒ�
	if (tmp_c_w == 2 || tmp_c_w == 4 || tmp_c_w == 5){
		prm_c_wmin = prm_v_period;
	}
	else if (tmp_c_w == 6 && enable_w15){	// 15�b����
		prm_c_wmin = 15;
	}
	//--- ��������b�� ---
	int prm_c_wmax = default_c_wmax;		// �W���ݒ�
	if (tmp_c_w == 2 || tmp_c_w == 3){
		prm_c_wmax = prm_v_period;
	}
	else if (tmp_c_w == 5){
		prm_c_wmax = prm_v_maxprd;
	}
	else if (tmp_c_w == 6 && enable_w15){	// 15�b����
		prm_c_wmax = 15;
	}

	setVal(PARAM_AUTO_c_w15     , prm_c_w15 );
	setVal(PARAM_AUTO_c_in1     , prm_c_in1 );
	setVal(PARAM_AUTO_c_chklast , prm_c_chklast );
	setVal(PARAM_AUTO_c_wmin    , prm_c_wmin );
	setVal(PARAM_AUTO_c_wmax    , prm_c_wmax );
}

void JlsAutoArg::setParamEdge(JlsCmdArg &cmdarg){
	//--- �p�����[�^�擾 ---
	int autop_code   = cmdarg.getOpt(JLOPT_DATA_AutopCode);
	int tmp_scope    = cmdarg.getOpt(JLOPT_DATA_AutopScope);
	int tmp_period   = cmdarg.getOpt(JLOPT_DATA_AutopPeriod);
	int tmp_maxprd   = cmdarg.getOpt(JLOPT_DATA_AutopMaxPrd);
	int tmp_c_w      = autop_code % 10;					// �����b������p���Ԓl
	int tmp_c_sea    = (autop_code / 10) % 10;			// �����͈�
	int prm_c_cmpart = ((((autop_code / 100) % 10) & 0x1) == 1)? 1 : 0;
	int prm_c_add    = ((((autop_code / 100) % 10) & 0x2) != 0)? 1 : 0;
	int prm_c_allcom = ((((autop_code / 1000) % 10) & 0x1) == 1)? 1 : 0;
	int prm_c_noedge = ((((autop_code / 1000) % 10) & 0x2) != 0)? 0 : 1;
	//--- ���s���f ---
	int prm_c_exe = (tmp_c_w != 0)? 1 : 0;
	//--- �f�t�H���g�l�t���ݒ� ---
	int prm_v_scope  = (tmp_scope  == 0)? 90 : tmp_scope;
	int prm_v_period = (tmp_period == 0 && cmdarg.isSetOpt(JLOPT_DATA_AutopPeriod) == false)?  5  : tmp_period;
	int prm_v_maxprd = (tmp_maxprd == 0)?  10 : tmp_maxprd;
	int prm_c_search = (tmp_c_sea  == 0)?  1  : tmp_c_sea;
	//--- ���������b�� ---
	int prm_c_wmin = 3;
	if (tmp_c_w == 2 || tmp_c_w == 4 || tmp_c_w == 5){
		prm_c_wmin = prm_v_period;
	}
	//--- ��������b�� ---
	int prm_c_wmax = 10;
	if (tmp_c_w == 2 || tmp_c_w == 3){
		prm_c_wmax = prm_v_period;
	}
	else if (tmp_c_w == 5){
		prm_c_wmax = prm_v_maxprd;
	}

	setVal(PARAM_AUTO_c_cmpart , prm_c_cmpart );
	setVal(PARAM_AUTO_c_add    , prm_c_add    );
	setVal(PARAM_AUTO_c_allcom , prm_c_allcom );
	setVal(PARAM_AUTO_c_noedge , prm_c_noedge );
	setVal(PARAM_AUTO_c_exe    , prm_c_exe    );
	setVal(PARAM_AUTO_v_scope  , prm_v_scope  );
	setVal(PARAM_AUTO_v_period , prm_v_period );
	setVal(PARAM_AUTO_v_maxprd , prm_v_maxprd );
	setVal(PARAM_AUTO_c_search , prm_c_search );
	setVal(PARAM_AUTO_c_wmin   , prm_c_wmin   );
	setVal(PARAM_AUTO_c_wmax   , prm_c_wmax   );
}

void JlsAutoArg::setParamInsDel(JlsCmdArg &cmdarg){
	//--- �p�����[�^�擾 ---
	int autop_code   = cmdarg.getOpt(JLOPT_DATA_AutopCode);
	int tmp_c_w      = autop_code % 10;					// ���Ԓl
	//--- ���s���f ---
	int prm_c_exe      = (tmp_c_w != 0)? 1 : 0;
	int prm_c_restruct = (tmp_c_w == 2)? 1 : 0;
	int prm_c_noedge   = 1;

	setVal(PARAM_AUTO_c_exe      , prm_c_exe      );
	setVal(PARAM_AUTO_c_restruct , prm_c_restruct );
	setVal(PARAM_AUTO_c_noedge   , prm_c_noedge   );
}



///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�gAuto�n���s�N���X
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// �����ݒ�
//---------------------------------------------------------------------
JlsAutoScript::JlsAutoScript(JlsDataset *pdata){
	//--- �֐��|�C���^�쐬 ---
	this->pdata = pdata;
}

JlsAutoScript::~JlsAutoScript() = default;


//---------------------------------------------------------------------
// Auto�n�R�}���h���s
//---------------------------------------------------------------------
bool JlsAutoScript::startCmd(JlsCmdSet &cmdset, bool setup_only){
	checkFirstAct(cmdset.arg);
	if (setup_only == false){
		return exeCmdMain(cmdset);
	}
	return true;
}



//=====================================================================
// Auto�R�}���h���ʏ���
//=====================================================================

//---------------------------------------------------------------------
// Auto�n�R�}���h���񓮍�ݒ�
//---------------------------------------------------------------------
void JlsAutoScript::checkFirstAct(JlsCmdArg &cmdarg){
	//--- ����̂ݎ��s ---
	if ( pdata->isAutoModeInitial() ){
		//--- �����\�����쐬 ---
		pdata->setFlagAutoMode(true);				// Auto�n��L��
		JlsAutoReform func_reform(pdata);
		func_reform.mkReformAll(cmdarg);			// ��{�\������
	}
}

//---------------------------------------------------------------------
// �eAuto�n�R�}���h���s
//---------------------------------------------------------------------
bool JlsAutoScript::exeCmdMain(JlsCmdSet &cmdset){
	//--- �p�����[�^�ƃR�}���h��ނ��擾 ---
	JlcmdAutoType cmdtype = exeCmdParam(cmdset.arg);

	//--- �͈͐ݒ� ---
	RangeMsec autoscope = cmdset.limit.getFrameRange();
	if (autoscope.st < 0){
		autoscope.st = cmdset.limit.getHead();
	}
	if (autoscope.ed < 0){
		autoscope.ed = cmdset.limit.getTail();
	}

	bool exeflag = false;
	switch(cmdtype){
		case JLCMD_AUTO_CUTTR :
			exeflag = startAutoCutTR(autoscope);
			break;
		case JLCMD_AUTO_CUTEC :
			exeflag = startAutoCutEC(autoscope);
			break;
		case JLCMD_AUTO_ADDTR :
			exeflag = startAutoAddTR(autoscope);
			break;
		case JLCMD_AUTO_ADDSP :
			exeflag = startAutoAddSP(autoscope);
			break;
		case JLCMD_AUTO_ADDEC :
			exeflag = startAutoAddEC(autoscope);
			break;
		case JLCMD_AUTO_EDGE :
			exeflag = startAutoEdge(cmdset.limit);
			break;
		case JLCMD_AUTO_ATCM :
			{
				JlsAutoReform func_reform(pdata);
				exeflag = func_reform.startAutoCM(cmdset.arg);
			}
			break;
		case JLCMD_AUTO_ATUP :
			exeflag = startAutoUp();
			break;
		case JLCMD_AUTO_ATBORDER :
			exeflag = startAutoBorder(autoscope);
			break;
		case JLCMD_AUTO_INS :
			exeflag = startAutoIns(cmdset.limit);
			break;
		case JLCMD_AUTO_DEL :
			exeflag = startAutoDel(cmdset.limit);
			break;
		case JLCMD_AUTO_ATCHG :
			exeflag = startAutoChg(cmdset.limit);
			break;
		default :
			cerr << "error:internal setting (AutoType: " << cmdtype << ")"  << endl;
			break;
	}
	return exeflag;
}

//---------------------------------------------------------------------
// Auto�n�R�}���h�p�̃p�����[�^�擾
//---------------------------------------------------------------------
jlscmd::JlcmdAutoType JlsAutoScript::exeCmdParam(JlsCmdArg &cmdarg){
	//--- �R�}���h�����ނ�I�� ---
	JlcmdAutoType cmdtype = JLCMD_AUTO_None;
	switch(cmdarg.cmdsel){
		case JLCMD_SEL_AutoCut :
			if (cmdarg.selectAutoSub == JLCMD_SUB_TR){
				cmdtype = JLCMD_AUTO_CUTTR;
			}
			else if (cmdarg.selectAutoSub == JLCMD_SUB_EC){
				cmdtype = JLCMD_AUTO_CUTEC;
			}
			else{
				cerr << "error: AutoCut is need TR/EC" << endl;
			}
			break;
		case JLCMD_SEL_AutoAdd :
			if (cmdarg.selectAutoSub == JLCMD_SUB_TR){
				cmdtype = JLCMD_AUTO_ADDTR;
			}
			else if (cmdarg.selectAutoSub == JLCMD_SUB_EC){
				cmdtype = JLCMD_AUTO_ADDEC;
			}
			else if (cmdarg.selectAutoSub == JLCMD_SUB_SP){
				cmdtype = JLCMD_AUTO_ADDSP;
			}
			else{
				cerr << "error: AutoAdd is need TR/EC/SP" << endl;
			}
			break;
		case JLCMD_SEL_AutoEdge :
			cmdtype = JLCMD_AUTO_EDGE;
			break;
		case JLCMD_SEL_AutoCM :
			cmdtype = JLCMD_AUTO_ATCM;
			break;
		case JLCMD_SEL_AutoBorder :
			cmdtype = JLCMD_AUTO_ATBORDER;
			break;
		case JLCMD_SEL_AutoUp :
			cmdtype = JLCMD_AUTO_ATUP;
			break;
		case JLCMD_SEL_AutoIns :
			cmdtype = JLCMD_AUTO_INS;
			break;
		case JLCMD_SEL_AutoDel :
			cmdtype = JLCMD_AUTO_DEL;
			break;
		default :
			if (cmdarg.getOpt(JLOPT_DATA_FlagAutoChg) > 0){
				cmdtype = JLCMD_AUTO_ATCHG;
			}
			else{
				cerr << "error:internal setting at autoCmd(Command: " << cmdtype << ")"  << endl;
			}
			break;
	}

	//--- �p�����[�^�i�[ ---
	if (cmdtype != JLCMD_AUTO_None){
		if (cmdarg.cmdsel != JLCMD_SEL_AutoCM){	// AutoCM�͕ʓr���s���ɕʃN���X�Őݒ�
			m_autoArg.setParam(cmdarg, cmdtype);
		}
	}

	return cmdtype;
}



//=====================================================================
// �eAuto�n�R�}���h
//=====================================================================

//---------------------------------------------------------------------
// AutoUp���s�J�n
// �o�́F
//  �Ԃ�l  : �ʒu�X�V���s 0=�����s 1=���s
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoUp(){
	//--- ���s�L���m�F ---
	int prm_c_exe = getAutoParam(PARAM_AUTO_c_exe);
	if (prm_c_exe == 0){
		return false;
	}
	//--- �R�}���h���s ---
	int restruct = getAutoParam(PARAM_AUTO_c_restruct);
	Nlg nlg_fall = 0;
	Nsc nsc_lastfall = 0;
	bool cont_bk = true;
	//--- �\���č\�z�p ---
	JlsAutoReform func_reform(pdata);
	do{
		Msec msec_rise, msec_fall;
		bool cont_next;
		int num_scpos = pdata->sizeDataScp();
		//--- ���S�ʒu���擾���Ď��̃��S�ʒu�Ɉړ� ---
		nlg_fall = pdata->getResultLogoNext(msec_rise, msec_fall, cont_next, nlg_fall);
		//--- ���S���Ԃ�����ꍇ ---
		if (nlg_fall >= 0){
			//--- ���S�ԍ��擾�B�Ȃ����ɑ}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX���� ---
			Nsc nsc_rise = pdata->getNscForceMsec(msec_rise, LOGO_EDGE_RISE);
			Nsc nsc_fall = pdata->getNscForceMsec(msec_fall, LOGO_EDGE_FALL);
			//--- �O����������Ǝ������オ��ʒu�̊Ԃ�CM�� ---
			if (nsc_lastfall < nsc_rise){
				func_reform.mkReformTarget(nsc_lastfall, nsc_rise, 0, restruct);	// ���S��
			}
			//--- �O����������Ǝ������オ�肪����ʒu�ŘA�������łȂ��ꍇ�͐؂�ڐݒ� ---
			else if (nsc_lastfall == nsc_rise && cont_bk == false && nsc_lastfall > 0){
				pdata->setScpChap(nsc_lastfall, SCP_CHAP_DUNIT);
			}
			//--- ���S���Ԃ����S�L�ɐݒ� ---
			if (nsc_rise < nsc_fall){
				func_reform.mkReformTarget(nsc_rise, nsc_fall, 1, restruct);		// ���S�L
			}
			nsc_lastfall = nsc_fall;
			cont_bk = cont_next;			// �����S�Ƃ̊Ԃ����S���؂�ڂȂ��̘A����
		}
		//--- �Ō�̃��S�I������ŏI�ʒu�܂ł̐ݒ� ---
		else if (nsc_lastfall >= 0 && nsc_lastfall < num_scpos-1){
			func_reform.mkReformTarget(nsc_lastfall, num_scpos-1, 0, restruct);		// ���S��
		}
	}while(nlg_fall >= 0);
	return true;
}

//---------------------------------------------------------------------
// AutoBorder���s�J�n�iBorder�ƂȂ��Ă���\����ύX�j
// �o�́F
//  �Ԃ�l  : �ʒu�X�V���s 0=�����s 1=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoBorder(RangeMsec autoscope){
	//--- ���s�L���m�F ---
	int prm_c_exe = getAutoParam(PARAM_AUTO_c_exe);
	if (prm_c_exe == 0){
		return false;
	}
	//--- �R�}���h���s ---
	bool exeflag = false;
	{
		Term term = {};
		bool cont = true;
		while( cont ){
			cont = getTermNext(term);
			if (cont && autoscope.st <= term.msec.st && term.msec.ed <= autoscope.ed){
				ScpArType arstat_term = getScpArstat(term);
				if ( jlsd::isScpArTypeBorder(arstat_term) ){
					int restruct = getAutoParam(PARAM_AUTO_c_restruct);
					if (restruct){
						if (arstat_term == SCP_AR_B_UNIT) arstat_term = SCP_AR_L_UNIT;
						else arstat_term = SCP_AR_L_OTHER;
					}
					else{
						if (arstat_term == SCP_AR_B_UNIT) arstat_term = SCP_AR_N_UNIT;
						else arstat_term = SCP_AR_N_OTHER;
					}
					exeflag = true;
					setScpArstat(term, arstat_term);
				}
			}
		}
	}
	return exeflag;
}

//---------------------------------------------------------------------
// AutoChg���s�J�n�i�]���R�}���h��-autochg�I�v�V�����j
// �o�́F
//  �Ԃ�l  : �ʒu�X�V���s 0=�����s 1=���s
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoChg(JlsCmdLimit &cmdlimit){
	bool exeflag = false;
	//--- �Ώۂ̖���SC��ݒ�i�f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����j ---
	Nrf  nrf_base    = cmdlimit.getLogoBaseNrf();
	LogoEdgeType edgelogo = jlsd::edgeFromNrf(nrf_base);
	Nsc  nsc_target  = cmdlimit.getResultTargetSel();
	Msec msec_target = cmdlimit.getTargetRangeForce();
	if (nsc_target < 0 && msec_target >= 0){
		nsc_target = pdata->getNscForceMsec(msec_target, edgelogo);
	}
	//--- ��ʒu���擾 ---
	Nsc  nsc_base = getNscElgFromNrf(nrf_base);
	//--- �L���ʒu�ł���Ύ��s ---
	if (nsc_target != nsc_base && nsc_base >= 0 && nsc_target >= 0){
		exeflag = true;
		JlsAutoReform func_reform(pdata);
		if (edgelogo == LOGO_EDGE_RISE){
			if (nsc_base < nsc_target){
				func_reform.mkReformTarget(nsc_base, nsc_target, 0, 1);		// ���S��
			}
			else{
				func_reform.mkReformTarget(nsc_target, nsc_base, 1, 1);		// ���S�L
			}
		}
		if (edgelogo == LOGO_EDGE_FALL){
			if (nsc_base < nsc_target){
				func_reform.mkReformTarget(nsc_base, nsc_target, 1, 1);		// ���S�L
			}
			else{
				func_reform.mkReformTarget(nsc_target, nsc_base, 0, 1);		// ���S��
			}
		}
	}
	return exeflag;
}

//---------------------------------------------------------------------
// �����S�G�b�W�ɑΉ����郍�S�����\���ʒu���擾
// �o�́F
//  �Ԃ�l  : ���S�����\���G�b�W�̃V�[���`�F���W�ԍ�
//---------------------------------------------------------------------
Nsc JlsAutoScript::getNscElgFromNrf(Nrf nrf_base){
	LogoEdgeType edgelogo = jlsd::edgeFromNrf(nrf_base);
	int msec_base_nrf = pdata->getMsecLogoNrf(nrf_base);

	//--- �\����̑Ή����郍�S�[���擾 ---
	Nsc nsc_base_elg = -1;
	if (nrf_base >= 0){
		//--- ���S�ɑΉ�����\����؂�ʒu�擾 ---
		ElgCurrent elg = {};
		bool flag_cont = true;
		while( getElgNextKeep(elg) && flag_cont){
			if (edgelogo == LOGO_EDGE_RISE){
				if ((elg.msecLastFall < msec_base_nrf || elg.msecLastFall <= 0) && msec_base_nrf < elg.msecFall){
					nsc_base_elg = elg.nscRise;
					flag_cont    = false;
				}
			}
			if (edgelogo == LOGO_EDGE_FALL){
				if (elg.msecLastRise < msec_base_nrf && msec_base_nrf < elg.msecRise){
					nsc_base_elg = elg.nscLastFall;
					flag_cont    = false;
				}
			}
		}
		//--- �Ō�̃��S���������茟�o ---
		if (flag_cont == true && edgelogo == LOGO_EDGE_FALL){
			if (elg.msecLastRise <= msec_base_nrf){
				nsc_base_elg = elg.nscLastFall;
			}
		}
	}
	//--- �L���Ȉʒu�֌W���m�F ---
	if (nsc_base_elg >= 0){
		Msec msec_base_elg = pdata->getMsecScpEdge(nsc_base_elg, edgelogo);
		//--- ��ʒu�ƃ��S�����ʒu������Ă����疳�������鏈�� ---
		if (abs(msec_base_nrf - msec_base_elg) > 31*1000){		// 31�b�ȏ�
			nsc_base_elg = -1;
		}
	}
	return nsc_base_elg;
}


//---------------------------------------------------------------------
// AutoIns���s�J�n
// �o�́F
//  �Ԃ�l  : �ʒu�X�V���s 0=�����s 1=���s
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoIns(JlsCmdLimit &cmdlimit){
	bool exeflag = false;
	//--- ���s�L���m�F ---
	int prm_c_exe = getAutoParam(PARAM_AUTO_c_exe);
	if (prm_c_exe == 0){
		return false;
	}
	//--- �R�}���h���s ---
	Nsc nsc_target;
	Nsc nsc_base;
	int restruct = getAutoParam(PARAM_AUTO_c_restruct);
	if ( subInsDelGetRange(nsc_target, nsc_base, cmdlimit) ){
		JlsAutoReform func_reform(pdata);
		func_reform.mkReformTarget(nsc_target, nsc_base, 1, restruct);		// ���S�L
		exeflag = true;
	}
	return exeflag;
}

//---------------------------------------------------------------------
// AutoDel���s�J�n
// �o�́F
//  �Ԃ�l  : �ʒu�X�V���s 0=�����s 1=���s
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoDel(JlsCmdLimit &cmdlimit){
	bool exeflag = false;
	//--- ���s�L���m�F ---
	int prm_c_exe = getAutoParam(PARAM_AUTO_c_exe);
	if (prm_c_exe == 0){
		return false;
	}
	//--- �R�}���h���s ---
	Nsc nsc_target;
	Nsc nsc_base;
	int restruct = getAutoParam(PARAM_AUTO_c_restruct);
	if ( subInsDelGetRange(nsc_target, nsc_base, cmdlimit) ){
		JlsAutoReform func_reform(pdata);
		func_reform.mkReformTarget(nsc_target, nsc_base, 0, restruct);		// ���S��
		exeflag = true;
	}
	return exeflag;
}

//---------------------------------------------------------------------
// AutoIns / AutoDel�p �͈͏��擾
// �o�́F
//  �Ԃ�l  : ��ʒu�̑��ݗL��
//  nsc_target : �^�[�Q�b�g�V�[���`�F���W�ԍ�
//  nsc_base   : ��V�[���`�F���W�ԍ�
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
bool JlsAutoScript::subInsDelGetRange(Nsc &nsc_target, Nsc &nsc_base, JlsCmdLimit &cmdlimit){
	LogoEdgeType edge = cmdlimit.getLogoBaseEdge();
	//--- ��ʒu�擾 ---
	nsc_base = subInsDelGetBase(cmdlimit);
	Msec msec_base = pdata->getMsecScpEdge(nsc_base, edge);
	//--- �^�[�Q�b�g�ʒu�擾 ---
	nsc_target  = cmdlimit.getResultTargetSel();
	Msec msec_target = cmdlimit.getTargetRangeForce();
	if (nsc_target < 0 && msec_target >= 0){
		nsc_target = pdata->getNscForceMsec(msec_target, edge);
	}
	//--- ��ʒu�̍X�V�i�^�[�Q�b�g�ʒu���V�K�ǉ��̎��ς�邽�߁j ---
	if (nsc_base >= 0){
		nsc_base = pdata->getNscFromMsecAllEdgein(msec_base);
	}
	if (nsc_target < 0 || nsc_base < 0){
		return false;
	}
	return true;
}


//---------------------------------------------------------------------
// AutoIns / AutoDel�p ��ʒu���擾
// �o�́F
//  �Ԃ�l  : ��ʒu�̃V�[���`�F���W�ԍ�
//---------------------------------------------------------------------
Nsc JlsAutoScript::subInsDelGetBase(JlsCmdLimit &cmdlimit){
	Nsc nsc_base = -1;
	Nsc limit_nscend  = cmdlimit.getResultTargetEnd();
	Nsc limit_nscbase = cmdlimit.getLogoBaseNsc();
	Nrf limit_nrfbase = cmdlimit.getLogoBaseNrf();

	if (limit_nscend >= 0){
		nsc_base = limit_nscend;
	}
	else if (limit_nscbase >= 0){
		nsc_base = limit_nscbase;
	}
	else if (limit_nrfbase >= 0){
		nsc_base = pdata->getNscFromNrf(limit_nrfbase, pdata->msecValLap2, SCP_CHAP_DECIDE, false);
//		nsc_base = getNscElgFromNrf(limit_nrfbase);
	}
	return nsc_base;
}



//---------------------------------------------------------------------
// AutoCut TR �\�����c���Ԑ���J�b�g���鏈��
// �o�́F
//  �Ԃ�l  : �J�b�g�������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoCutTR(RangeMsec autoscope){
	//--- ������Ԑݒ� ---
	Msec msec_spc = pdata->msecValSpc;
	//--- �\���ݒ�ʒu�iCM�\�����������������ōs���ʒu�j ---
	pdata->recHold.msecTrPoint = autoscope.st;
	//--- ���s�L���m�F ---
	int prm_c_exe = getAutoParam(PARAM_AUTO_c_exe);
	if (prm_c_exe == 0){
		return false;
	}

	//--- �\���J�n�ʒu���擾 ---
	Nsc nsc_trstart = subCutTRGetLocSt(autoscope);
	if (nsc_trstart < 0){	// �\����₪������Ȃ��ꍇ
		//--- �\�����o�Ȃ��̌��o ---
		return false;
	}

	//--- �p�����[�^�擾 ---
	int prm_msec_wlogo_trmax = getConfig(CONFIG_VAR_msecWLogoTRMax);
	int prm_msec_wcomp_trmax = getConfig(CONFIG_VAR_msecWCompTRMax);
	int prm_limit     = getAutoParam(PARAM_AUTO_v_limit);
	int prm_trsumprd  = getAutoParam(PARAM_AUTO_v_trsumprd);
	int prm_c_cutst   = getAutoParam(PARAM_AUTO_c_cutst);

	//--- �\���ʒu��ݒ� ---
	bool ret = false;
	{
		int state_cut = 0;
		int ncut_rest = prm_limit;
		Nlg nlg = 0;
		Nlg nlg_start = 0;
		Sec sec_tr_total = 0;
		ElgCurrent elg = {};
		bool det_logo = getElgNextKeep(elg);
		Term term = {};
		term.ini = nsc_trstart;
		bool cont = getTermNext(term);
		while(cont){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			bool flag_scope = checkScopeTerm(term, autoscope);
			if (flag_scope &&									// �͈͓�
				(jlsd::isScpArTypeLogoBorder(arstat_term) ||	// ���S���蕔��
				 term.nsc.st == nsc_trstart) &&					// �\���擪�ʒu
				((arext_term != SCP_AREXT_L_LGCUT) &&			// �G�b�W�����ςݕ����͏���
				 (arext_term != SCP_AREXT_L_LGADD))){

				//--- �V�[���`�F���W�ɑΉ����郍�S�ʒu�擾 ---
				while(term.nsc.ed > elg.nscFall && det_logo){
					nlg ++;
					det_logo = getElgNextKeep(elg);
				}
				//--- �O�t���[������̊��� ---
				Msec msec_dif_term = term.msec.ed - term.msec.st;
				Sec sec_dif_term = pdata->cnv.getSecFromMsec( msec_dif_term );
				//--- ����`�F�b�N ---
				if (state_cut == 0){
					nlg_start = nlg;
					state_cut = 1;
				}
				//--- �J�b�g�J�n�������f ---
				bool flag_cut1st = false;
				if (state_cut == 1 && ncut_rest == 0 && sec_tr_total >= prm_trsumprd && prm_c_cutst != 3){
					if (((sec_dif_term % 15 == 0) && prm_c_cutst == 0) ||
						((sec_dif_term >= 15) && prm_c_cutst == 1) ||
						(prm_c_cutst == 2)){
						state_cut = 2;
						flag_cut1st = true;
					}
				}
				//--- �J�b�g�J�n��̏��� ---
				if (state_cut == 2){
					if ((elg.msecFall - elg.msecRise <= prm_msec_wlogo_trmax + msec_spc ||
						 nlg == nlg_start) &&											// ���S���Q���ȓ����J�n���S
						elg.msecFall - term.msec.ed <= prm_msec_wlogo_trmax + msec_spc &&	// ���S�I���܂łQ���ȓ�
						msec_dif_term <= prm_msec_wcomp_trmax + msec_spc &&				// �\����60�b�ȓ�
						elg.msecFall <= autoscope.ed + msec_spc){						// ���S�I���n�_���͈͓�
						if (jlsd::isScpArTypeBorder(arstat_term) && flag_cut1st){		// �J�b�g�J�n�����Border
							setScpArext(term, SCP_AREXT_L_TRRAW);
						}
						else{
							subCutTRSetCut(term);
						}
						ret = true;
					}
					else{
						state_cut = 3;					// ���S�I��
					}
				}
				//--- �J�b�g�J�n�܂Ŏc������ ---
				if (state_cut == 1){
					if (ncut_rest > 0){
						setScpArext(term, SCP_AREXT_L_TRKEEP);
						ncut_rest --;
						sec_tr_total += sec_dif_term;
					}
					else{								// �G���h�J�[�h���f�҂�
						setScpArext(term, SCP_AREXT_L_TRRAW);
					}
				}
			}
			else if (flag_scope && (arstat_term == SCP_AR_N_OTHER)){	// ���S�Ȃ��s���\��
				if (state_cut == 1 || state_cut == 2){		// �J�b�g���Ԓ�
					setScpArext(term, SCP_AREXT_N_TRCUT);
				}
			}
			//--- ���̈ʒu��ݒ� ---
			cont = getTermNext(term);
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �\���J�n�ʒu���擾
// �o�́F
//  �Ԃ�l  : �\���J�n�ʒu�ƂȂ�V�[���`�F���W�ԍ��i-1�̎��Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsAutoScript::subCutTRGetLocSt(RangeMsec autoscope){
	Nsc  nsc_cand = -1;
	bool flag_cand  = false;
	bool det_logo   = true;
	ElgCurrent elg = {};
	while(det_logo){
		det_logo = getElgNextKeep(elg);
		if (det_logo){
			subCutTRGetLocStSub(&nsc_cand, &flag_cand, autoscope, elg);
		}
	};
	return nsc_cand;
}

//---------------------------------------------------------------------
// �\���J�n�ʒu���擾�i�P���S���ԓ��̔��f�j
// �o�́F
//   *r_nsc_cand   : ���ʒu�ԍ�
//   *r_flag_cand  : �����(false:���Ȃ�  true:��₠��)
//---------------------------------------------------------------------
void JlsAutoScript::subCutTRGetLocStSub(Nsc *r_nsc_cand, bool *r_flag_cand, RangeMsec autoscope, ElgCurrent elg){
	//--- �ݒ�l ---
	Msec msec_spc = pdata->msecValSpc;
	int prm_wcomp_spmin      = getConfig(CONFIG_VAR_secWCompSPMin);
	int prm_wcomp_spmax      = getConfig(CONFIG_VAR_secWCompSPMax);
	int prm_msec_wlogo_trmax = getConfig(CONFIG_VAR_msecWLogoTRMax);
	int prm_msec_wcomp_trmax = getConfig(CONFIG_VAR_msecWCompTRMax);

	//--- �g�p�p�����[�^ ---
	int prm_c_from   = getAutoParam(PARAM_AUTO_c_from);
	int prm_c_lgpre  = getAutoParam(PARAM_AUTO_c_lgpre);
	int prm_trscope  = getAutoParam(PARAM_AUTO_v_trscope);
	int prm_tr1stprd = getAutoParam(PARAM_AUTO_v_tr1stprd);

	//--- ���S�I�������ԓ� ---
	if (elg.msecFall <= autoscope.st + msec_spc || elg.msecFall >= autoscope.ed + msec_spc){
		return;
	}

	//--- �O���Ԃ���荞�� ---
	Nsc nsc_cand = *r_nsc_cand;
	bool flag_cand  = *r_flag_cand;

	//--- ���S���Ԃ��\���p�̒������m�F ---
	bool longlogo = false;
	{
		if (elg.msecRise < autoscope.st - msec_spc){
			longlogo = true;
		}
		else if (elg.msecFall - elg.msecRise > prm_msec_wlogo_trmax + msec_spc){
			longlogo = true;
		}
	}
	//--- ���S�ʒu�ݒ�i���S�G�b�W�������Ă��鏊�͏����j ---
	RangeNsc rnsc_logo = {elg.nscRise, elg.nscFall};
	{
		Term term = {};
		term.ini = rnsc_logo.ed;					// �J�n�ʒu
		bool cont = getTermPrev(term);
		while(cont){
			if (term.nsc.st < rnsc_logo.st){
				cont = false;
			}
			else{
				ScpArExtType arext_term = getScpArext(term);
				if ((arext_term == SCP_AREXT_L_LGCUT) ||
					(arext_term == SCP_AREXT_L_LGADD)){
					rnsc_logo.ed = term.nsc.st;		// �G�b�W�������Ă��鏊�͏���
				}
				else{
					cont = false;
				}
			}
			if (cont){
				cont = getTermPrev(term);
			}
		}
	}
	Msec msec_dst_fall = pdata->getMsecScp(rnsc_logo.ed);

	//--- ���S�J�n����ɂ����I��邩�`�F�b�N ---
	bool shortlogo = false;
	{
		Term term = {};
		term.ini = rnsc_logo.st;					// �J�n�ʒu
		bool cont = getTermNext(term);
		if (term.nsc.ed != rnsc_logo.ed || !cont){
		}
		else if (pdata->cnv.getSecFromMsec(msec_dst_fall - elg.msecRise) < 15){
			ElgCurrent next_elg = elg;
			bool valid_next = getElgNextKeep(next_elg);
			//--- ���̃��S���Ȃ����I���n�_������܂ő����ꍇ�A�P�̒Z���ԃt���O ---
			if (valid_next == false || next_elg.msecFall >= autoscope.ed - msec_spc){
				shortlogo = true;
			}
		}
	}
	//--- ���S��O�ł��ʒu�m�肪�܂��ŗ\����₪����΃`�F�b�N ---
	if (rnsc_logo.st < rnsc_logo.ed && flag_cand == false && (prm_c_lgpre == 1 || prm_c_lgpre == 2)){
		//--- �P�O�̍\�����擾 ---
		Term term1 = {};
		term1.ini = rnsc_logo.st;					// �J�n�ʒu
		bool cont1 = getTermPrev(term1);
		//--- ���S��O��CM15�b�P�ʌ��o�ł͂Ȃ��� ---
		if (cont1 && isScpArstatCmUnit(term1) == false){
			//--- �Q�O�̍\�����擾 ---
			Term term2 = term1;
			bool cont2 = getTermPrev(term2);
			if (term2.nsc.st <= elg.nscLastFall){
				cont2 = false;
			}
			//--- �R�O�̍\�����擾 ---
			Term term3 = term2;
			bool cont3 = getTermPrev(term2);
			if (term3.nsc.st <= elg.nscLastFall || !cont2){
				cont3 = false;
			}
			//--- ���ʂ̍\�����Ԑ������m�F ---
			bool flag_common_cond = false;
			if (longlogo == false && (prm_c_lgpre == 2 || shortlogo == true)){
				flag_common_cond = true;
			}
			//--- �Q�O��CM15�b�P�ʂ������ꍇ ---
			if (cont2 && isScpArstatCmUnit(term2) == true){
				// �P�O�̍\�����ԑg�񋟂̍\���O�ł���Η\���ɒǉ�
				Sec sec_dif_term1 = pdata->cnv.getSecFromMsec(term1.msec.ed - term1.msec.st);
				if ((sec_dif_term1 < prm_wcomp_spmin || sec_dif_term1 > prm_wcomp_spmax) &&
					flag_common_cond &&
					(term1.msec.st >= autoscope.st) &&
					(sec_dif_term1 <= prm_tr1stprd || prm_tr1stprd == 0)){
					nsc_cand = term1.nsc.st;
					flag_cand = true;
				}
			}
			// �Q�O��CM15�b�P�ʌ��o�ł͂Ȃ��A�R�O��CM15�b�P�ʂ������ꍇ
			else if (cont3 && isScpArstatCmUnit(term3) == true){
				// �P�O�̍\�����ԑg�񋟂̍\���O�ł���Η\���ɒǉ�
				Sec sec_dif_term2 = pdata->cnv.getSecFromMsec(term2.msec.ed - term2.msec.st);
				if ((sec_dif_term2 < prm_wcomp_spmin || sec_dif_term2 > prm_wcomp_spmax) &&
					flag_common_cond &&
					(term1.msec.st >= autoscope.st) &&
					(sec_dif_term2 <= prm_tr1stprd || prm_tr1stprd == 0)){
					nsc_cand = term2.nsc.st;
					flag_cand = true;
				}
			}
		}
	}
	//--- �������ʂ�CM�����̃��S������ΗD�悳����ꍇ�̂��� ---
	if (rnsc_logo.st < rnsc_logo.ed && flag_cand == false && nsc_cand >= 0){
		if (nsc_cand >= 0){
			//--- �Z���P��\���ȊO�̃��S�͐V���ɂ��邽�߉ߋ������N���A ---
			Term term = {};
			term.ini = rnsc_logo.st;
			if (getTermPrev(term)){
				//--- ���S��O��CM���o�ł͂Ȃ�����CM�������� ---
				if (isScpArstatCmUnit(term) == false){
					nsc_cand = -1;
				}
				else{
					if (shortlogo == false){
						nsc_cand = -1;
					}
				}
			}
		}
		if (nsc_cand >= 0){
			//--- ���S���ԏI���ʒu���玟�̍\���ʒu���擾 ---
			Term term = {};
			term.ini = rnsc_logo.ed;
			if (getTermNext(term)){
				//--- ���S���オCM���o�ł͂Ȃ�����CM�������� ---
				if (isScpArstatCmUnit(term) == false){
					nsc_cand = -1;
				}
			}
		}
	}
	//--- ���S���ԓ��̃V�[���`�F���W�m�F ---
	if (rnsc_logo.st < rnsc_logo.ed){
		Term term = {};
		term.ini = rnsc_logo.st;
		bool cont = getTermNext(term);
		while(cont){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			if (checkScopeTerm(term, autoscope) &&		// �͈͓�
				(term.nsc.st >= rnsc_logo.st && term.nsc.ed <= rnsc_logo.ed) &&		// ���S�͈͓�
				jlsd::isScpArTypeLogo(arstat_term) &&	// ���S���蕔��
				((arext_term != SCP_AREXT_L_LGCUT) &&	// �G�b�W�����ςݕ����͏���
				 (arext_term != SCP_AREXT_L_LGADD))){
				//--- �O�񂩂�̊��Ԃ��擾 ---
				Msec msec_logo_ed = pdata->getMsecScp(rnsc_logo.ed);
				Msec msec_dif_term = term.msec.ed - term.msec.st;
				Sec sec_dif_term = pdata->cnv.getSecFromMsec(msec_dif_term);
				Sec sec_dif_lged = pdata->cnv.getSecFromMsec(msec_logo_ed - term.msec.ed);
				//--- �\�����\�����Ԉȏ゠������{�҂ƔF�� ---
				if (msec_dif_term > prm_msec_wcomp_trmax + msec_spc){
					longlogo = true;
					nsc_cand = -1;
				}
				//--- �ŏ��̗\���ʒu�ݒ� ---
				else if (nsc_cand < 0){
					// �\�����Ԃ�����ȉ��Ȃ�ݒ�
					if (sec_dif_term <= prm_tr1stprd || prm_tr1stprd == 0){
						nsc_cand = term.nsc.st;
					}
				}
				//--- �{�Ҍ�̓����S���ʒu�������ꍇ�����𖞂������m�F ---
				if (longlogo){
					//--- CM�������琔����ꍇ�͖{�҃��S���͖����� ---
					if (prm_c_from == 1){
						 nsc_cand = -1;
					}
					else if ((((sec_dif_term % 15) != 0) && prm_c_from == 5) ||
							 (((sec_dif_term % 5)  != 0) && prm_c_from == 4) ||
							 (sec_dif_lged > prm_trscope)){
							 nsc_cand = -1;
					}
				}
			}
			//--- �m�肷�郍�S���Ԃ̏ꍇ�̓t���O�ݒ� ---
			if (longlogo && prm_c_from == 2){
				flag_cand = false;
			}
			else{
				if (nsc_cand >= 0){
					flag_cand = true;
				}
			}
			//--- ���̈ʒu��ݒ� ---
			cont = getTermNext(term);
		}
	}

	*r_nsc_cand = nsc_cand;
	*r_flag_cand  =flag_cand;
}

//---------------------------------------------------------------------
// �\����̕s�v�\���J�b�g����
//---------------------------------------------------------------------
void JlsAutoScript::subCutTRSetCut(Term &term){
	//--- ���Ԃɂ��J�b�g��Ԑݒ� ---
	Msec msec_dif_term = term.msec.ed - term.msec.st;
	Sec sec_dif_term = pdata->cnv.getSecFromMsec( msec_dif_term );
	if (sec_dif_term >= 14){
		setScpArext(term, SCP_AREXT_L_TRCUT);
	}
	else{
		setScpArext(term, SCP_AREXT_L_ECCUT);
	}
	//--- �ݒ�l�ǂݍ��� ---
	int prm_msec_wcomp_trmax = getConfig(CONFIG_VAR_msecWCompTRMax);
	int mgn_cm_detect = pdata->getConfig(CONFIG_VAR_msecMgnCmDetect);
	//--- �O�\����15�b�P�ʂ̌������� ---
	{
		Term preterm = term;
		int nloop = 0;
		bool cont = true;
		while(cont && nloop <= 2){
			//--- �����̓J�b�g��ԓ��m�̂݁B����ȊO�ł͏I�� ---
			ScpArExtType arext_term = getScpArext(preterm);
			if ((arext_term != SCP_AREXT_L_TRCUT) &&
				(arext_term != SCP_AREXT_L_ECCUT)){
				cont = false;
			}
			if (cont){
				//--- 15�b�P�ʂ��炸�ꂪ�Ȃ�����T�� ---
				Msec msec_dif_target = term.msec.ed - preterm.msec.st;
				Msec msec_gap = ((msec_dif_target + 7500) % 15000) - 7500;
				if (msec_dif_target > prm_msec_wcomp_trmax + mgn_cm_detect){	// 60�b�𒴂�����I��
					cont = false;
				}
				else if (msec_gap <= mgn_cm_detect){
					if (nloop > 0){
						//--- 15�b�P�ʂ̏�������Γr�������� ---
						Term tmpterm = preterm;
						for(int i=0; i<nloop; i++){
							pdata->setScpChap(tmpterm.nsc.ed, SCP_CHAP_CPOSIT);
							getTermNext(tmpterm);
						}
						//--- term��ݒ肵���� ---
						Nsc nsc_end = term.nsc.ed;
						term = {};
						term.ini = preterm.nsc.st;
						bool tmpcont = getTermNext(term);
						while(term.nsc.ed < nsc_end && tmpcont){	// �O�̂��ߊm�F
							tmpcont = getTermNext(term);
						}
						setScpArext(term, SCP_AREXT_L_TRCUT);
					}
					cont = false;
				}
			}
			if (cont){
				cont = getTermPrev(preterm);
				nloop ++;
			}
		}
	}
}



//---------------------------------------------------------------------
// AutoCut EC �G���h�J�[�h�����J�b�g����
// �o�́F
//  �Ԃ�l  : �J�b�g�������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoCutEC(RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_limit  = getAutoParam(PARAM_AUTO_v_limit);

	//--- ������Ԑݒ� ---
	bool ret = false;

	//--- �ʒu�����擾 ---
	vector<int> local_cntcut(pdata->sizeDataScp());
	int ovw_force;
	int n_cutdst = subCutECGetLocSt(local_cntcut, &ovw_force, autoscope);

	//--- �m�F�����ʒu�ŃJ�b�g���� ---
	{
		int  state_cut = 0;
		Term term = {};
		bool cont = getTermNext(term);
		while(cont){
			ScpArExtType arext_term = getScpArext(term);
			int cntcut_term = local_cntcut[term.nsc.ed];
			//--- �͈͓��Ń��S�L�̔z�u�ʒu�������ꍇ�̂ݎ��s ---
			if (checkScopeTerm(term, autoscope)){
				if (state_cut == 0){
					if (cntcut_term == n_cutdst){
						state_cut = 1;
					}
					else if (cntcut_term > 0){
						if (prm_limit > 0){
							setScpArext(term, SCP_AREXT_L_EC);
						}
					}
				}
				if (state_cut == 1){
					if (cntcut_term > 0){
						if ((arext_term != SCP_AREXT_L_EC &&
							 arext_term != SCP_AREXT_L_SP) ||
							(ovw_force > 0 && ovw_force <= cntcut_term)){
							setScpArext(term, SCP_AREXT_L_ECCUT);
							ret = true;
						}
					}
					else if (arext_term == SCP_AREXT_L_TRCUT ||
							 arext_term == SCP_AREXT_L_ECCUT){
						state_cut = 2;
						cont = false;
					}
				}
			}
			else if (term.msec.ed > autoscope.ed){
				cont = false;
			}
			//--- ���ʒu��ݒ� ---
			if (cont){
				cont = getTermNext(term);
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �J�n�ʒu�E�����擾
// �o�́F
//  �Ԃ�l  : �G���h�J�[�h�̃J�b�g�J�n�ʒu�ƂȂ�V�[���`�F���W�ԍ��i-1�̎��Y���Ȃ��j
//  local_cntcut[] : �e�V�[���`�F���W�ԍ��̃G���h�J�[�h�����P���珇�Ԃɔԍ��t��
//  r_ovw_force    : 0:�������̏㏑���Ȃ� 1-:�����̔ԑg�񋟁E�G���h�J�[�h�ݒ���㏑���J�n����ԍ�
//---------------------------------------------------------------------
int JlsAutoScript::subCutECGetLocSt(vector<int> &local_cntcut, int *r_ovw_force, RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_limit   = getAutoParam(PARAM_AUTO_v_limit);
	int prm_period  = getAutoParam(PARAM_AUTO_v_period);
	int prm_maxprd  = getAutoParam(PARAM_AUTO_v_maxprd);
	int prm_c_sel   = getAutoParam(PARAM_AUTO_c_sel);
	int prm_c_cutsp = getAutoParam(PARAM_AUTO_c_cutsp);
	int prm_c_cutla = getAutoParam(PARAM_AUTO_c_cutla);
	int prm_c_cutlp = getAutoParam(PARAM_AUTO_c_cutlp);
	int prm_c_cut30 = getAutoParam(PARAM_AUTO_c_cut30);

	//--- ������Ԑݒ� ---
	local_cntcut.assign(local_cntcut.size(), 0);
	int prmflag_cutlast = 0;
	if (prm_limit < 0 || prm_c_cutla > 0 || prm_c_cutlp > 0){
		prmflag_cutlast = 1;
	}

	//--- �擪�ʒu������ ---
	Nsc nsc_cand = -1;
	{
		bool flag_cand    = false;
		bool flag_existec = false;
		bool flag_end     = false;
		Term term = {};
		bool cont = getTermNext(term);
		while(cont && !flag_end){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			//--- �͈͓��Ń��S�L�̔z�u�ʒu�������ꍇ�̂ݎ��s ---
			if (checkScopeTerm(term, autoscope)){
				Sec sec_dif_term = pdata->cnv.getSecFromMsec(term.msec.ed - term.msec.st);
				//--- �G���h�J�[�h�Ƃ��ėL���ȏ����������� ---
				if (arext_term == SCP_AREXT_L_TRRAW){
					if (flag_cand == false){
						nsc_cand = term.nsc.st;
						flag_cand = true;
					}
				}
				else if (arext_term == SCP_AREXT_L_EC){
					if (flag_cand == false || flag_existec == false){
						nsc_cand = term.nsc.st;
						flag_cand = true;
						flag_existec = true;
					}
				}
				else if (arext_term == SCP_AREXT_L_SP){
					if (prmflag_cutlast > 0 || prm_c_cutsp > 0){	// �J�b�g�ΏۂƂ���ꍇ
						if (nsc_cand < 0){
							nsc_cand = term.nsc.st;
						}
					}
					else{
						nsc_cand = -1;
						flag_cand = false;
					}
				}
				else if (arext_term == SCP_AREXT_L_TRCUT ||
						 arext_term == SCP_AREXT_L_ECCUT){
					flag_end = true;
				}
				else if (arext_term == SCP_AREXT_L_TRKEEP){
					nsc_cand = -1;
					flag_cand = false;
					if (prm_c_cut30 > 0){
						if (sec_dif_term == 30){
							prm_limit = 0;			// �G���h�J�[�h�Ȃ�
						}
					}
				}
				else if (jlsd::isScpArTypeLogo(arstat_term) &&
						 arext_term != SCP_AREXT_L_LGCUT &&
						 arext_term != SCP_AREXT_L_LGADD){
					if (flag_cand == false){
						// �����͈͓����`�F�b�N
						if (subCutECCheckScope(sec_dif_term, prm_c_sel, prm_period, prm_maxprd)){
							if (prmflag_cutlast > 0 && nsc_cand < 0){
								nsc_cand = term.nsc.st;
							}
						}
						else{
							if (flag_cand == false && nsc_cand >= 0){
								nsc_cand = -1;
							}
						}
					}
				}
			}
			else if (term.msec.ed > autoscope.ed){
				flag_end = true;
			}
			//--- ���ʒu��ݒ� ---
			cont = getTermNext(term);
		}
	}

	//--- �e�V�[���`�F���W�ʒu���ԕt�� ---
	int n_cutmax = 0;
	Sec sec_dif_last = 0;
	if (nsc_cand >= 0){
		bool flag_end = false;
		bool flag_cont = false;
		Term term = {};
		term.ini = nsc_cand;
		bool cont = getTermNext(term);
		while(cont && flag_end == false){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			Sec sec_dif_term = pdata->cnv.getSecFromMsec(term.msec.ed - term.msec.st);
			//--- �͈͓��Ń��S�L�̔z�u�ʒu�������ꍇ�̂ݎ��s ---
			if (checkScopeTerm(term, autoscope)){
				bool flag_valid = false;
				//--- �G���h�J�[�h�Ƃ��ėL���ȏ����������� ---
				if (arext_term == SCP_AREXT_L_TRRAW){
					flag_valid = true;
				}
				else if (arext_term == SCP_AREXT_L_EC){
					flag_valid = true;
				}
				else if (arext_term == SCP_AREXT_L_SP){
					flag_cont = true;								// �p���t���O�𗧂Ă�
					if (prmflag_cutlast > 0 || prm_c_cutsp > 0){	// �J�b�g�ΏۂƂ���ꍇ
						flag_valid = true;
					}
				}
				else if (arext_term == SCP_AREXT_L_TRCUT ||	// �����J�b�g������ΏI��
						 arext_term == SCP_AREXT_L_ECCUT){
					flag_end = true;
				}
				else if (jlsd::isScpArTypeLogo(arstat_term) &&
						 arext_term != SCP_AREXT_L_LGCUT &&
						 arext_term != SCP_AREXT_L_LGADD){
					// �����͈͓����`�F�b�N
					if (subCutECCheckScope(sec_dif_term, prm_c_sel, prm_period, prm_maxprd)){
						if (prmflag_cutlast > 0 || flag_cont){
							flag_valid = true;
						}
					}
					else{
						flag_cont = false;					// �p���͏I��
					}
				}
				if (flag_valid){
					//--- �J�n�ʒu�擾 ---
					if (subCutECCheckScope(sec_dif_term, prm_c_sel, prm_period, prm_maxprd)){
						n_cutmax ++;
						local_cntcut[term.nsc.ed] = n_cutmax;
						sec_dif_last = sec_dif_term;		// �Ō�̊��Ԃ��L��
					}
				}
			}
			else if (term.msec.ed > term.msec.ed){
				flag_end = true;
			}
			//--- ���̈ʒu ---
			cont = getTermNext(term);
		}
	}
	//--- �J�b�g�ʒu�̔ԍ������� ---
	int n_cutdst;
	int ovw_force = 0;
	if (prm_limit >= 0){				// �擪���琔����ꍇ
		n_cutdst = prm_limit + 1;
		if (n_cutmax > 0 && prmflag_cutlast > 0){	// �Ō���J�b�g����ꍇ
			if (prm_c_cutla > 0){
				ovw_force = n_cutmax;
				if (n_cutdst > n_cutmax){
					n_cutdst = n_cutmax;
				}
			}
			else if (prm_c_cutlp > 0){
				if (subCutECCheckScope(sec_dif_last, 2, prm_period, prm_maxprd)){
					ovw_force = n_cutmax;
					if (n_cutdst > n_cutmax){
						n_cutdst = n_cutmax;
					}
				}
			}
		}
	}
	else{								// �Ōォ�琔����ꍇ
		n_cutdst = n_cutmax + prm_limit + 1;
		ovw_force = n_cutdst;
		if (prm_c_cutlp > 0){						// ���ԏ���������ꍇ
			if (subCutECCheckScope(sec_dif_last, 2, prm_period, prm_maxprd) == 0){
				n_cutdst = n_cutmax + 100;			// �J�b�g������
				ovw_force = 0;
			}
		}
	}
	*r_ovw_force = ovw_force;
	return n_cutdst;
}

//---------------------------------------------------------------------
// ���͕b�����G���h�J�[�h�����͈͓����`�F�b�N
// �o�́F
//  �Ԃ�l  : false=�G���h�J�[�h�����͈͊O true=�G���h�J�[�h�����͈͓�
//---------------------------------------------------------------------
bool JlsAutoScript::subCutECCheckScope(Sec sec_dif, int prm_c_sel, int prm_period, int prm_maxprd){

	if ((sec_dif < 15 && prm_c_sel == 1) ||
		(sec_dif <= prm_period && prm_c_sel == 3) ||
		(sec_dif >= prm_period && sec_dif < 15 && prm_c_sel == 4) ||
		(sec_dif >= prm_period && sec_dif < prm_maxprd && prm_c_sel == 5) ||
		(sec_dif == prm_period && prm_c_sel == 2)){
		return true;
	}
	else{
		return false;
	}
}



//---------------------------------------------------------------------
// AutoAdd SP �ԑg�񋟒ǉ�����
// �o�́F
//  �Ԃ�l  : �J�b�g�������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoAddSP(RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_c_exe      = getAutoParam(PARAM_AUTO_c_exe);
	int prm_limit      = getAutoParam(PARAM_AUTO_v_limit);
	int prm_c_limtrsum = getAutoParam(PARAM_AUTO_c_limtrsum);
	int prm_trsumprd   = getAutoParam(PARAM_AUTO_v_trsumprd);
	int prm_c_in1      = getAutoParam(PARAM_AUTO_c_in1);

	//--- ���s�L���m�F ---
	if (prm_c_exe == 0){
		return false;
	}

	//--- limit�m�F�A�\���E�ԑg�񋟗L���m�F ---
	AddExistInfo exist_info;
	int cnt_limit = subAddGetLimit(exist_info, JLCMD_AUTO_ADDSP, autoscope);
	//--- limit����m�F�A����ȏ�ł���Ή��������I�� ---
	if (prm_limit != 0 && cnt_limit >= prm_limit){
		return false;
	}
	//--- �\���b�����w��ȏ゠��Ή��������I�� ---
	if (prm_c_limtrsum > 0 && exist_info.sec_tr >= prm_trsumprd){
		return false;
	}
	//--- �����̔ԑg�񋟏��͌����ʒu�Ɏg��Ȃ� ---
	exist_info.sponsor = false;

	//--- �D�揇�ʍő�̃V�[���`�F���W�ʒu�m�F�iprm_limit == 0�̎��͏������������s�j ---
	int nsc_prior = subAddSearch(JLCMD_AUTO_ADDSP, exist_info, autoscope);
	if (nsc_prior < 0){
		return false;
	}

	//--- ��ԗD�揇�ʂ̍�������ԑg�񋟂Ƃ��� ---
	if (nsc_prior > 0 && prm_limit > 0){
		//--- �\���ƔF�������ŏ��̓��e��ԑg�񋟕����Ƃ���ꍇ ---
		if (pdata->getScpArext(nsc_prior) == SCP_AREXT_L_TRKEEP){
			bool flag_detcand = false;
			Term term = {};
			term.ini = nsc_prior;
			bool cont = true;
			do{
				cont = getTermNext(term);
				if (cont){
					//--- �����͈͂𒴂�����I�� ---
					if (term.msec.ed > autoscope.ed){
						cont = false;
					}
					else{
						//--- ���̊m�F ---
						ScpArType arstat_term = getScpArstat(term);
						ScpArExtType arext_term = getScpArext(term);
						if (jlsd::isScpArTypeLogoBorder(arstat_term) &&
							arext_term != SCP_AREXT_L_TRKEEP){
							flag_detcand = true;
						}
					}
				}
			} while(flag_detcand == false && cont);
			// ��₪����ꍇ�͗\���F�����P��ɂ��炷
			if (cont){
				setScpArext(term, SCP_AREXT_L_TRKEEP);
			}
			// ��₪�Ȃ��ꍇ�͋����ݒ莞�ȊO�͒��~
			else if (prm_c_in1 < 2){
				nsc_prior = -1;
			}
		}
		//--- �ԑg�񋟕�����ݒ� ---
		if (nsc_prior > 0){
			// ���ʒu���J�b�g��Ԃ������ꍇ�͎��ȍ~�̃J�b�g�ʒu���m�F
			if (pdata->getScpArext(nsc_prior) == SCP_AREXT_L_TRCUT){
				Term term = {};
				term.ini = nsc_prior;
				bool cont = true;
				do{
					cont = getTermNext(term);
					if (cont){
						ScpArExtType arext_term = getScpArext(term);
						// �G���h�J�[�h�J�b�g�������ꍇ�͔��f�O�ɖ߂�
						if (arext_term == SCP_AREXT_L_ECCUT){
							setScpArext(term, SCP_AREXT_L_TRRAW);
						}
					}
				} while(cont);
			}
			subAddReviseUnitCm(nsc_prior);	// �����P�T�b�P��CM����
			pdata->setScpArext(nsc_prior, SCP_AREXT_L_SP);
		}
	}
	return true;
}


//---------------------------------------------------------------------
// AutoAdd EC �G���h�J�[�h�ǉ�����
// �o�́F
//  �Ԃ�l  : �J�b�g�������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoAddEC(RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_c_exe      = getAutoParam(PARAM_AUTO_c_exe);
	int prm_limit      = getAutoParam(PARAM_AUTO_v_limit);
	int prm_c_limtrsum = getAutoParam(PARAM_AUTO_c_limtrsum);
	int prm_trsumprd   = getAutoParam(PARAM_AUTO_v_trsumprd);

	//--- ���s�L���m�F ---
	if (prm_c_exe == 0){
		return false;
	}

	//--- limit�m�F�A�\���E�ԑg�񋟗L���m�F ---
	AddExistInfo exist_info;
	int cnt_limit = subAddGetLimit(exist_info, JLCMD_AUTO_ADDEC, autoscope);
	//--- limit����m�F�A����ȏ�ł���Ή��������I�� ---
	if (prm_limit != 0 && cnt_limit >= prm_limit){
		return false;
	}
	//--- �\���b�����w��ȏ゠��Ή��������I�� ---
	if (prm_c_limtrsum > 0 && exist_info.sec_tr >= prm_trsumprd){
		return false;
	}

	//--- �D�揇�ʍő�̃V�[���`�F���W�ʒu�m�F�iprm_limit == 0�̎��͏������������s�j ---
	int nsc_prior = subAddSearch(JLCMD_AUTO_ADDEC, exist_info, autoscope);
	if (nsc_prior < 0){
		return false;
	}

	//--- ��ԗD�揇�ʂ̍��������G���h�J�[�h�Ƃ��� ---
	if (nsc_prior > 0 && prm_limit > 0){
		// �G���h�J�[�h��ݒ�
		subAddReviseUnitCm(nsc_prior);	// �����P�T�b�P��CM����
		pdata->setScpArext(nsc_prior, SCP_AREXT_L_EC);
	}
	return true;
}


//---------------------------------------------------------------------
// AutoAdd TR �\���ǉ�����
// �o�́F
//  �Ԃ�l  : �J�b�g�������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoAddTR(RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_c_exe      = getAutoParam(PARAM_AUTO_c_exe);
	int prm_limit      = getAutoParam(PARAM_AUTO_v_limit);
	int prm_c_limtrsum = getAutoParam(PARAM_AUTO_c_limtrsum);
	int prm_trsumprd   = getAutoParam(PARAM_AUTO_v_trsumprd);

	//--- ���s�L���m�F ---
	if (prm_c_exe == 0){
		return false;
	}

	//--- limit�m�F�A�\���E�ԑg�񋟗L���m�F ---
	AddExistInfo exist_info;
	int cnt_limit = subAddGetLimit(exist_info, JLCMD_AUTO_ADDTR, autoscope);
	//--- limit����m�F�A����ȏ�ł���Ή��������I�� ---
	if (prm_limit != 0 && cnt_limit >= prm_limit){
		return false;
	}
	//--- �\���b�����w��ȏ゠��Ή��������I�� ---
	if (prm_c_limtrsum > 0 && exist_info.sec_tr >= prm_trsumprd){
		return false;
	}

	//--- �D�揇�ʍő�̃V�[���`�F���W�ʒu�m�F�iprm_limit == 0�̎��͏������������s�j ---
	Nsc nsc_prior = subAddSearch(JLCMD_AUTO_ADDTR, exist_info, autoscope);
	if (nsc_prior < 0){
		return false;
	}
	//--- �\�����o�Ȃ��̌��o ---
	if (nsc_prior <= 0 && prm_limit > 0){
		return false;
	}

	//--- ��ԗD�揇�ʂ̍�������\���Ƃ��� ---
	if (nsc_prior > 0 && prm_limit > 0){
		// �\����ݒ�
		subAddReviseUnitCm(nsc_prior);	// �����P�T�b�P��CM����
		pdata->setScpArext(nsc_prior, SCP_AREXT_L_TRKEEP);
	}
	return true;
}

//---------------------------------------------------------------------
// �����\��������
// �o�́F
//  �Ԃ�l  : ���o�����ړI�z�u�̍��v
//  exist_info:
//    trailer  : �\���L���m�F false=�Ȃ� true=����
//    sponsor  : �ԑg�񋟗L���m�F false=�Ȃ� true=����
//    sec_tr   : �\���F���b��
//    devideCm : CM�\���������ifalse=�Ȃ� true=����j
//---------------------------------------------------------------------
int JlsAutoScript::subAddGetLimit(AddExistInfo &exist_info, JlcmdAutoType cmdtype, RangeMsec autoscope){
	//--- �R�}���h�ɑΉ�����\���擾 ---
	ScpArExtType arext_dst;
	if (cmdtype == JLCMD_AUTO_ADDSP){			// �ԑg��
		arext_dst = SCP_AREXT_L_SP;
	}
	else if (cmdtype == JLCMD_AUTO_ADDEC){		// �G���h�J�[�h
		arext_dst = SCP_AREXT_L_EC;
	}
	else{
		arext_dst = SCP_AREXT_L_TRKEEP;
	}

	//--- limit�m�F�A�\���E�ԑg�񋟗L���m�F ---
	int  cnt_limit = 0;
	bool flag_trailer = false;
	bool flag_sponsor = false;
	Sec  totalsec_tr = 0;
	{
		Term term = {};
		bool cont = true;
		while(cont){
			cont = getTermNext(term);
			if (cont){
				ScpArExtType arext_term = getScpArext(term);
				if (checkScopeTerm(term, autoscope)){
					if (arext_term == arext_dst){			// �ړI����
						cnt_limit ++;
					}
					if (arext_term == SCP_AREXT_L_TRKEEP){	// �\������
						flag_trailer = true;
						// �\���b���擾
						Sec tmpsec = pdata->cnv.getSecFromMsec(term.msec.ed - term.msec.st);
						totalsec_tr += tmpsec;
					}
					if (arext_term == SCP_AREXT_L_SP){		// �ԑg�񋟑���
						flag_sponsor = true;
					}
				}
			}
		}
	}

	//--- �\�����o�Ȃ��̌��o��͔ԑg�񋟂ɂ��č���15�b�\���𕪉����ĔF�������� ---
	bool divide_cm = false;
	{
		if (pdata->recHold.msecTrPoint == autoscope.st && autoscope.st > 0){
			if (cmdtype == JLCMD_AUTO_ADDSP){		// �ԑg��
				divide_cm = true;
			}
		}
		if (flag_trailer == false && cmdtype == JLCMD_AUTO_ADDTR){	// �\���Ȃ��\�����o�������l
			divide_cm = true;
		}
	}

	exist_info.trailer   = flag_trailer;
	exist_info.sponsor   = flag_sponsor;
	exist_info.sec_tr    = totalsec_tr;
	exist_info.divideCm  = divide_cm;

	return cnt_limit;
}

//---------------------------------------------------------------------
// �D�揇�ʍő�̈ʒu������
// limit�Ȃ����͏������������s
// ���́F
//   exist_info : trailer,sponsor,divideCm���g�p
// �o�́F
//  �Ԃ�l  : �D�揇�ʍő�̃V�[���`�F���W�ԍ�
//---------------------------------------------------------------------
Nsc JlsAutoScript::subAddSearch(JlcmdAutoType cmdtype, AddExistInfo exist_info, RangeMsec autoscope){
	//--- �g�p�p�����[�^ ---
	int prm_secprev   = getAutoParam(PARAM_AUTO_v_secprev);
	int prm_secnext   = getAutoParam(PARAM_AUTO_v_secnext);
	int prm_limit     = getAutoParam(PARAM_AUTO_v_limit  );
	int prm_c_limloc  = getAutoParam(PARAM_AUTO_c_limloc );
	int prm_c_cutskip = getAutoParam(PARAM_AUTO_c_cutskip);
	int prm_c_wmin    = getAutoParam(PARAM_AUTO_c_wmin);
	int prm_c_wmax    = getAutoParam(PARAM_AUTO_c_wmax);
	int prm_c_w15     = getAutoParam(PARAM_AUTO_c_w15);
	int prm_c_wdefmin = getAutoParam(PARAM_AUTO_c_wdefmin);
	int prm_c_wdefmax = getAutoParam(PARAM_AUTO_c_wdefmax);

	//--- �R�}���h�ɑΉ�����\���擾 ---
	ScpArExtType arext_dst;
	switch(cmdtype){
		case JLCMD_AUTO_ADDSP:				// �ԑg��
			arext_dst = SCP_AREXT_L_SP;
			break;
		case JLCMD_AUTO_ADDEC:				// �G���h�J�[�h
			arext_dst = SCP_AREXT_L_EC;
			break;
		default:
			arext_dst = SCP_AREXT_L_TRKEEP;
			break;
	}

	//--- �e�V�[���`�F���W�ʒu�m�F ---
	Nsc nsc_prior     = -1;
	Nsc nsc_sub_prior = -1;
	{
		int total_prior     = 0;
		int total_sub_prior = 0;
		Term term = {};
		bool cont = getTermNext(term);
		while(cont){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			//--- �͈͓��̏ꍇ ---
			if (checkScopeTerm(term, autoscope)){
				AddLocInfo locinfo;
				//--- �w��ʒu�̑O���Ԃ��擾 ---
				subAddGetLocInfo(locinfo, term, autoscope);
//if (cmdtype == JLCMD_AUTO_ADDSP){
//printf("(%d,%d,%d:(%d,%d),(%d,%d),(%d,%d),%d)", term.nsc.st,term.nsc.ed,arstat_term,
//	locinfo.typeLogo, locinfo.secDifLogo, locinfo.typeTr, locinfo.secDifTr,
//	locinfo.typeSp, locinfo.secDifSp, locinfo.secDifSc);
//}
				//--- ��Ԃ���̗D�揇�ʐݒ� ---
				int prior1 = subAddGetPriority(locinfo, cmdtype, arstat_term, arext_term, exist_info);
				//--- ���̍\�����Ԃɐ��񂠂�ꍇ�̃`�F�b�N ---
				if (prm_secnext > 0){
					if (subAddCheckSec(term.nsc.ed, prm_secnext) == false){
						prior1 = 0;
					}
				}
				//--- �O�̍\�����Ԃɐ��񂠂�ꍇ�̃`�F�b�N ---
				if (prm_secprev > 0){
					if (subAddCheckSec(term.nsc.st, -1 * prm_secprev) == false){
						prior1 = 0;
					}
				}
				//--- ������p ---
				int sub_prior1 = prior1;
				//--- ���e���Ԃɂ��I�� ---
				int secdif_sc = locinfo.secDifSc;
				if ((secdif_sc >= prm_c_wmin && secdif_sc <= prm_c_wmax) ||
					(secdif_sc == 15 && prm_c_w15 > 0)){	//�w��͈͓�
				}
				else if (secdif_sc >= prm_c_wdefmin && secdif_sc <= prm_c_wdefmax){
					prior1 = 0;		// �w��͈͊O�ł��邪�W���͈͓�
				}
				else{				// ���Ԃ��͈͊O
					prior1 = 0;
					sub_prior1 = 0;
				}
				//--- �ŗD��ʒu�̍X�V ---
				if (total_prior < prior1){
					total_prior = prior1;
					nsc_prior = term.nsc.ed;
				}
				if (total_sub_prior < sub_prior1){
					total_sub_prior = sub_prior1;
					nsc_sub_prior = term.nsc.ed;
				}
				//--- limit�Ȃ��ꍇ�͖��񏑂����� ---
				if (prior1 > 0 && prm_limit == 0){
					subAddReviseUnitCm(term.nsc.ed);	// �����P�T�b�P��CM����
					pdata->setScpArext(term.nsc.ed, arext_dst);
				}
			}
			cont = getTermNext(term);
		}
	}
	//--- �W�����Ԃ̌��ʒu�݂̂Ɍ������肷�鏈�� ---
	if (prm_c_limloc > 0){
		if (nsc_prior >= 0 && nsc_sub_prior != nsc_prior){
			nsc_prior = -1;
		}
	}
	//--- �ǉ��ʒu���O�̗\�����J�b�g�͖��������鏈�� ---
	if (nsc_prior > 0 && prm_c_cutskip > 0){
		subAddCancelCut(nsc_prior, autoscope);
	}

	return nsc_prior;
}

//---------------------------------------------------------------------
// AutoAdd�Ŕ��f����w��ʒu�̑O���Ԃ���D�揇�ʎZ�o���f�[�^�擾
// �o�́F
//  locinfo
//    typeLogo       : ���S����̏�ԁi0-9:���S�Ȃ� 10-19:���S���� +100:���S�J�n���j
//    secDifLogo     : ���S����̎���
//    typeTr         : �\������̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O�� 3:�ŏ��̗\���ʒu 4:�\���Ԍ��ԁj
//    secDifTr       : �\������̎���
//    typeSp         : �ԑg�񋟂���̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O�� 3:�����j
//    secDifSp       : �G���h�J�[�h����̎��ԁi�O���̎��̂݌v���j
//    typeEc         : �G���h�J�[�h����̏�ԁi0:�Y���Ȃ� 1:�㑤 2:�O���j
//    secDifEc       : �ԑg�񋟂���̎���
//    typeNolast     : �\���ł͂Ȃ���ɓ��e������ꍇ=1
//    typeEndlogo    : �͈͓��Ɋ��S�Ɋ܂܂��Ō�̃��S����̏�ԁi0:�Y�����S�Ȃ� 1:���S�ȍ~ 2:���S��O���j
//    secDifSc       : �P�O�̔ԑg�\������̎���
//---------------------------------------------------------------------
void JlsAutoScript::subAddGetLocInfo(AddLocInfo &locinfo, Term target, RangeMsec autoscope){
	//--- �ݒ�l ---
	int prm_msec_wcomp_trmax = getConfig(CONFIG_VAR_msecWCompTRMax);

	//--- �g�p�p�����[�^ ---
	int prm_c_cutskip = getAutoParam(PARAM_AUTO_c_cutskip);
	int prm_c_lgprev  = getAutoParam(PARAM_AUTO_c_lgprev);
	int prm_c_lgpost  = getAutoParam(PARAM_AUTO_c_lgpost);

	//--- ���S�ʒu���擾 ---
	bool logomode = false;				// �^�[�Q�b�g�ʒu�̃��S���
	RangeNsc rnsc_logo = {-1, -1};		// �^�[�Q�b�g�ʒu�O��̃��S�ʒu
	Nsc  nsc_endrise = -1;				// �͈͓��ɑS�̂��܂܂��Ō�̃��S�J�n�ʒu
	{
		ElgCurrent elg = {};
		bool cont = true;
		bool want_logo = true;
		while(cont){
			cont = getElgNextKeep(elg);
			//--- �Ώۃ��S�Ɉ�ԋ߂������オ��^����������ʒu���擾 ---
			if (want_logo){
				if (elg.nscLastFall <= target.nsc.st && target.nsc.st < elg.nscRise){
					want_logo = false;
					logomode = false;
					rnsc_logo = {elg.nscLastFall, elg.nscRise};
					if (elg.nscLastFall == 0){
						rnsc_logo.st = -1;
					}
				}
				else if (elg.nscRise <= target.nsc.st && target.nsc.st < elg.nscFall){
					want_logo = false;
					logomode = true;
					rnsc_logo = {elg.nscRise, elg.nscFall};
				}
				else if (!cont){
					want_logo = false;
					logomode = false;
					rnsc_logo = {elg.nscFall, -1};
				}
			}
			//--- �͈͓��ɑS�̂��܂܂��Ō�̃��S�ʒu���擾 ---
			RangeMsec range_logo = {elg.msecRise, elg.msecFall};
			if (checkScopeRange(range_logo, autoscope) && cont){
				Term term = {};
				term.ini = elg.nscRise;
				bool nextflag = getTermNext(term);
				while(nextflag){
					if (term.nsc.ed > elg.nscFall){
						nextflag = false;
					}
					else{
						ScpArExtType arext_term = getScpArext(term);
						if (arext_term != SCP_AREXT_L_LGCUT){
							nextflag = false;
							nsc_endrise = term.nsc.st;		// �ݒ�ʒu
						}
					}
					if (nextflag){
						nextflag = getTermNext(term);
					}
				}
			}
		}
	}

	//--- �\���ʒu���擾 ---
	RangeNsc rnsc_tr_1st  = {-1, -1};
	RangeNsc rnsc_tr_2nd  = {-1, -1};
	RangeNsc rnsc_tr_stop = {-1, -1};
	RangeNsc rnsc_tr_near = {-1, -1};
	RangeNsc rnsc_sp      = {-1, -1};
	RangeNsc rnsc_ec      = {-1, -1};
	bool type_nolast = 0;
	bool flag_invalid_all = false;
	{
		Term term = {};
		Msec msec_difmin_tr = 0;
		ScpArExtType arext_hold = SCP_AREXT_NONE;
		bool cont = getTermNext(term);
		while(cont){
			//--- �����͈͓��̏��� ---
			if (checkScopeTerm(term, autoscope)){
				ScpArType arstat_term = getScpArstat(term);
				ScpArExtType arext_term = getScpArext(term);
				//--- �Ώۈʒu�Ƃ̋��� ---
				Msec msec_dif_target = 0;
				if (term.nsc.ed <= target.nsc.st){
					msec_dif_target = abs(target.msec.st - term.msec.ed);
				}
				else if (term.nsc.st >= target.nsc.ed){
					msec_dif_target = abs(term.msec.st - target.msec.ed);
				}
				//--- ���E�����������ꍇ�̕␳ ---
				if (arstat_term == SCP_AR_B_OTHER){
					if (arext_hold == SCP_AREXT_L_TRKEEP){
						arext_term = arext_hold;
					}
				}
				else{
					arext_hold = arext_term;
				}
				//--- �\������ ---
				if (arext_term == SCP_AREXT_L_TRKEEP){
					if (rnsc_tr_1st.st < 0){					// ��ԍŏ�
						rnsc_tr_1st  = term.nsc;
					}
					if (msec_difmin_tr > msec_dif_target || rnsc_tr_near.st < 0){	// ��ԋ߂�
						rnsc_tr_near = term.nsc;
						msec_difmin_tr = msec_dif_target;
					}
					if (rnsc_tr_stop.st >= 0 && term.nsc.ed <= target.nsc.st){	// �\���I���ʒu�ύX
						rnsc_tr_stop = {-1, -1};
						rnsc_tr_2nd  = {-1, -1};
					}
					if (rnsc_tr_stop.st >= 0 && rnsc_tr_2nd.st < 0){	// ��x�\���I����ĊJ
						rnsc_tr_2nd = term.nsc;
					}
				}
				else if (rnsc_tr_1st.st >= 0 && rnsc_tr_stop.st < 0){	// �\���I���ʒu
					rnsc_tr_stop = term.nsc;
				}
				//--- �ԑg�񋟕��� ---
				if (arext_term == SCP_AREXT_L_SP){
					if (rnsc_sp.st < 0){				// ��ԍŏ�
						rnsc_sp = term.nsc;
					}
				}
				//--- �G���h�J�[�h���� ---
				if (arext_term == SCP_AREXT_L_EC){
					if (rnsc_ec.st < 0){				// ��ԍŏ�
						rnsc_ec = term.nsc;
					}
				}
				//--- �\���E�ԑg�񋟗L���͈̓`�F�b�N ---
				if (term.msec.ed - term.msec.st > prm_msec_wcomp_trmax + pdata->msecValSpc){
					if (term.nsc.st >= target.nsc.ed){				// ���ݒn�_����ɗ\���𒴂���\��
						if (jlsd::isScpArTypeLogo(arstat_term)){	// ���S��
							if (arext_term != SCP_AREXT_L_LGCUT){	// ���S�J�b�g�����ȊO
								type_nolast = 1;					// �{�҂��܂���ɑ���
							}
						}
					}
				}
				//--- �\���E�ԑg�񋟖������`�F�b�N ---
				if (arext_term == SCP_AREXT_L_TRCUT ||
					arext_term == SCP_AREXT_L_ECCUT){
					if (term.nsc.ed <= target.nsc.st){			// �\���J�b�g����O�ɑ���
						if (prm_c_cutskip == 0){				// �J�b�g�ȍ~�͖����̎�
							flag_invalid_all = true;
						}
					}
				}
			}
			cont = getTermNext(term);
		}
	}

	//--- ���S�אڏ�Ԍ��o ---
	Sec sec_dif_logo = 0;
	int type_logo = 0;						// ���S���o�Ȃ�
	if (flag_invalid_all == false){
		int msec_dif_tmp1 = 600 * 1000;		// �����o���̍ő�t���[������ݒ�
		int msec_dif_tmp2 = 600 * 1000;		// �����o���̍ő�t���[������ݒ�
		if (rnsc_logo.st >= 0){					// ���S�؂�ւ��O������̋���
			// ���S�J�n���E�I��������₩��O�����̏���
			if ((prm_c_lgprev == 0 && prm_c_lgpost != 0 && logomode == true) ||
				(prm_c_lgprev != 0 && prm_c_lgpost == 0 && logomode == false)){
			}
			else{
				msec_dif_tmp1 = target.msec.st - pdata->getMsecScp(rnsc_logo.st);
			}
		}
		if (rnsc_logo.ed >= 0){					// ���S�؂�ւ��㑤����̋���
			// ���S�J�n���E�I��������₩��O�����̏���
			if ((prm_c_lgprev != 0 && prm_c_lgpost == 0 && logomode == true) ||
				(prm_c_lgprev == 0 && prm_c_lgpost != 0 && logomode == false)){
			}
			else{
				msec_dif_tmp2 = pdata->getMsecScp(rnsc_logo.ed) - target.msec.ed;
			}
		}
		int msec_dif_tmp;
		int type_logo_add = 0;
		int type_logo_pside = 0;
		if (msec_dif_tmp1 <= msec_dif_tmp2){	// �O�������ʒu�ɋ߂�
			msec_dif_tmp = msec_dif_tmp1;
			if (logomode == true){				// �J�n�����S����
				type_logo_add = 100;
			}
		}
		else{									// �㑤�����ʒu�ɋ߂�
			type_logo_pside = 1000;
			msec_dif_tmp = msec_dif_tmp2;
			if (logomode == false){				// �J�n�����S�Ȃ����I���������S����
				type_logo_add = 100;
			}
		}
		sec_dif_logo = pdata->cnv.getSecFromMsec(msec_dif_tmp);		// �b���ɕϊ�
		if (logomode){			// ���S����
			if (sec_dif_logo >= 300){
				type_logo = 10;			// ���S�����łT���ȏ㗣��Ă���
			}
			else if (sec_dif_logo >= 31){
				type_logo = 12;			// ���S������31�b�ȏ㗣��Ă���
			}
			else if (sec_dif_logo >= 15){
				type_logo = 13;			// ���S������15�b�ȏ㗣��Ă���
			}
			else if (sec_dif_logo > 1){
				type_logo = 15;			// ���S�����ŋ߂����אڂł͂Ȃ�
			}
			else{
				type_logo = 16;			// ���S�����ŗא�
			}
		}
		else{							// ���S�O��
			if (sec_dif_logo >= 300){
				type_logo = 0;			// ���S�O���łT���ȏ㗣��Ă���
			}
			else if (sec_dif_logo >= 15){
				type_logo = 3;			// ���S�O����15�b�ȏ㗣��Ă���
			}
			else if (sec_dif_logo > 1){
				type_logo = 4;			// ���S�O���ŋ߂����אڂł͂Ȃ�
			}
			else{
				type_logo = 7;			// ���S�O���ŗא�
			}
		}
		type_logo += type_logo_add;
		type_logo += type_logo_pside;
	}

	//--- �\������̎��� ---
	int sec_dif_tr, type_tr;
	{
		if (rnsc_tr_near.st < 0){				// �\����������Ȃ��ꍇ
			sec_dif_tr = 0;
			type_tr = 0;
		}
		else if (rnsc_tr_1st.st == target.nsc.ed){	// ���ʒu���\���J�n�ɗא�
			sec_dif_tr = 0;
			type_tr = 2;
		}
		else if (rnsc_tr_1st.ed == target.nsc.ed){	// ���ʒu���\���J�n�̍ŏ��̈ʒu
			sec_dif_tr = 0;
			type_tr = 3;
		}
		else if (rnsc_tr_2nd.st >= 0 && rnsc_tr_stop.st >= 0 &&
				 rnsc_tr_stop.ed == target.nsc.ed){		// ���ʒu���\������
			sec_dif_tr = 0;
			type_tr = 4;
		}
		else if (rnsc_tr_near.st >= target.nsc.ed){		// ���ʒu���\�����O
			sec_dif_tr = pdata->cnv.getSecFromMsec(pdata->getMsecScp(rnsc_tr_near.st) - target.msec.ed);
			type_tr = 2;
		}
		else{								// ���ʒu���\����
			sec_dif_tr = pdata->cnv.getSecFromMsec(target.msec.st - pdata->getMsecScp(rnsc_tr_near.ed));
			type_tr = 1;
		}
	}
	//--- �ԑg�񋟂���̎��� ---
	int sec_dif_sp, type_sp;
	{
		if (rnsc_sp.st < 0){
			sec_dif_sp = 0;
			type_sp = 0;
		}
		else if (rnsc_sp.st < target.nsc.ed && rnsc_sp.ed >= target.nsc.ed){	// ���ʒu���ԑg�񋟂Ɠ���
			sec_dif_sp = 0;
			type_sp = 3;
		}
		else if (rnsc_sp.st >= target.nsc.ed){		// ���ʒu���ԑg�񋟎�O
			sec_dif_sp = pdata->cnv.getSecFromMsec(pdata->getMsecScp(rnsc_sp.st) - target.msec.ed);
			type_sp = 2;
		}
		else{								// ���ʒu���ԑg�񋟌�
			sec_dif_sp = pdata->cnv.getSecFromMsec(target.msec.st - pdata->getMsecScp(rnsc_sp.ed));
			type_sp = 1;
		}
	}
	//--- �G���h�J�[�h��O�̎��� ---
	int sec_dif_ec, type_ec;
	{
		if (rnsc_ec.st < 0){
			sec_dif_ec = 0;
			type_ec = 0;
		}
		else if (rnsc_ec.st >= target.nsc.ed){		// ���ʒu���G���h�J�[�h��O
			sec_dif_ec = pdata->cnv.getSecFromMsec(pdata->getMsecScp(rnsc_ec.st) - target.msec.ed);
			type_ec = 2;
		}
		else{								// ���ʒu���G���h�J�[�h��
			sec_dif_ec = 0;					// ���ԃ`�F�b�N�͂��Ă��Ȃ�
			type_ec = 1;
		}
	}
	//--- �P�ʊԊu ---
	int sec_dif_sc = pdata->cnv.getSecFromMsec(target.msec.ed - target.msec.st);

	//--- �Ō�̃��S����̏�� ---
	int type_endlogo;
	{
		if (nsc_endrise < 0){
			type_endlogo = 0;
		}
		else if (target.msec.st < nsc_endrise){
			type_endlogo = 2;
		}
		else{
			type_endlogo = 1;
		}
	}

	//--- �Ԃ�l��� ---
	locinfo.secDifLogo    = sec_dif_logo;
	locinfo.typeLogo      = type_logo;
	locinfo.secDifTr      = sec_dif_tr;
	locinfo.typeTr        = type_tr;
	locinfo.secDifSp      = sec_dif_sp;
	locinfo.typeSp        = type_sp;
	locinfo.secDifEc      = sec_dif_ec;
	locinfo.typeEc        = type_ec;
	locinfo.secDifSc      = sec_dif_sc;
	locinfo.typeNolast    = type_nolast;
	locinfo.typeEndlogo   = type_endlogo;
}


//---------------------------------------------------------------------
// �ǉ����Ƃ��Ďw��ӏ��̗D�揇�ʂ��Z�o
// ���́F
//	locinfo				// �ʒu���
//	cmdtype				// �R�}���h���
//	arstat_cur			// �Ώۂ̔z�u���
//	arext_cur			// �Ώۂ̔z�u�g�����
//  exist_info
//	  trailer			// true:�\�����o�L��
//	  sponsor			// true:�ԑg�񋟗L��
//	  divideCm			// true:CM�\������������
// �o�́F
//  �Ԃ�l  : ���͔z�u�̗D�揇��
//---------------------------------------------------------------------
int JlsAutoScript::subAddGetPriority(AddLocInfo &locinfo, JlcmdAutoType cmdtype, ScpArType arstat_cur, ScpArExtType arext_cur, AddExistInfo exist_info){
	//--- �g�p�p�����[�^ ---
	int prm_c_lgprev  = getAutoParam(PARAM_AUTO_c_lgprev);
	int prm_c_lgpost  = getAutoParam(PARAM_AUTO_c_lgpost);
	int prm_c_lgintr  = getAutoParam(PARAM_AUTO_c_lgintr);
	int prm_c_lgy     = getAutoParam(PARAM_AUTO_c_lgy);
	int prm_c_lgn     = getAutoParam(PARAM_AUTO_c_lgn);
	int prm_c_lgbn    = getAutoParam(PARAM_AUTO_c_lgbn);
	int prm_c_search  = getAutoParam(PARAM_AUTO_c_search);
	int prm_scope     = getAutoParam(PARAM_AUTO_v_scope);
	int prm_scopen    = getAutoParam(PARAM_AUTO_v_scopen);
	int prm_c_lgsp    = getAutoParam(PARAM_AUTO_c_lgsp);
	int prm_c_in1     = getAutoParam(PARAM_AUTO_c_in1);
	int prm_c_chklast = getAutoParam(PARAM_AUTO_c_chklast);
	int prm_c_unitcmoff = getAutoParam(PARAM_AUTO_c_unitcmoff);
	int prm_c_unitcmon  = getAutoParam(PARAM_AUTO_c_unitcmon);

	//--- type_logo�𕪗� ---
	int type_logo_prior = locinfo.typeLogo % 10;				// �D�揇��
	int type_logo_in    = (locinfo.typeLogo / 10) % 10;			// ���S����
	int type_logo_prev  = (locinfo.typeLogo / 100) % 10;		// ���S��O
	int type_logo_pside = (locinfo.typeLogo / 1000) % 10;		// ���S�O��
	// ���ׂ��܂߃��S�Ȃ�
	int type_logo_nonbr = ((type_logo_in == 0) && (type_logo_prior <= 5))? 1 : 0;

	//--- �\���Ɣԑg�񋟂̈ʒu�֌W���疳���ӏ������o ---
	bool invalid_pos = false;
	bool invalid_sp = false;
	bool invalid_tr = false;
	int type_tr = locinfo.typeTr;
	int type_sp = locinfo.typeSp;
	int type_ec = locinfo.typeEc;
	Sec secdif_tr = locinfo.secDifTr;
	Sec secdif_sp = locinfo.secDifSp;
	Sec secdif_ec = locinfo.secDifEc;
	Sec secdif_logo = locinfo.secDifLogo;
	if (exist_info.sponsor && exist_info.trailer){
		if (prm_c_lgprev == 0){					// �O���𖳌���
			if (type_sp == 2 || type_tr == 2){
				invalid_pos = true;
			}
		}
		if (prm_c_lgpost == 0){					// �㑤�𖳌���
			if (type_sp == 1 || type_tr == 1){
				invalid_pos = true;
			}
		}
		if (prm_c_lgintr > 0){					// ���Ԃ��c��
			if (type_tr == 1 && (type_sp == 2 || type_sp == 0)){
				invalid_pos = false;
			}
			else if (type_tr == 2 && type_sp == 1){
				invalid_pos = false;
			}
			else if (type_tr == 4){
				invalid_pos = false;
			}
		}
		if (invalid_pos == false){					// �\���E�ԑg�񋟂Ƃ��ɗL���ȏꍇ
			if ((type_sp == 1 && type_tr == 1) ||	// �����㑤�̏ꍇ
				(type_sp == 2 && type_tr == 2)){	// �����O���̏ꍇ
				if (secdif_sp > secdif_tr){			// ����Ă�����߂������c��
					invalid_sp = true;
				}
				else if (secdif_sp < secdif_tr){	// ����Ă�����߂������c��
					invalid_tr = true;
				}
			}
		}
	}
	else if (exist_info.sponsor){
		if (prm_c_lgprev == 0){					// �O���𖳌���
			if (type_sp == 2){
				invalid_pos = true;
			}
		}
		if (prm_c_lgpost == 0){					// �㑤�𖳌���
			if (type_sp == 1){
				invalid_pos = true;
			}
		}
	}
	else if (exist_info.trailer){
		if (prm_c_lgprev == 0){					// �O���𖳌���
			if (type_tr == 2){
				invalid_pos = true;
			}
		}
		if (prm_c_lgpost == 0){					// �㑤�𖳌���
			if (type_tr == 1){
				invalid_pos = true;
			}
		}
	}
	//--- �����͈͂ɂ�閳���ݒ� ---
	// ���S�Ȃ��̂݌����Ń��S����̈�A�܂��͂��̋t�̏ꍇ�͖���
	if ((prm_c_lgy == 0 && type_logo_in != 0) ||
		(prm_c_lgn == 0 && type_logo_in == 0)){
		invalid_pos = true;
	}
	// ���ׂ��܂߂ă��S�Ȃ������ŏ����𖞂����Ȃ��ꍇ�͖���
	else if (prm_c_lgbn > 0 && type_logo_nonbr == 0){
		invalid_pos = true;
	}
	//--- ���オ�ԑg�񋟁E�G���h�J�[�h���݂̂Ƃ���ꍇ�ɏ����𖞂����Ȃ��ꍇ�͖��� ---
	bool is_next_sp = (exist_info.sponsor && type_sp == 2 && secdif_sp == 0)? true : false;
	bool is_next_ec = (type_ec == 2 && secdif_ec == 0)? true : false;
	if (prm_c_lgsp != 0 && is_next_sp == false && is_next_ec == false){
		invalid_pos = true;
	}

	//--- �����͈͋N�_�����S���o�ɂ��邩���f ---
	bool valid_onlylogo = false;
	if (prm_c_search >= 4 && prm_c_search <= 6){
		valid_onlylogo = true;
	}
	else if (prm_c_search >= 1 && prm_c_search <= 3){
		if (exist_info.trailer == false &&
			(exist_info.sponsor == false || cmdtype == JLCMD_AUTO_ADDTR || cmdtype == JLCMD_AUTO_ADDSP)){
			valid_onlylogo = true;
		}
	}
	//--- �����͈͂ɂ��D�揇�ʐݒ� ---
	int prior1 = 0;
	//--- ���S����̋������f ---
	if (invalid_pos == false){
		// �O�������Ȃ��őO���A�㑤�����Ȃ��Ō㑤�̏ꍇ�͖���
		if ((prm_c_lgprev == 0 && type_logo_prev != 0) ||
				 (prm_c_lgpost == 0 && type_logo_prev == 0)){
			prior1 = 0;
		}
		// ���S�؂�ւ��ʒu����
		else if (type_logo_prior > 0){
			if ( (prm_c_search == 0) ||
				((prm_c_search == 1 || prm_c_search == 4) && (secdif_logo <= 1)) ||
				((prm_c_search == 2 || prm_c_search == 5) &&
				    (secdif_logo >= prm_scopen && secdif_logo <= prm_scope)) ||
				((prm_c_search == 3 || prm_c_search == 6) && (secdif_logo == prm_scope))){
				if ((prm_c_search == 2) &&
					(type_logo_pside != 0 && secdif_logo != 0)){	// ��O���͗אڂɌ��肷��ꍇ
					prior1 = 0;
				}
				else{
					prior1 = type_logo_prior;
				}
			}
		}
	}
	//--- �\���E�ԑg�񋟁E�G���h�J�[�h����̋������f ---
	bool valid_in1 = false;		// �\���ƔF�������ŏ��̓��e��ԑg�񋟕����ɂ��鋖�t���O
	if (invalid_pos == false && valid_onlylogo == false){
		// �\���E�ԑg�񋟂�����ꍇ�̓��S�[����̗D�揇�ʂ́i���S�Ȃ��ŗאځj�ȊO�����ɂ���
		int prior1bak = prior1;
		if (prior1 > 0 && prior1 < 7){
			prior1bak = 0;
		}
		// ���S�D�揇�ʂ͊O�����߂ėD�揇�ʂ�����
		prior1 = 0;
		// �ԑg�񋟂�����ꍇ�̏���
		if (exist_info.sponsor){
			if (type_sp == 0){
			}
			// �O�������Ȃ��őO���A�㑤�����Ȃ��Ō㑤�̏ꍇ�͖���
			else if (invalid_pos){
			}
			// �ԑg�񋟂͖����̏ꍇ
			else if (invalid_sp){
			}
			// �T���͈͓�
			else if ( (prm_c_search == 0) ||
					 ((prm_c_search == 1) && (secdif_sp <= 1)) ||
					 ((prm_c_search == 2) &&
					      (secdif_sp >= prm_scopen && secdif_sp <= prm_scope)) ||
					 ((prm_c_search == 3) && (secdif_sp == prm_scope))){
				// �ԑg�񋟂��O
				if (type_sp == 2){
					// �אڂ̂ݗD�揇�ʂ��グ��
					if (secdif_sp <= 1){
						prior1 += 20;
					}
					// ��O���͗אڂɌ��肷��ꍇ
					else if (prm_c_search == 2){
						prior1 = 0;
					}
				}
				else if (type_sp == 1){
					// �ԑg�񋟒���ɗא�
					if (secdif_sp <= 1){
						prior1 += 130;
					}
					// �ԑg�񋟂���w�莞�Ԉȓ�
					else{
						prior1 += 30;
					}
				}
			}
		}
		if (exist_info.trailer){
			// �\�������n�_�ɋ߂��Ȃ��ꍇ�͖���
			if (type_tr == 0){
			}
			// �O�������Ȃ��őO���A�㑤�����Ȃ��Ō㑤�̏ꍇ�͖���
			else if (invalid_pos){
			}
			// �\���͖����̏ꍇ
			else if (invalid_tr){
			}
			// �T���͈͓�
			else if (((prm_c_search == 1) && (secdif_tr <= 1)) ||
					 ((prm_c_search == 2) && (secdif_tr <= prm_scope)) ||
					 ((prm_c_search == 3) && (secdif_tr == prm_scope))){
				// �\���J�n�O
				if (type_tr == 2){
					// �אڂ̂ݗD�揇�ʂ��グ��
					if (secdif_tr == 0){
						prior1 += 20;
						// ���S�Ȃ��������ꍇ�͗D��x���グ��
						if (type_logo_in == 0){
							prior1 += 15;
						}
					}
					// ��O���͗אڂɌ��肷��ꍇ
					else if (prm_c_search == 2){
						prior1 = 0;
					}
				}
				else if (type_tr == 1){
					// �\������ɗא�
					if (secdif_tr <= 1){
						prior1 += 30;
						// ���S�Ȃ��������ꍇ�͗D��x���グ��
						if (type_logo_in == 0){
							prior1 += 15;
						}
					}
					// �\������w�莞�Ԉȓ�
					else{
						prior1 += 20;
					}
				}
				else if (type_tr == 4){		// �\���Ԍ���
						prior1 += 40;
				}
				else if (type_tr == 3){		// �ŏ��̗\���ʒu
					// type_tr == 3 �͌���ێ����邪�A�ԑg�񋟂ɒu�����f������
					valid_in1 = true;
				}
			}
		}
		// �L���ł���Ύc�������S�D�揇�ʂ��ǉ�
		if (prior1 > 0){
			prior1 += prior1bak;
		}
	}
	//--- �{�ғ��e�O�̃��S�Ȃ��̈�ɗ\���E�G���h�J�[�h�ʒu���Ȃ����m�F ---
	if (prm_c_chklast > 0 && locinfo.typeNolast > 0){
		if (type_logo_in == 0){
			prior1 = 0;
		}
	}
	//--- �\���������炸�A�Ō�̃��S���O�őO���͌����O���ꍇ�̊m�F ---
	if (exist_info.trailer == false && locinfo.typeEndlogo == 2 && prm_c_lgprev == 0){
		prior1 = 0;
	}
	//--- ����Ԃɂ��I�� ---
	//--- �\���ƔF�������ŏ��̓��e��ԑg�񋟕����Ƃ���ꍇ�͓��ꏈ�� ---
	if (arext_cur == SCP_AREXT_L_TRKEEP && type_tr == 3 && prm_c_in1 > 0 && valid_in1){
		if (prm_c_in1 == 2){		// �D��x�グ��
			prior1 += 500;
		}
		if (prm_c_in1 == 1){		// �D��x�グ��
			prior1 += 50;
		}
		else{						// �D��x�ŏ�
			prior1 = 1;
		}
	}
	else if (prior1 > 0){
		if (arext_cur == SCP_AREXT_L_TRKEEP){
			prior1 = 0;
		}
		else if (arext_cur == SCP_AREXT_L_EC ||
				 arext_cur == SCP_AREXT_L_SP){
				prior1 = 0;
		}
		else if (arext_cur == SCP_AREXT_L_LGCUT ||
				 arext_cur == SCP_AREXT_L_LGADD ||
				 arext_cur == SCP_AREXT_N_LGCUT ||
				 arext_cur == SCP_AREXT_N_LGADD){
				prior1 = 0;
		}
		else if ((arstat_cur == SCP_AR_N_AUNIT) ||
				 (arstat_cur == SCP_AR_N_BUNIT)){
			// �����P�T�b�P��CM�̖���������
			if (prm_c_unitcmoff > 0){			// �����P�T�b�P��CM�͋����I��CM�ȊO�Ƃ���
			}
			else if (prm_c_unitcmon > 0){		// �����P�T�b�P��CM�͋����I��CM�Ƃ���
				prior1 = 0;
			}
			else if (type_tr != 1){				// �\����̔F�����ԈȊO
				if (exist_info.divideCm == false){	// �������Ȃ��ꍇ�͖�����CM����ύX���Ȃ�
					prior1 = 0;
				}
				else{							// ��������ꍇ�͗D��x�Œ�ŕێ�
					prior1 = 1;
				}
			}
		}
		else if (jlsd::isScpArTypeBorder(arstat_cur)){
			if (cmdtype == JLCMD_AUTO_ADDSP){	// �ԑg�񋟂̂ݗD��x�A�b�v
				prior1 += 200;
			}
		}
	}
	return prior1;
}


//---------------------------------------------------------------------
// ���̍\���܂ł̎��ԏ����ɍ��v���邩�`�F�b�N
// �o�́F
//  �Ԃ�l  : false=�w��b�̍\���Ȃ� true=�w��b�̍\������
//---------------------------------------------------------------------
bool JlsAutoScript::subAddCheckSec(Nsc nsc_cur, Sec difsec){
	bool ret = false;
	SearchDirType dr = SEARCH_DIR_NEXT;
	if (difsec < 0){
		dr = SEARCH_DIR_PREV;
	}
	Term term = {};
	term.ini = nsc_cur;
	bool cont = true;
	while(cont){
		if (dr == SEARCH_DIR_NEXT){
			cont = getTermNext(term);
		}
		else{
			cont = getTermPrev(term);
		}
		if (cont){
			Sec sec_dif_term = pdata->cnv.getSecFromMsec(term.msec.ed - term.msec.st);
			if (sec_dif_term == difsec){
				ret = true;
			}
			if (sec_dif_term >= difsec){
				cont = false;
			}
		}
	}
	return ret;
}


//---------------------------------------------------------------------
// �ǉ��ʒu���O�̗\�����J�b�g�͖��������鏈��
// �o�́F
//   �f�[�^(arext) : �z�u��Ԃ�ύX
//---------------------------------------------------------------------
void JlsAutoScript::subAddCancelCut(Nsc nsc_prior, RangeMsec autoscope){
	if (nsc_prior > 0){
		//--- �ǉ��ʒu�܂ł̌��� ---
		Term term = {};
		bool cont = getTermNext(term);
		while(cont && term.nsc.ed < nsc_prior){
			//--- �͈͓��̏ꍇ ---
			ScpArExtType arext_term = getScpArext(term);
			if (checkScopeTerm(term, autoscope)){
				//--- �J�b�g�����𖳌��� ---
				if (arext_term == SCP_AREXT_L_TRCUT){
					setScpArext(term, SCP_AREXT_L_TRKEEP);
				}
				else if (arext_term == SCP_AREXT_L_ECCUT){
					setScpArext(term, SCP_AREXT_L_TRRAW);
				}
			}
			cont = getTermNext(term);
		}
	}
}


//---------------------------------------------------------------------
// �����P�T�b�P��CM�̈ꕔ����������ꍇ�̏���
// �o�́F
//   �f�[�^(arstat,arext) : �����P�T�b�P��CM������������
//---------------------------------------------------------------------
void JlsAutoScript::subAddReviseUnitCm(Nsc nsc_target){
	int num_scpos = pdata->sizeDataScp();
	if (nsc_target <= 0 || nsc_target >= num_scpos){
		return;
	}
	ScpArType arstat_target = pdata->getScpArstat(nsc_target);
	ScpArType arstat_new = SCP_AR_N_OTHER;
	ScpArExtType arext_new  = SCP_AREXT_NONE;

	//--- �����P�T�b�P��CM�������ꍇ�̎�O������ ---
	if ((arstat_target == SCP_AR_N_AUNIT || arstat_target == SCP_AR_N_BUNIT) && nsc_target > 1){
		//--- ��O��������̏�Ԃ�ݒ� ---
		{
			//--- ��O���\�����`�F�b�N ---
			Term term = {};
			term.ini  = nsc_target;
			bool cont = getTermPrev(term);
			ScpArType arstat_term = getScpArstat(term);
			bool flag_unitcont = true;
			//--- CM�����������ꍇ�͕����̎�O�\�����擾 ---
			while(cont && flag_unitcont && (arstat_term == SCP_AR_N_AUNIT || arstat_term == SCP_AR_N_BUNIT)){
				if (arstat_term == SCP_AR_N_BUNIT){		// �����擪�̎��ŏI��
					flag_unitcont = false;
				}
				cont = getTermPrev(term);
				arstat_term = getScpArstat(term);
			}
			if (cont){
				ScpArExtType arext_term = getScpArext(term);
				//--- ��O���\���̏ꍇ�͗\����Ԃ��p��������ݒ� ---
				if ((arext_term == SCP_AREXT_L_TRKEEP) ||
					(arext_term == SCP_AREXT_L_TRRAW )){
					arext_new = SCP_AREXT_L_TRRAW;		// ��O��������̏�Ԃ�ύX
				}
			}
		}
		//--- �������� ---
		{
			Term term = {};
			term.ini  = nsc_target;
			bool cont = getTermPrev(term);
			ScpArType arstat_term = getScpArstat(term);
			//--- ���Ԉʒu������ ---
			while(cont && arstat_term == SCP_AR_N_AUNIT){
				setScpArstat(term, arstat_new);
				setScpArext(term, arext_new);
				cont = getTermPrev(term);
				arstat_term = getScpArstat(term);
			}
			//--- �擪�ʒu������ ---
			if (arstat_term == SCP_AR_N_BUNIT){
				setScpArstat(term, arstat_new);
				setScpArext(term, arext_new);
			}
		}
	}

	// �����P�T�b�P��CM�������ꍇ�̌㔼��������
	if ((arstat_target == SCP_AR_N_AUNIT || arstat_target == SCP_AR_N_BUNIT) && nsc_target < num_scpos - 1){
		int keep = 0;
		Msec msec_start = 0;
		ScpArExtType arext_keep;
		bool change = true;
		Term term = {};
		term.ini  = nsc_target;
		bool cont = getTermNext(term);
		while(cont && keep >= 0 && keep <= 2){
			ScpArType arstat_term = getScpArstat(term);
			ScpArExtType arext_term = getScpArext(term);
			if ((arstat_term == SCP_AR_N_AUNIT) ||
				(arstat_term == SCP_AR_N_BUNIT && change) ||
				(arstat_term == SCP_AR_N_OTHER && change)){
				keep ++;
				if (keep == 1){
					change = true;						// �r����Ԃ�ݒ�
					msec_start = term.msec.st;			// 15�b�P�ʊJ�n�ʒu
					arext_keep = arext_term;
				}
				else if (arext_term != arext_keep){		// �\�����ς�����番���ݒ肵�Ȃ�
					change = false;
				}
				if (keep > 1 && change){				// �Q��ڈȍ~�̏ꍇ
					Sec sec_dif  = pdata->cnv.getSecFromMsec(term.msec.ed - msec_start);
					// �V���ɍ����P�T�b�P�ʂƂȂ�ꍇ�͐ݒ�
					if ((sec_dif % 15) == 0 && sec_dif <= 30){
						Nsc nsc_lastwrite = -1;
						Term subterm = term;
						for(int i=0; i<keep-1; i++){
							Msec msec_prm_gap = (Msec) pdata->getConfig(CONFIG_VAR_msecMgnCmDivide);
							Msec msec_sub_gapp = calcDifGap(subterm.msec.st, msec_start);
							Msec msec_sub_gapn = calcDifGap(subterm.msec.st, term.msec.ed);
							if (msec_sub_gapp <= msec_prm_gap && msec_sub_gapn <= msec_prm_gap){
								setScpArstat(subterm, SCP_AR_N_AUNIT);
								nsc_lastwrite = subterm.nsc.st;
							}
							else{
								pdata->setScpChap(subterm.nsc.st, SCP_CHAP_NONE);	// �\������
								if (nsc_lastwrite == subterm.nsc.ed){
									setScpArstat(subterm, SCP_AR_N_AUNIT);
								}
								else if (nsc_lastwrite < 0){
									setScpArstat(subterm, SCP_AR_N_AUNIT);
									nsc_lastwrite = subterm.nsc.ed;
								}
							}
							getTermPrev(subterm);
						}
						if (nsc_lastwrite == term.nsc.ed){
							pdata->setScpArstat(nsc_lastwrite, SCP_AR_N_UNIT);
						}
						else if (nsc_lastwrite > 0){
							pdata->setScpArstat(nsc_lastwrite, SCP_AR_N_BUNIT);
						}
						keep = 0;
						change = false;
					}
					else if (sec_dif > 30) change = false;
				}
				if (keep > 0){
					setScpArstat(term, SCP_AR_N_OTHER);
				}
			}
			else{
				cont = false;
			}
			//--- ���̈ʒu�ݒ� ---
			if (cont){
				cont = getTermNext(term);
			}
		}
	}
}



//---------------------------------------------------------------------
// AutoEdge �J�n�O����
// �o�́F
//  �Ԃ�l  : ������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoEdge(JlsCmdLimit &cmdlimit){
	Nsc nsc_base = subInsDelGetBase(cmdlimit);
	bool flag_exec = false;
	if (nsc_base >= 0){
		LogoEdgeType edge_base = cmdlimit.getLogoBaseEdge();
		flag_exec = startAutoEdgeMain(nsc_base, edge_base);
	}
	return flag_exec;
}


//---------------------------------------------------------------------
// AutoEdge ���S�[�����̃J�b�g�E�ǉ�
// �o�́F
//  �Ԃ�l  : ������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::startAutoEdgeMain(Nsc nsc_elg, LogoEdgeType edge_elg){
	//--- �g�p�p�����[�^ ---
	int prm_c_exe      = getAutoParam(PARAM_AUTO_c_exe);
	int prm_c_cmpart   = getAutoParam(PARAM_AUTO_c_cmpart);

	//--- ���s�L���m�F ---
	bool flag_exec = false;
	if (prm_c_exe == 0){
		return false;
	}
	//--- ���S�G�b�W��ނ��擾 ---
//	bool nedge = pdata->isElgInScp(nsc_elg);

	//--- ���S�J�n�O�ɒǉ� ---
	if (nsc_elg >= 0 && edge_elg == LOGO_EDGE_RISE && prm_c_cmpart > 0){
		flag_exec = subEdgeExec(nsc_elg, LOGO_EDGE_RISE, SEARCH_DIR_PREV);
	}
	//--- ���S�J�n����ɒǉ� ---
	else if (nsc_elg >= 0 && edge_elg == LOGO_EDGE_RISE && prm_c_cmpart == 0){
		flag_exec = subEdgeExec(nsc_elg, LOGO_EDGE_RISE, SEARCH_DIR_NEXT);
	}
	//--- ���S�I�����O�ɒǉ� ---
	else if (nsc_elg >= 0 && edge_elg == LOGO_EDGE_FALL && prm_c_cmpart == 0){
		flag_exec = subEdgeExec(nsc_elg, LOGO_EDGE_FALL, SEARCH_DIR_PREV);
	}
	//--- ���S�I������ɒǉ� ---
	else if (nsc_elg >= 0 && edge_elg == LOGO_EDGE_FALL && prm_c_cmpart > 0){
		flag_exec = subEdgeExec(nsc_elg, LOGO_EDGE_FALL, SEARCH_DIR_NEXT);
	}
	return flag_exec;
}

//---------------------------------------------------------------------
// ���S�[�����̃J�b�g�E�ǉ����s����
// ���́F
//   nsc_logo�F    ���S��_�ɑΉ�����V�[���`�F���W�ԍ�
//   edge_logo:    ���S�G�b�W�����オ��^����������iLOGO_EDGE_RISE / LOGO_EDGE_FALL
//   dr:           ���������O��iSEARCH_DIR_PREV / SEARCH_DIR_NEXT�j
// �o�́F
//  �Ԃ�l  : ������s false=�����s true=���s
//---------------------------------------------------------------------
bool JlsAutoScript::subEdgeExec(Nsc nsc_logo, LogoEdgeType edge_logo, SearchDirType dr){
	//--- �g�p�p�����[�^ ---
	int prm_c_search   = getAutoParam(PARAM_AUTO_c_search);
	int prm_scope      = getAutoParam(PARAM_AUTO_v_scope);
	int prm_c_wmin     = getAutoParam(PARAM_AUTO_c_wmin);
	int prm_c_wmax     = getAutoParam(PARAM_AUTO_c_wmax);
	int prm_c_add      = getAutoParam(PARAM_AUTO_c_add);
	int prm_c_allcom   = getAutoParam(PARAM_AUTO_c_allcom);
	bool flag_adapt = (prm_c_search == 2 || prm_c_search == 3)? false : true;	// �����J�b�g�K�p�ォ��̎�1
	bool flag_ovw = (prm_c_search == 3)? true : false;

	//--- ������ ---
	bool flag_exec     = false;
	//--- ���S���̕ύX�����f ---
	bool inlogo = false;
	if ((edge_logo == LOGO_EDGE_RISE && dr == SEARCH_DIR_NEXT) ||
		(edge_logo == LOGO_EDGE_FALL && dr == SEARCH_DIR_PREV)){
		inlogo = true;
	}
	//--- ���S�I���ʒu���擾 ---
	Nsc nsc_endlogo;
	if (dr == SEARCH_DIR_PREV){
		nsc_endlogo = pdata->getNscPrevElg(nsc_logo, LOGO_EDGE_BOTH);
		if (nsc_endlogo < 0){			// �O���Ȃ���ΐ擪�ʒu
			nsc_endlogo = 0;
		}
	}
	else{
		nsc_endlogo = pdata->getNscNextElg(nsc_logo, LOGO_EDGE_BOTH);
		if (nsc_endlogo < 0){			// �����Ȃ���΍ŏI�ʒu
			nsc_endlogo = pdata->sizeDataScp() - 1;
		}
	}
	//--- ���S�ʒu���珇�ԂɊm�F ---
	{
		Nsc  nsc_base  = nsc_logo;
		Msec msec_logo = pdata->getMsecScp(nsc_logo);
		Msec msec_base = pdata->getMsecScp(nsc_base);
		Term term = {};
		term.ini = nsc_logo;
		bool cont = true;
		while(cont){
			Nsc nsc_dst;
			if (dr == SEARCH_DIR_PREV){			// �O���Ɍ�����
				cont = getTermPrev(term);
				nsc_dst = term.nsc.st;
				if (nsc_dst < nsc_endlogo) cont = false;
			}
			else{								// �㑤�Ɍ�����
				cont = getTermNext(term);
				nsc_dst = term.nsc.ed;
				if (nsc_dst > nsc_endlogo) cont = false;
			}
			if (cont){
				Msec msec_dst = pdata->getMsecScp(nsc_dst);
				Sec sec_dif_base = pdata->cnv.getSecFromMsec( abs(msec_dst - msec_base) );
				Sec sec_dif_logo = pdata->cnv.getSecFromMsec( abs(msec_dst - msec_logo) );
				ScpArExtType arext_term = getScpArext(term);
				//--- �����̃J�b�g�i�c���j��Ԃ��擾 ---
				bool select_arext = false;
				if (arext_term == SCP_AREXT_N_LGCUT ||
					arext_term == SCP_AREXT_N_LGADD ||
					arext_term == SCP_AREXT_L_LGCUT ||
					arext_term == SCP_AREXT_L_LGADD){
					select_arext = true;					// �J�b�g�i�c���j�����ɑ���
				}
				//--- �����̃J�b�g�i�c���j�ŃV�t�g����Ԃ̏ꍇ�͊�ʒu��ύX ---
				if (flag_adapt && select_arext){
					nsc_base = nsc_dst;						// �����J�b�g�K�p��̈ʒu�Ɋ���ړ�
					msec_base = msec_dst;
				}
				else{
					if (flag_adapt) flag_adapt = false;		// ��ʒu�m��
					//--- �J�b�g�\�ȍő�b���`�F�b�N ---
					if ((sec_dif_logo > prm_scope) || sec_dif_base > prm_c_wmax){
						cont = false;
					}
					//--- �����J�b�g�����݂ŏ����������Ȃ� ---
					else if (select_arext && flag_ovw == false){
						cont = false;
					}
					//--- �J�b�g�K�p�͈͓��̏��� ---
					else if (sec_dif_base >= prm_c_wmin && sec_dif_base <= prm_c_wmax){
						flag_exec = true;
						if (prm_c_allcom == 0){		// ����������I������ꍇ
							cont = false;
						}
						ScpArExtType arext_add;
						if (prm_c_add == 0){
							arext_add = (inlogo)? SCP_AREXT_L_LGCUT : SCP_AREXT_N_LGCUT;
						}
						else{
							arext_add = (inlogo)? SCP_AREXT_L_LGADD : SCP_AREXT_N_LGADD;
						}
						//--- ���݈ʒu��ݒ肵�A�����\���������ꍇ�͑S�\���Őݒ� ---
						setScpArext(term, arext_add);		// �����������s
						Term subterm = term;
						bool subcont = true;
						while(subcont){
							if (dr == SEARCH_DIR_PREV){	// �O���Ɍ�����
								subcont = getTermNext(subterm);
								if (subterm.nsc.ed > nsc_base){
									subcont = false;
								}
							}
							else{						// �㑤�Ɍ�����
								subcont = getTermPrev(subterm);
								if (subterm.nsc.st < nsc_base){
									subcont = false;
								}
							}
							if (subcont) setScpArext(subterm, arext_add);	// �����������s
						}
					}
				}
			}
		}
	}
	return flag_exec;
}




//---------------------------------------------------------------------
// ���̃��S���ԍ\���i�����オ��^����������j�ʒu�ԍ����擾�i���s���͍X�V���Ȃ��j
// ����
//  elg     : ���S�����ʒu���
// �o�́F
//  �Ԃ�l  : 0:���S�Ȃ�  1:���S����
//  elg     : ���S�����ʒu���i���S�Ȃ��̎��͍X�V���Ȃ��j
//---------------------------------------------------------------------
bool JlsAutoScript::getElgNextKeep(ElgCurrent &elg){
	ElgCurrent elgbak = elg;
	bool ret = pdata->getElgptNext(elg);
	if (ret == false){
		elg = elgbak;
	}
	return ret;
}



//---------------------------------------------------------------------
// �ݒ�l�̎擾�isetParam�R�}���h�ݒ�j
//---------------------------------------------------------------------
int JlsAutoScript::getConfig(ConfigVarType tp){
	return pdata->getConfig(tp);
}

//---------------------------------------------------------------------
// �ݒ�l�̎擾�iauto�R�}���h���s���̃p�����[�^�j
//---------------------------------------------------------------------
int JlsAutoScript::getAutoParam(JlParamAuto tp){
	return m_autoArg.getParam(tp);
}

//---------------------------------------------------------------------
// �������ɒ[���܂߂邩�I��
//---------------------------------------------------------------------
void JlsAutoScript::setTermEndtype(Term &term, ScpEndType endtype){
	pdata->setTermEndtype(term, endtype);
}
//---------------------------------------------------------------------
// ���̍\�����擾
//---------------------------------------------------------------------
bool JlsAutoScript::getTermNext(Term &term){
	if (!term.endfix){				// �G�b�W��񖢐ݒ莞
		ScpEndType noedge = (getAutoParam(PARAM_AUTO_c_noedge)!= 0)? SCP_END_NOEDGE : SCP_END_EDGEIN;
		setTermEndtype(term, noedge);
	}
	return pdata->getTermNext(term);
}
//---------------------------------------------------------------------
// �O�̍\�����擾
//---------------------------------------------------------------------
bool JlsAutoScript::getTermPrev(Term &term){
	if (!term.endfix){				// �G�b�W��񖢐ݒ莞
		ScpEndType noedge = (getAutoParam(PARAM_AUTO_c_noedge)!= 0)? SCP_END_NOEDGE : SCP_END_EDGEIN;
		setTermEndtype(term, noedge);
	}
	return pdata->getTermPrev(term);
}
//---------------------------------------------------------------------
// �e�v�f�̐ݒ�E�擾
//---------------------------------------------------------------------
ScpArType JlsAutoScript::getScpArstat(Term term){
	return pdata->getScpArstat(term);
}
ScpArExtType JlsAutoScript::getScpArext(Term term){
	return pdata->getScpArext(term);
}
void JlsAutoScript::setScpArstat(Term term, ScpArType arstat){
	pdata->setScpArstat(term, arstat);
}
void JlsAutoScript::setScpArext(Term term, ScpArExtType arext){
	pdata->setScpArext(term, arext);
}
//---------------------------------------------------------------------
// CM�\��������
//---------------------------------------------------------------------
bool JlsAutoScript::isScpArstatCmUnit(Term term){
	return pdata->isScpArstatCmUnit(term);
}
//---------------------------------------------------------------------
// �����͈͓��ɂ��邩����
//---------------------------------------------------------------------
bool JlsAutoScript::checkScopeTerm(Term term, RangeMsec scope){
	return pdata->checkScopeTerm(term, scope);
}
bool JlsAutoScript::checkScopeRange(RangeMsec bounds, RangeMsec scope){
	return pdata->checkScopeRange(bounds, scope);
}

//---------------------------------------------------------------------
// �b�P�ʂ̌덷���擾
//---------------------------------------------------------------------
Msec JlsAutoScript::calcDifGap(Msec msec1, Msec msec2){
	Msec msec_dif = abs(msec1 - msec2);
	Msec msec_gap_pre = msec_dif - (msec_dif / 1000 * 1000);
	Msec msec_gap = (msec_gap_pre > 500)? 1000-msec_gap_pre : msec_gap_pre;
	return msec_gap;
}
