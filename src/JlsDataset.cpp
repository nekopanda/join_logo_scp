//
// join_logo_scp �f�[�^�i�[�N���X
//

#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsDataset.hpp"

//=====================================================================
// �����ݒ�
//=====================================================================

JlsDataset::JlsDataset(){
	//--- �֐��|�C���^�쐬 ---
	this->pdata = this;
	//--- �f�[�^������ ---
	initData();
}

//---------------------------------------------------------------------
// �����l�ݒ�
//---------------------------------------------------------------------
void JlsDataset::initData(){
	m_scp.clear();
	m_logo.clear();
	resultTrim.clear();

	//--- �����ݒ� ---
	m_config[CONFIG_VAR_msecWLogoTRMax]      = 120*1000;
	m_config[CONFIG_VAR_msecWCompTRMax]      = 60*1000;
	m_config[CONFIG_VAR_msecWLogoSftMrg]     = 4200;
	m_config[CONFIG_VAR_msecWCompFirst]      = 0;
	m_config[CONFIG_VAR_msecWCompLast]       = 0;
	m_config[CONFIG_VAR_msecWLogoSumMin]     = 20*1000;
	m_config[CONFIG_VAR_msecWLogoLgMin]      = 4500;
	m_config[CONFIG_VAR_msecWLogoCmMin]      = 40*1000;
	m_config[CONFIG_VAR_msecWLogoRevMin]     = 185*1000;
	m_config[CONFIG_VAR_msecMgnCmDetect]     = 1500;
	m_config[CONFIG_VAR_msecMgnCmDivide]     = 500;
	m_config[CONFIG_VAR_secWCompSPMin]       = 6;
	m_config[CONFIG_VAR_secWCompSPMax]       = 13;
	m_config[CONFIG_VAR_flagCutTR]           = 1;
	m_config[CONFIG_VAR_flagCutSP]           = 0;
	m_config[CONFIG_VAR_flagAddLogo]         = 1;
	m_config[CONFIG_VAR_flagAddUC]           = 0;
	m_config[CONFIG_VAR_typeNoSc]            = 0;
	m_config[CONFIG_VAR_cancelCntSc]         = 0;
	m_config[CONFIG_VAR_LogoLevel]           = 0;
	m_config[CONFIG_VAR_LogoRevise]          = 0;
	m_config[CONFIG_VAR_AutoCmSub]           = 0;
	m_config[CONFIG_VAR_msecPosFirst]        = -1;
	m_config[CONFIG_VAR_msecLgCutFirst]      = -1;
	m_config[CONFIG_VAR_msecZoneFirst]       = -1;
	m_config[CONFIG_VAR_msecZoneLast]        = -1;
	m_config[CONFIG_VAR_priorityPosFirst]    = 0;

	//--- �O���ݒ�I�v�V���� ---
	extOpt.verbose    = 0;
	extOpt.msecCutIn  = 0;
	extOpt.msecCutOut = 0;
	extOpt.frmLastcut = 0;
	extOpt.wideCutIn  = 0;
	extOpt.wideCutOut = 0;
	extOpt.flagNoLogo = 0;
	extOpt.fixCutIn   = 0;
	extOpt.fixCutOut  = 0;
	extOpt.fixWidCutI = 0;
	extOpt.fixWidCutO = 0;
	extOpt.oldAdjust  = 0;

	//--- ��ԏ����ݒ� ---
	recHold.msecSelect1st = -1;
	recHold.msecTrPoint   = -1;
	recHold.rmsecHeadTail = {-1, -1};
	m_msecTotalMax    = 0;
	m_levelUseLogo  = 0;
	m_flagSetupAdj  = 0;
	m_flagSetupAuto = 0;
	m_nscOutDetail = 0;

	//--- �Œ�l�ݒ� ---
	msecValExact = 100;
	msecValNear1 = 200;
	msecValNear2 = 400;
	msecValNear3 = 1200;
	msecValLap1  = 700;
	msecValLap2  = 2500;
	msecValSpc   = 1200;
};



//=====================================================================
// ����ݒ�̕ۑ��E�ǂݏo��
//=====================================================================

//---------------------------------------------------------------------
// config�ݒ�
//---------------------------------------------------------------------
void JlsDataset::setConfig(ConfigVarType tp, int val){
	m_config[tp] = val;
}

//---------------------------------------------------------------------
// config�ݒ�l�擾
//---------------------------------------------------------------------
int JlsDataset::getConfig(ConfigVarType tp){
	return m_config[tp];
}

int JlsDataset::getConfigAction(ConfigActType acttp){
	int val;
	int ret = 0;
	switch(acttp){
		case CONFIG_ACT_LogoDelEdge:		// ���S�[��CM���f
			val = getConfig(CONFIG_VAR_LogoRevise);
			ret = val % 10;
			break;
		case CONFIG_ACT_LogoDelMid:			// ���S����15�b�P��CM��
			val = getConfig(CONFIG_VAR_LogoRevise);
			ret = ((val / 10 % 10) & 0x1)? 1 : 0;
			break;
		case CONFIG_ACT_LogoDelWide:		// �L�惍�S�Ȃ��폜
			val = getConfig(CONFIG_VAR_LogoRevise);
			ret = ((val / 10 % 10) & 0x2)? 1 : 0;
			{								// AddUC=1�̎��͖���
				int tmp = getConfig(CONFIG_VAR_flagAddUC);
				if ((tmp % 10) & 0x1) ret = 0;
			}
			break;
		case CONFIG_ACT_LogoUCRemain:		// ���S�Ȃ��s���������c��
			val = getConfig(CONFIG_VAR_flagAddUC);
			ret = val % 10;
			break;
		case CONFIG_ACT_LogoUCGapCm:		// CM�P�ʂ���덷���傫���\�����c��
			val = getConfig(CONFIG_VAR_flagAddUC);
			ret = ((val / 10 % 10) & 0x1)? 1 : 0;
			if ( isUnuseLevelLogo() ) ret = 1;		// ���S�g�p���Ȃ��ꍇ�͏펞
			break;
		case CONFIG_ACT_MuteNoSc:			// �V�[���`�F���W�Ȃ������ʒu��CM���f
			val = getConfig(CONFIG_VAR_typeNoSc);
			ret = val;
			if (val == 0){					// �������f
				ret = ( pdata->isUnuseLevelLogo() )? 1 : 2;
			}
			break;
		default:
			break;
	}
	return ret;
}


//=====================================================================
// �f�[�^�T�C�Y�擾
//=====================================================================

//---------------------------------------------------------------------
// ���S�f�[�^�T�C�Y�擾
//---------------------------------------------------------------------
int JlsDataset::sizeDataLogo(){
	return (int) m_logo.size();
}
//---------------------------------------------------------------------
// ����SC�f�[�^�T�C�Y�擾
//---------------------------------------------------------------------
int JlsDataset::sizeDataScp(){
	return (int) m_scp.size();
}

//---------------------------------------------------------------------
// ���S�f�[�^����H
//---------------------------------------------------------------------
bool JlsDataset::emptyDataLogo(){
	return m_logo.empty();
}


//=====================================================================
// �P�f�[�^�Z�b�g�P�ʂ̏���
//=====================================================================

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʏ������i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::clearRecordLogo(DataLogoRecord &dt){
	dt.org_rise     = 0;
	dt.org_fall     = 0;
	dt.org_rise_l   = 0;
	dt.org_rise_r   = 0;
	dt.org_fall_l   = 0;
	dt.org_fall_r   = 0;
	dt.rise         = 0;
	dt.fall         = 0;
	dt.rise_l       = 0;
	dt.rise_r       = 0;
	dt.fall_l       = 0;
	dt.fall_r       = 0;
	dt.fade_rise    = 0;
	dt.fade_fall    = 0;
	dt.intl_rise    = 0;
	dt.intl_fall    = 0;
	dt.stat_rise    = LOGO_PRIOR_NONE;
	dt.stat_fall    = LOGO_PRIOR_NONE;
	dt.unit         = LOGO_UNIT_NORMAL;
	dt.outtype_rise = LOGO_RESULT_NONE;
	dt.outtype_fall = LOGO_RESULT_NONE;
	dt.result_rise  = 0;
	dt.result_fall  = 0;
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʏ������i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::clearRecordScp(DataScpRecord &dt){
	dt.msec     = 0;
	dt.msbk     = 0;
	dt.msmute_s = -1;
	dt.msmute_e = -1;
	dt.still    = 0;
	dt.statpos  = SCP_PRIOR_NONE;
	dt.score    = 0;
	dt.chap     = SCP_CHAP_NONE;
	dt.arstat   = SCP_AR_UNKNOWN;
	dt.arext    = SCP_AREXT_NONE;
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʑ}���i�Ō�̈ʒu�j�i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::pushRecordLogo(DataLogoRecord &dt){
	m_logo.push_back(dt);						// add data
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʑ}���i�Ō�̈ʒu�j�i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::pushRecordScp(DataScpRecord &dt){
	m_scp.push_back(dt);						// add data
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʍ폜�i�Ō�̈ʒu�j�i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::popRecordLogo(){
	m_logo.pop_back();							// delete data
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʑ}���i�w��ʒu�j�i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::insertRecordLogo(DataLogoRecord &dt, Nlg nlg){
	if (nlg >= 0 && nlg <= sizeDataLogo()){
		m_logo.insert(m_logo.begin()+nlg, dt);
	}
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʑ}���i�w��ʒu�j�i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::insertRecordScp(DataScpRecord &dt, Nsc nsc){
	if (nsc >= 0 && nsc <= sizeDataScp()){
		m_scp.insert(m_scp.begin()+nsc, dt);
	}
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʎ擾�i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::getRecordLogo(DataLogoRecord &dt, Nsc nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		dt = m_logo[nlg];
	}
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʕύX�i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::setRecordLogo(DataLogoRecord &dt, Nlg nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		m_logo[nlg] = dt;
	}
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʎ擾�i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::getRecordScp(DataScpRecord &dt, Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		dt = m_scp[nsc];
	}
}

//---------------------------------------------------------------------
// �P�f�[�^�Z�b�g�P�ʕύX�i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::setRecordScp(DataScpRecord &dt, Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc] = dt;
	}
}



//=====================================================================
// �P�v�f�P�ʂ̏���
//=====================================================================

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i���S�f�[�^�����オ��j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecLogoRise(Nlg nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		return m_logo[nlg].rise;
	}
	return 0;
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i���S�f�[�^����������j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecLogoFall(Nlg nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		return m_logo[nlg].fall;
	}
	return 0;
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i���S�f�[�^�G�b�W�j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecLogoNrf(Nrf nrf){
	Nlg n = nrf/2;
	if (n >= 0 && n < sizeDataLogo()){
		if (nrf%2 == 0){
			return m_logo[n].rise;
		}
		else{
			return m_logo[n].fall;
		}
	}
	return 0;
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i�s���m�̈���܂߂����S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::getMsecLogoNrfWide(int &msec_c, int &msec_l, int &msec_r, Nrf nrf){
	Nlg n = nrf/2;
	if (n >= 0 && n < sizeDataLogo()){
		if (nrf%2 == 0){
			msec_c = m_logo[n].rise;
			msec_l = m_logo[n].rise_l;
			msec_r = m_logo[n].rise_r;
		}
		else{
			msec_c = m_logo[n].fall;
			msec_l = m_logo[n].fall_l;
			msec_r = m_logo[n].fall_r;
		}
	}
	else{
		msec_c = 0;
		msec_l = 0;
		msec_r = 0;
	}
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i�s���m�̈���܂߂����S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::getWideMsecLogoNrf(WideMsec &wmsec, Nrf nrf){
	Nlg n = nrf/2;
	if (n >= 0 && n < sizeDataLogo()){
		if (nrf%2 == 0){
			wmsec.just  = m_logo[n].rise;
			wmsec.early = m_logo[n].rise_l;
			wmsec.late  = m_logo[n].rise_r;
		}
		else{
			wmsec.just  = m_logo[n].fall;
			wmsec.early = m_logo[n].fall_l;
			wmsec.late  = m_logo[n].fall_r;
		}
	}
	else{
		wmsec.just  = 0;
		wmsec.early = 0;
		wmsec.late  = 0;
	}
}

//---------------------------------------------------------------------
// �~���b�f�[�^�ύX�i���S�f�[�^�G�b�W�j
//---------------------------------------------------------------------
void JlsDataset::setMsecLogoNrf(Nrf nrf, Msec val){
	int n = nrf/2;
	if (n >= 0 && n < sizeDataLogo()){
		if (nrf%2 == 0){
			m_logo[n].rise = val;
			if (m_logo[n].rise_l > val){
				m_logo[n].rise_l = val;
			}
			if (m_logo[n].rise_r < val){
				m_logo[n].rise_r = val;
			}
		}
		else{
			m_logo[n].fall = val;
			if (m_logo[n].fall_l > val){
				m_logo[n].fall_l = val;
			}
			if (m_logo[n].fall_r < val){
				m_logo[n].fall_r = val;
			}
		}
	}
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i����SC�ʒu�j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecScp(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].msec;
	}
	return 0;
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i����SC ���O�I���ʒu�j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecScpBk(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].msbk;
	}
	return 0;
}

//---------------------------------------------------------------------
// �~���b�f�[�^�擾�i����SC �J�n�^���O�I���ʒu�̑I��t���j
//---------------------------------------------------------------------
Msec JlsDataset::getMsecScpEdge(Nsc nsc, LogoEdgeType edge){
	if (edge == LOGO_EDGE_FALL){
		return getMsecScpBk(nsc);
	}
	return getMsecScp(nsc);
}

//---------------------------------------------------------------------
// �͈̓~���b�f�[�^�擾�i�͈͖����V�[���`�F���W�ԍ�����j
//---------------------------------------------------------------------
RangeMsec JlsDataset::getRangeMsecFromRangeNsc(RangeNsc rnsc){
	RangeMsec rmsec;
	rmsec.st = pdata->getMsecScp(rnsc.st);
	rmsec.ed = pdata->getMsecScp(rnsc.ed);
	if (rnsc.st < 0) rmsec.st = -1;
	if (rnsc.ed < 0) rmsec.ed = -1;
	return rmsec;
}

//---------------------------------------------------------------------
// �摜�ω��Ȃ��t���O�擾
//---------------------------------------------------------------------
bool JlsDataset::getScpStill(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].still;
	}
	return false;
}

//---------------------------------------------------------------------
// ��؂��Ԏ擾
//---------------------------------------------------------------------
jlsd::ScpPriorType JlsDataset::getScpStatpos(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].statpos;
	}
	return SCP_PRIOR_NONE;
}

//---------------------------------------------------------------------
// �\�������p �X�R�A�擾
//---------------------------------------------------------------------
int JlsDataset::getScpScore(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].score;
	}
	return 0;
}

//---------------------------------------------------------------------
// �\�������p ��؂��Ԏ擾
//---------------------------------------------------------------------
jlsd::ScpChapType JlsDataset::getScpChap(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].chap;
	}
	return SCP_CHAP_NONE;
}

//---------------------------------------------------------------------
// �\�������p �\�����e�擾
//---------------------------------------------------------------------
jlsd::ScpArType JlsDataset::getScpArstat(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].arstat;
	}
	return SCP_AR_UNKNOWN;
}

//---------------------------------------------------------------------
// �\�������p �\�����e�擾
//---------------------------------------------------------------------
jlsd::ScpArExtType JlsDataset::getScpArext(Nsc nsc){
	if (nsc >= 0 && nsc < sizeDataScp()){
		return m_scp[nsc].arext;
	}
	return SCP_AREXT_NONE;
}

//---------------------------------------------------------------------
// ���S���ʎ擾
//---------------------------------------------------------------------
bool JlsDataset::getResultLogoAtNrf(Msec &msec, LogoResultType &outtype, Nrf nrf){
	if (nrf >= 0 && nrf/2 < sizeDataLogo()){
		if (nrf%2 == 0){
			msec = m_logo[nrf/2].result_rise;
			outtype = m_logo[nrf/2].outtype_rise;
		}
		else{
			msec = m_logo[nrf/2].result_fall;
			outtype = m_logo[nrf/2].outtype_fall;
		}
		return true;
	}
	return false;
}

//---------------------------------------------------------------------
// ��؂��Ԑݒ�
//---------------------------------------------------------------------
void JlsDataset::setScpStatpos(Nsc nsc, ScpPriorType val){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc].statpos = val;
	}
}

//---------------------------------------------------------------------
// �\�������p �X�R�A�ݒ�
//---------------------------------------------------------------------
void JlsDataset::setScpScore(Nsc nsc, int val){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc].score = val;
	}
}

//---------------------------------------------------------------------
// �\�������p ��؂��Ԑݒ�
//---------------------------------------------------------------------
void JlsDataset::setScpChap(Nsc nsc, ScpChapType val){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc].chap = val;
	}
}

//---------------------------------------------------------------------
// �\�������p �\�����e�ݒ�
//---------------------------------------------------------------------
void JlsDataset::setScpArstat(Nsc nsc, ScpArType val){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc].arstat = val;
		m_scp[nsc].arext  = SCP_AREXT_NONE;		// �g��������������
	}
}

//---------------------------------------------------------------------
// �\�������p �\�����e�g���ݒ�
//---------------------------------------------------------------------
void JlsDataset::setScpArext(Nsc nsc, ScpArExtType val){
	if (nsc >= 0 && nsc < sizeDataScp()){
		m_scp[nsc].arext = val;
	}
}


//---------------------------------------------------------------------
// ���S���ʐݒ�
//---------------------------------------------------------------------
void JlsDataset::setResultLogoAtNrf(Msec msec, LogoResultType outtype, Nrf nrf){
	if (nrf >= 0 && nrf/2 < sizeDataLogo()){
		if (nrf%2 == 0){
			m_logo[nrf/2].result_rise = msec;
			m_logo[nrf/2].outtype_rise = outtype;
		}
		else{
			m_logo[nrf/2].result_fall = msec;
			m_logo[nrf/2].outtype_fall = outtype;
		}
	}
}



//=====================================================================
// �D��x�擾����
//=====================================================================

//---------------------------------------------------------------------
// ���S�f�[�^�̌��Ƃ��ėD��x�擾
//---------------------------------------------------------------------
jlsd::LogoPriorType JlsDataset::getPriorLogo(Nrf nrf){
	int n = nrf/2;
	if (n >= 0 && n < sizeDataLogo()){
		if (nrf%2 == 0){
			return m_logo[n].stat_rise;
		}
		else{
			return m_logo[n].stat_fall;
		}
	}
	return LOGO_PRIOR_NONE;
}

//---------------------------------------------------------------------
// ����SC�̌��Ƃ��ėD��x�擾
// Auto�R�}���h�ɂ�鐄���L���Ŏ擾�f�[�^��ύX
//---------------------------------------------------------------------
jlsd::ScpPriorType JlsDataset::getPriorScp(Nsc nsc){
	if (nsc >= 0 && nsc < (int) m_scp.size()){
		if (m_flagSetupAuto <= 1){
			return m_scp[nsc].statpos;
		}
		else{
			if (m_scp[nsc].chap >= SCP_CHAP_DECIDE){
				return SCP_PRIOR_DECIDE;
			}
			else if (m_scp[nsc].chap > SCP_CHAP_NONE){
				return SCP_PRIOR_LV1;
			}
			else if (m_scp[nsc].chap < SCP_CHAP_NONE){
				return SCP_PRIOR_DUPE;
			}
		}
	}
	return SCP_PRIOR_NONE;
}



//=====================================================================
// �O��f�[�^�擾�����i���S�j
//=====================================================================

//---------------------------------------------------------------------
// ���̃��S�ʒu�擾
// ���́F
//   nrf  : ���S�ԍ�*2 + fall����1
//   dr   : ���������i�O�� / �㑤�j
//   edge : ���S�[�i0:�����オ��G�b�W  1:����������G�b�W  2:���G�b�W�j
//   type : 0:���ׂ�  1:�L�����S
// �Ԃ�l�F ���̃��S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
Nrf JlsDataset::getNrfDirLogo(Nrf nrf, SearchDirType dr, LogoEdgeType edge, LogoSelectType type){
	int size_logo = sizeDataLogo();
	int r = -1;
	bool flag_end = false;
	bool check_rise = isLogoEdgeRise(edge);
	bool check_fall = isLogoEdgeFall(edge);
	int i = nrf;
	int step = (dr == SEARCH_DIR_NEXT)? +1 : -1;
	while(flag_end == false){
		i += step;
		if (i >= 0 && i < size_logo*2){
			int nlg_i  = nlgFromNrf(i);
			int edge_i = edgeFromNrf(i);
			if (edge_i == LOGO_EDGE_RISE && check_rise){
				if (isValidLogoRise(nlg_i) || type == LOGO_SELECT_ALL){
					r = i;
					flag_end = true;
				}
			}
			else if (edge_i == LOGO_EDGE_FALL && check_fall){
				if (isValidLogoFall(nlg_i) || type == LOGO_SELECT_ALL){
					r = i;
					flag_end = true;
				}
			}
		}
		else{
			flag_end = true;
		}
	};
	return r;
}

// �P�O�̃��S�ʒu�擾
Nrf JlsDataset::getNrfPrevLogo(Nrf nrf, LogoEdgeType edge, LogoSelectType type){
	return getNrfDirLogo(nrf, SEARCH_DIR_PREV, edge, type);
}

// �P��̃��S�ʒu�擾
Nrf JlsDataset::getNrfNextLogo(Nrf nrf, LogoEdgeType edge, LogoSelectType type){
	return getNrfDirLogo(nrf, SEARCH_DIR_NEXT, edge, type);
}

//---------------------------------------------------------------------
// ���̃��S�ʒu�擾�i�������Ɨ�������̃Z�b�g�j
// ���́F
//   nfall : ���S�ԍ�*2 + fall����1
//   type  : 0:���ׂ�  1:�L�����S
// �Ԃ�l�F ���S�ʒu�擾���ʁi�擾��:true�j
//   nrise : ���̗���胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//   nfall : ���̗����胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
bool JlsDataset::getNrfNextLogoSet(Nrf &nrf_rise, Nrf &nrf_fall, LogoSelectType type){
	nrf_rise = getNrfNextLogo(nrf_fall, LOGO_EDGE_RISE, type);
	if (nrf_rise >= 0){
		nrf_fall = getNrfNextLogo(nrf_rise, LOGO_EDGE_FALL, type);
	}
	else{
		nrf_fall = -1;
	}
	if (nrf_fall < 0) return false;
	return true;
}


//---------------------------------------------------------------------
// ���̐����\�����S�����ʒu�擾�i�ŏI�o�͔�����͂��j
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   dr   : ���������i�O�� / �㑤�j
//   edge : ���S�[�i0:�����オ��G�b�W  1:����������G�b�W  2:���G�b�W�j
//   flag_border : Border�̈�����S�Ɋ܂߂�
//   flag_out    : �o�͗p�i0:�����\�z�p�\��  1:�o�͗p�\���j
// �Ԃ�l�F ���̃��S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscDirElgForAll(Nsc nsc, SearchDirType dr, LogoEdgeType edge, bool flag_border, bool flag_out){
	int size_scp = sizeDataScp();
	int r = -1;
	int i = nsc;
	int inext = getNscDirScpDecide(i, dr, SCP_END_EDGEIN);
	if (dr == SEARCH_DIR_NEXT){			// arstat�͂Q�_�Ԃ̌㑤�����邽�߂̈ړ�
		i = getNscNextScpDecide(i,  SCP_END_EDGEIN);
	}
	//--- �t�G�b�W�����܂ŒT�� ---
	bool inlogo_base = isElgInScpForAll(i, flag_border, flag_out);
	while(((edge == LOGO_EDGE_RISE && dr == SEARCH_DIR_NEXT && inlogo_base == true) ||
		   (edge == LOGO_EDGE_RISE && dr == SEARCH_DIR_PREV && inlogo_base == false) ||
		   (edge == LOGO_EDGE_FALL && dr == SEARCH_DIR_NEXT && inlogo_base == false) ||
		   (edge == LOGO_EDGE_FALL && dr == SEARCH_DIR_PREV && inlogo_base == true))
		  && (getScpChap(inext) != SCP_CHAP_DUNIT)
		  && i >= 0){
		i = getNscDirScpDecide(i, dr, SCP_END_EDGEIN);
		inlogo_base = isElgInScpForAll(i, flag_border, flag_out);
	}
	//--- �G�b�W������T�� ---
	if ((i > 0 && i < size_scp) ||
		(i == 0 && dr == SEARCH_DIR_NEXT) ||
		(i == size_scp && dr == SEARCH_DIR_PREV)){
		bool flag_end = false;
		while(flag_end == false){
			int ilast = i;
			i = getNscDirScpDecide(i, dr, SCP_END_EDGEIN);
			if (i > 0 && i < size_scp){
				bool inlogo_i = isElgInScpForAll(i, flag_border, flag_out);
				int  iset = i;
				if (dr == SEARCH_DIR_NEXT){
					iset = ilast;					// �Q�_�Ԃ̑O��
				}
				if (inlogo_i != inlogo_base){		// �G�b�W�ω�����Ίm��
					flag_end = true;
					r = iset;
				}
				else if (getScpChap(iset) == SCP_CHAP_DUNIT){	// ������؂�
					flag_end = true;
					r = iset;
				}
			}
			else{
				flag_end = true;
				if (inlogo_base == true){			// ���S���ŏI��������[�ݒ�
					if (dr == SEARCH_DIR_NEXT){
						r = size_scp-1;
					}
					else{
						r = 0;
					}
				}
			}
		}
	};
	return r;
}


//---------------------------------------------------------------------
// ���̐����\�����S�����ʒu�擾
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   edge : ���S�[�i0:�����オ��G�b�W  1:����������G�b�W  2:���G�b�W�j
// �Ԃ�l�F ���̃��S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
// �P�O�̈ʒu�擾
Nsc JlsDataset::getNscPrevElg(Nsc nsc, LogoEdgeType edge){
	return getNscDirElgForAll(nsc, SEARCH_DIR_PREV, edge, false, false);
}

// �P��̈ʒu�擾
Nsc JlsDataset::getNscNextElg(Nsc nsc, LogoEdgeType edge){
	return getNscDirElgForAll(nsc, SEARCH_DIR_NEXT, edge, false, false);
}

//---------------------------------------------------------------------
// ���̃��S�ʒu�擾�i�������Ɨ�������̃Z�b�g�j
// ���́F
//   nrf_fall : ���S�ԍ�*2 + fall����1
//   type     : 0:���ׂ�  1:�L�����S
// �Ԃ�l�F ���S�ʒu�擾���ʁi�擾��:true�j
//   nrf_rise : ���̗���胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//   nrf_fall : ���̗����胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
bool JlsDataset::getNrfptNext(NrfCurrent &logopt, LogoSelectType type){
	//--- initialize ---
	if (logopt.valid == false){
		logopt.nrfRise = -1;
		logopt.nrfFall = -1;
		logopt.valid = true;
	}
	//--- backup ---
	logopt.nrfLastRise = logopt.nrfRise;
	logopt.nrfLastFall = logopt.nrfFall;
	logopt.msecLastRise = logopt.msecRise;
	logopt.msecLastFall = logopt.msecFall;
	//--- rise ---
	logopt.nrfRise = getNrfNextLogo(logopt.nrfFall, LOGO_EDGE_RISE, type);
	if (logopt.nrfRise >= 0){
		logopt.nrfFall = getNrfNextLogo(logopt.nrfRise, LOGO_EDGE_FALL, type);
	}
	else{
		logopt.nrfFall = -1;
	}
	logopt.msecRise = getMsecLogoNrf(logopt.nrfRise);
	logopt.msecFall = getMsecLogoNrf(logopt.nrfFall);
	if (logopt.nrfFall < 0) return false;
	return true;
}
	


//---------------------------------------------------------------------
// ���̐����\�����S�����ʒu�擾�i�������Ɨ�������̃Z�b�g�j
// ���́F
//   nsc_fall : �O�̗���������V�[���`�F���W�ԍ�
// �Ԃ�l�F ���S�ʒu�擾���ʁi�擾��:true�j
//   nsc_rise : ���̗���胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//   nsc_fall : ���̗����胍�S�ʒu�i�Ȃ��ꍇ��-1�j
//---------------------------------------------------------------------
bool JlsDataset::getElgptNext(ElgCurrent &elg){
	bool flag1st = false;
	//--- initialize ---
	if (elg.valid == false){
		elg.nscFall = 0;
		elg.valid = true;
		flag1st = true;
	}
	//--- backup ---
	elg.nscLastRise  = elg.nscRise;
	elg.nscLastFall  = elg.nscFall;
	elg.msecLastRise = elg.msecRise;
	elg.msecLastFall = elg.msecFall;
	//--- rise ---
	if (isSameLocNextElg(elg.nscFall, LOGO_EDGE_RISE) == false){
		if (flag1st) elg.nscFall = -1;		// 0�ʒu����`�F�b�N���邽��
		elg.nscRise = getNscDirElgForAll(elg.nscFall, SEARCH_DIR_NEXT, LOGO_EDGE_RISE, elg.border, elg.outflag);
	}
	else{
		elg.nscRise = elg.nscFall;
	}
	elg.msecRise = getMsecScp(elg.nscRise);
	//--- fall ---
	if (elg.nscRise >= 0){
		elg.nscFall = getNscDirElgForAll(elg.nscRise, SEARCH_DIR_NEXT, LOGO_EDGE_FALL, elg.border, elg.outflag);
	}
	else{
		elg.nscFall = -1;
	}
	if (elg.outflag){					// �ŏI�o�͎��͐��m�ɗ���������ʒu
		elg.msecFall = getMsecScpBk(elg.nscFall);
	}
	else{								// �ʏ�̓V�[���`�F���W�ԍ��ŋ��ʈʒu�F��
		elg.msecFall = getMsecScp(elg.nscFall);
	}
	//--- end ---
	if (elg.nscFall < 0) return false;
	return true;
}

//---------------------------------------------------------------------
// ���S�̌��ʈʒu�i���̈ʒu�j���擾
// ���́F
//   nlg    : �������J�n���郍�S�ԍ��i�w��ʒu���܂ށj
// �o�́F
//   �Ԃ�l�F�擾�������S�̎��̃��S�ԍ��i-1�̎��͊Y���Ȃ��j
//   msec_rise : �����オ��ʒu�i�~���b�j
//   msec_rise : ����������ʒu�i�~���b�j
//   cont_next : ���̃��S���؂�ڂȂ��̘A����
//---------------------------------------------------------------------
Nlg JlsDataset::getResultLogoNext(Msec &msec_rise, Msec &msec_fall, bool &cont_next, Nlg nlg){
	int msec_val_cont = 80;					// ���ꃍ�S�Ƃ݂Ȃ��Ԋumsec

	int size_logo = sizeDataLogo();
	cont_next = false;
	msec_rise = -1;
	msec_fall = -1;
	//--- �����オ��G�b�W������ ---
	Nlg nlg_rise = nlg;
	bool flag_rise = false;
	while(nlg_rise >= 0 && nlg_rise < size_logo && flag_rise == false){
		DataLogoRecord dtlogo;
		getRecordLogo(dtlogo, nlg_rise);
		if (dtlogo.outtype_rise == LOGO_RESULT_DECIDE){				// �m��G�b�W
			flag_rise = true;
			msec_rise = dtlogo.result_rise;
		}
		if (flag_rise == false){
			nlg_rise ++;
		}
	}
	//--- ����������G�b�W������ ---
	Nlg nlg_fall = nlg_rise;
	bool flag_fall = false;
	bool flag_unit = false;
	if (flag_rise == false){
		nlg_fall = nlg;
	}
	while(nlg_fall >= 0 && nlg_fall < size_logo && flag_fall == false){
		DataLogoRecord dtlogo;
		getRecordLogo(dtlogo, nlg_fall);
		if (dtlogo.outtype_fall == LOGO_RESULT_DECIDE){				// �m��G�b�W
			flag_fall = true;
			msec_fall = dtlogo.result_fall;
			if (dtlogo.unit){
				flag_unit = true;
			}
		}
		if (flag_fall == false){
			nlg_fall ++;
		}
	}
	//--- ���̃��S�����ꃍ�S���m�F ---
	if (flag_fall){
		Nlg  nlg_next = nlg_fall + 1;
		bool flag_next = false;
		while(nlg_next >= 0 && nlg_next < size_logo && flag_next == false){
			DataLogoRecord dtlogo;
			getRecordLogo(dtlogo, nlg_next);
			if (dtlogo.outtype_rise == LOGO_RESULT_DECIDE){				// �m��G�b�W
				flag_next = true;
				int msec_next = dtlogo.result_rise;
				if (msec_fall + msec_val_cont > msec_next){				// ���ꃍ�S�ŕ␳����
					msec_fall = cnv.getMsecAdjustFrmFromMsec(msec_next, -1);
					if (flag_unit == false){					// �Ɨ����S�łȂ���ΐ؂�ڂȂ�����
						cont_next = true;
					}
				}
			}
			nlg_next ++;
		}
	}
	//--- ���� ---
	Nlg nlg_ret = -1;
	//--- ����擾�� ---
	if (flag_rise == true && flag_fall == true){
		nlg_ret = nlg_fall + 1;
	}
	else{
		//--- ���������肪�Ȃ��ꍇ ---
		if ((flag_rise == true || nlg == 0) && flag_fall == false){
			nlg_ret = size_logo;
			msec_fall = getMsecTotalMax();
		}
		//--- �ŏ����烍�S���S���Ȃ��ꍇ ---
		if (nlg == 0){
			msec_rise = 0;
		}
	}
	return nlg_ret;
}



//=====================================================================
// �O��f�[�^�擾�����i�����V�[���`�F���W�j
//=====================================================================

//---------------------------------------------------------------------
// ���̍\���ʒu���擾
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   dr   : ���������i�O�� / �㑤�j
//   chap_th : ��؂�Ƃ�����臒l
// �o�́F
//   �Ԃ�l�F ��O�\����؂�̈ʒu�ԍ��i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscDirScpChap(Nsc nsc, SearchDirType dr, ScpChapType chap_th){
	if (dr == SEARCH_DIR_PREV){
		return getNscPrevScpChap(nsc, chap_th);
	}
	return getNscNextScpChap(nsc, chap_th);
}

// �P�O�̈ʒu�擾
Nsc JlsDataset::getNscPrevScpChap(Nsc nsc, ScpChapType chap_th){
	int num_scpos = sizeDataScp();
	int r = -1;
	if (nsc-1 < num_scpos){
		for(int i=nsc-1; i>0; i--){
			ScpChapType chap_i = m_scp[i].chap;
			if (chap_i >= chap_th){
				r = i;
				break;
			}
		}
	}
	return r;
}

// �P��̈ʒu�擾
Nsc JlsDataset::getNscNextScpChap(Nsc nsc, ScpChapType chap_th){
	int num_scpos = sizeDataScp();
	int r = -1;
	if (nsc+1 >= 0){
		for(int i=nsc+1; i<num_scpos-1; i++){
			ScpChapType chap_i = m_scp[i].chap;
			if (chap_i >= chap_th){
				r = i;
				break;
			}
		}
	}
	return r;
}

// �P��̈ʒu�擾
Nsc JlsDataset::getNscNextScpChapEdge(Nsc nsc, ScpChapType chap_th, ScpEndType noedge){
	int num_scpos = sizeDataScp();
	int r = -1;
	if (nsc+1 >= 0){
		for(int i=nsc+1; i<num_scpos-noedge; i++){
			ScpChapType chap_i = m_scp[i].chap;
			if (chap_i >= chap_th || i == 0 || i == num_scpos-1){
				r = i;
				break;
			}
		}
	}
	return r;
}

//---------------------------------------------------------------------
// ���̍\���ʒu���擾�i2�\���ȏ��CM�͌����j
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   noedge : 0�t���[���ƍŏI�t���[���������ݒ�
// �o�́F
//   �Ԃ�l�F ���̍\����؂�ʒu�ԍ��i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscNextScpCheckCmUnit(Nsc nsc, ScpEndType noedge){
	int nsc_next = nsc;
	int nsc_cur;
	ScpArType arstat = SCP_AR_UNKNOWN;
	//--- 2�\���ȏ��CM�͍Ō�̈ʒu�܂ňړ� ---
	do{
		nsc_cur  = nsc_next;
		nsc_next = getNscNextScpChapEdge(nsc_cur, SCP_CHAP_DECIDE, noedge);
		arstat = getScpArstat(nsc_next);
	} while(nsc_next > 0 &&
			(arstat == SCP_AR_N_AUNIT ||
			(arstat == SCP_AR_N_BUNIT && nsc_cur == nsc)));
	//--- ���̈ʒu�ɐݒ� ---
	if (nsc_cur == nsc){
		nsc_cur = nsc_next;
	}
	return nsc_cur;
}

//---------------------------------------------------------------------
// ���̍\���ʒu���擾
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   dr   : ���������i�O�� / �㑤�j
//   noedge : 0�t���[���ƍŏI�t���[���������ݒ�
// �o�́F
//   �Ԃ�l�F ���̍\����؂�ʒu�ԍ��i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscDirScpDecide(Nsc nsc, SearchDirType dr, ScpEndType noedge){
	if (dr == SEARCH_DIR_PREV){
		return getNscPrevScpDecide(nsc, noedge);
	}
	return getNscNextScpDecide(nsc, noedge);
}

// �P�O�̈ʒu�擾
int JlsDataset::getNscPrevScpDecide(int nsc, ScpEndType noedge){
	int num_scpos = sizeDataScp();
	int r = -1;
	int nstart = nsc-1;
	if (nstart == num_scpos-1 && noedge == SCP_END_NOEDGE) nstart = num_scpos-2;
	for(int i=nstart; i>=noedge; i--){
		if (getPriorScp(i) >= SCP_PRIOR_DECIDE || i == 0 || i == num_scpos-1){
			r = i;
			break;
		}
	}
	return r;
}

// �P��̈ʒu�擾
Nsc JlsDataset::getNscNextScpDecide(Nsc nsc, ScpEndType noedge){
	int num_scpos = sizeDataScp();
	int r = -1;
	int nstart = nsc+1;
	if (nstart == 0 && noedge == SCP_END_NOEDGE) nstart = 1;
	for(int i=nstart; i<num_scpos-noedge; i++){
		if (getPriorScp(i) >= SCP_PRIOR_DECIDE || i == 0 || i == num_scpos-1){
			r = i;
			break;
		}
	}
	return r;
}

// �ŏI�o�͗p
Nsc JlsDataset::getNscNextScpOutput(Nsc nsc, ScpEndType noedge){
	int r = -1;
	if ( isAutoModeUse() ){
		r = getNscNextScpCheckCmUnit(nsc, noedge);
	}
	else{
		r = getNscNextScpDecide(nsc, noedge);
	}
	return r;
}



//=====================================================================
// �ʒu�ɑΉ�����f�[�^�擾����
//=====================================================================

//---------------------------------------------------------------------
// �Ώۈʒu���܂܂�郍�S�ԍ����擾
// ���́F
//   msec_target : �Ώۈʒu
//   edge        : 0=�����オ��G�b�W  1=����������G�b�W  2=���G�b�W
// �o�́F
//   �Ԃ�l�F ���S�ԍ�*2 + fall����1�i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nrf JlsDataset::getNrfLogoFromMsec(Msec msec_target, LogoEdgeType edge){
	int size_logo = sizeDataLogo();
	int r = -1;
	for(int i=0; i<size_logo; i++){
		if ( isLogoEdgeRise(edge) ){
			int msec_s = m_logo[i].rise_l;
			int msec_e = m_logo[i].rise_r;
			if (msec_s <= msec_target && msec_target <= msec_e){
				r = i*2;
				break;
			}
		}
		if ( isLogoEdgeFall(edge) ){
			int msec_s = m_logo[i].fall_l;
			int msec_e = m_logo[i].fall_r;
			if (msec_s <= msec_target && msec_target <= msec_e){
				r = i*2+1;
				break;
			}
		}
	}
	return r;
}

//---------------------------------------------------------------------
// ���S�؂�ւ��ʒu�̖����V�[���`�F���W�擾
// ���́F
//   nrf_target   : �Ώۃ��S�ԍ�
//   msec_th      : �����͈́i-1�̎��͐����Ȃ��j
//   chap_th      : �ΏۂƂ���\����؂���臒l
// �o�́F
//   �Ԃ�l�F �Ώۈʒu�Ɉ�ԋ߂��\����؂�̈ʒu�ԍ��i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscFromNrf(Nrf nrf_target, Msec msec_th, ScpChapType chap_th, bool flat){
	if (nrf_target < 0 || nrf_target >= sizeDataLogo()*2){
		return -1;
	}
	//--- ���S�\���͈͎擾 ---
	int msec_lg_c = -1;
	int msec_lg_l = -1;
	int msec_lg_r = -1;;
	getMsecLogoNrfWide(msec_lg_c, msec_lg_l, msec_lg_r, nrf_target);
	//--- ��ԋ߂��ʒu�̎擾 ---
	int nsc_det  = -1;
	int msec_dif_det = -1;
	ScpChapType chap_det;
	int i = 0;
	bool over = false;
	do{
		i = getNscNextScpChap(i, chap_th);
		if (i > 0){
			int msec_i = getMsecScp(i);
			ScpChapType chap_i = getScpChap(i);
			if (flat){
				chap_i = SCP_CHAP_NONE;
			}
			else if (chap_i >= SCP_CHAP_DECIDE){		// �m��Ȃ瓯�����x���ɐݒ�
				chap_i = SCP_CHAP_DECIDE;
			}
			if (msec_i > msec_lg_r + msecValNear3){
				over = true;
			}
			else if (msec_i >= msec_lg_l - msecValNear3){
				int msec_dif_i = abs(msec_i - msec_lg_c);
				if (msec_dif_i <= msec_th || msec_th < 0){
					if (nsc_det < 0 ||
						chap_i > chap_det ||
						(chap_i == chap_det && msec_dif_i < msec_dif_det)){
						nsc_det = i;
						msec_dif_det = msec_dif_i;
						chap_det = chap_i;
					}
				}
			}
		}
	}while(i > 0 && over == false);
	return nsc_det;
}

//---------------------------------------------------------------------
// �Ώۈʒu�Ɉ�ԋ߂��ʒu������
// ���́F
//   msec_target  : �Ώۈʒu
//   msec_th      : �����͈́i-1�̎��͐����Ȃ��j
//   chap_th      : �ΏۂƂ���\����؂���臒l
//   noedge       : 0�t���[���ƍŏI�t���[���������ݒ�
// �o�́F
//   �Ԃ�l�F �Ώۈʒu�Ɉ�ԋ߂��\����؂�̈ʒu�ԍ��i-1�̎��͊Y���Ȃ��j
//---------------------------------------------------------------------
Nsc JlsDataset::getNscFromMsecFull(Msec msec_target, Msec msec_th, ScpChapType chap_th, ScpEndType noedge){
	int num_scpos = sizeDataScp();
	int r = -1;
	int msec_min = -1;
	for(int i=noedge; i<num_scpos-noedge; i++){
		int msec_i = m_scp[i].msec;
		ScpChapType chap_i = m_scp[i].chap;
		if (chap_i >= chap_th || i == 0 || i == num_scpos-1){
			//--- �Ώۈʒu����̍����ŏ��l�ӏ����擾 ---
			int msec_dif = abs(msec_target - msec_i);
			if (msec_dif <= msec_th || msec_th < 0){
				if (msec_dif < msec_min || msec_min < 0){
					r = i;
					msec_min = msec_dif;
				}
			}
			//--- �Ώۈʒu���߂�����I�� ---
			if (msec_i >= msec_target){
				break;
			}
		}
	}
	return r;
}

// �\����؂���擾
Nsc JlsDataset::getNscFromMsecChap(Msec msec_target, Msec msec_th, ScpChapType chap_th){
	return getNscFromMsecFull(msec_target, msec_th, chap_th, SCP_END_NOEDGE);
}

// ����SC�ʒu���擾
Nsc JlsDataset::getNscFromMsecAll(Msec msec_target){
	return getNscFromMsecFull(msec_target, msecValNear2, SCP_CHAP_DUPE, SCP_END_NOEDGE);
}

// ����SC�ʒu���擾
Nsc JlsDataset::getNscFromMsecAllEdgein(Msec msec_target){
	return getNscFromMsecFull(msec_target, msecValNear2, SCP_CHAP_DUPE, SCP_END_EDGEIN);
}

// �͈͓��̖���SC�ʒu���擾
Nsc JlsDataset::getNscFromWideMsecFull(WideMsec wmsec_target, ScpChapType chap_th, ScpEndType noedge){
	int num_scpos = sizeDataScp();
	int r = -1;
	Msec msec_min = -1;
	for(int i=noedge; i<num_scpos-noedge; i++){
		Msec msec_i = m_scp[i].msec;
		ScpChapType chap_i = m_scp[i].chap;
		if (chap_i >= chap_th || i == 0 || i == num_scpos-1){
			//--- �Ώۈʒu����̍����ŏ��l�ӏ����擾 ---
			int msec_dif = abs(wmsec_target.just - msec_i);
			if ((wmsec_target.early <= msec_i || wmsec_target.early < 0) &&
				(msec_i <= wmsec_target.late  || wmsec_target.late  < 0)){
				if (msec_dif < msec_min || msec_min < 0){
					r = i;
					msec_min = msec_dif;
				}
			}
			//--- �Ώۈʒu���߂�����I�� ---
			if (msec_i >= wmsec_target.late){
				break;
			}
		}
	}
	return r;
}

// �͈͓��̖���SC�ʒu���擾
Nsc JlsDataset::getNscFromWideMsecByChap(WideMsec wmsec_target, ScpChapType chap_th){
	return getNscFromWideMsecFull(wmsec_target, chap_th, SCP_END_NOEDGE);
}


//---------------------------------------------------------------------
// �J�n�I���ʒu�ɑΉ����閳���V�[���`�F���W�ԍ����擾
//---------------------------------------------------------------------
bool JlsDataset::getRangeNscFromRangeMsec(RangeNsc &rnsc, RangeMsec rmsec){
	//--- �ʒu���琳�m�ȃ��S�ԍ����擾���� ---
	bool det = true;
	rnsc.st = -1;
	rnsc.ed = -1;

	if (rmsec.st >= 0){
		rnsc.st = getNscFromMsecChap(rmsec.st, pdata->msecValLap2, SCP_CHAP_DECIDE);
		if (rnsc.st < 0){
			det = false;
		}
	}
	if (rmsec.ed >= 0){
		rnsc.ed = getNscFromMsecChap(rmsec.ed, pdata->msecValLap2, SCP_CHAP_DECIDE);
		if (rnsc.ed < 0){
			det = false;
		}
	}
	return det;
}





//=====================================================================
// ��Ԑݒ�
//=====================================================================

//---------------------------------------------------------------------
// ���S���x���ݒ�E�ǂݏo��
//---------------------------------------------------------------------
void JlsDataset::setLevelUseLogo(int level){
	m_levelUseLogo = level;
}
int JlsDataset::getLevelUseLogo(){
	return m_levelUseLogo;
}

//---------------------------------------------------------------------
// �]������K�v�ȏ�������
//---------------------------------------------------------------------
void JlsDataset::setFlagSetupAdj(bool flag){
	m_flagSetupAdj = flag;
}

//---------------------------------------------------------------------
// �����\�������̃��[�h�ݒ�
//---------------------------------------------------------------------
void JlsDataset::setFlagAutoMode(bool flag){
	if (flag){
		m_flagSetupAuto = 2;
	}
	else{
		m_flagSetupAuto = 1;
	}
}



//=====================================================================
// ��Ԕ���
//=====================================================================

//---------------------------------------------------------------------
// �ǂݍ��񂾃��S�����݂��邩���f
//---------------------------------------------------------------------
bool JlsDataset::isExistLogo(){
	return (pdata->extOpt.flagNoLogo > 0)? false : true;
}

//---------------------------------------------------------------------
// ���S�������݂��邩���f
//---------------------------------------------------------------------
bool JlsDataset::isUnuseLogo(){
	return (m_levelUseLogo == CONFIG_LOGO_LEVEL_UNUSE_ALL)? true : false;
}
bool JlsDataset::isUnuseLevelLogo(){
	return (m_levelUseLogo < CONFIG_LOGO_LEVEL_USE_LOW)? true : false;
}

//---------------------------------------------------------------------
// Auto�\����������Ԃ��m�F
//---------------------------------------------------------------------
bool JlsDataset::isSetupAdjInitial(){
	return (m_flagSetupAdj == 0)? true : false;
}

//---------------------------------------------------------------------
// Auto�\����������Ԃ��m�F
//---------------------------------------------------------------------
bool JlsDataset::isAutoModeInitial(){
	return (m_flagSetupAuto == 0)? true : false;
}

//---------------------------------------------------------------------
// Auto�\�����g�p���郂�[�h���m�F
//---------------------------------------------------------------------
bool JlsDataset::isAutoModeUse(){
	return (m_flagSetupAuto == 2)? true : false;
}

//---------------------------------------------------------------------
// �����S���g�킸�\���������S���L���ȃ��[�h���m�F
//---------------------------------------------------------------------
bool JlsDataset::isAutoLogoOnly(){
	return (m_flagSetupAuto > 0 && isUnuseLevelLogo())? true : false;
}

//---------------------------------------------------------------------
// �Ώۈʒu�̃��S���L�������f�i���S�����オ��j
//---------------------------------------------------------------------
bool JlsDataset::isValidLogoRise(Nlg nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		if (m_logo[nlg].outtype_rise == LOGO_RESULT_NONE || m_logo[nlg].outtype_rise == LOGO_RESULT_DECIDE){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------
// �Ώۈʒu�̃��S���L�������f�i���S����������j
//---------------------------------------------------------------------
bool JlsDataset::isValidLogoFall(Nlg nlg){
	if (nlg >= 0 && nlg < sizeDataLogo()){
		if (m_logo[nlg].outtype_fall == LOGO_RESULT_NONE || m_logo[nlg].outtype_fall == LOGO_RESULT_DECIDE){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------
// �Ώۈʒu�̃��S���L�������f�i���S����������j
//---------------------------------------------------------------------
bool JlsDataset::isValidLogoNrf(Nrf nrf){
	Nlg nlg = nlgFromNrf(nrf);
	if (jlsd::isLogoEdgeRiseFromNrf(nrf)){
		return isValidLogoRise(nlg);
	}
	return isValidLogoFall(nlg);
}


//---------------------------------------------------------------------
// ���̐����\�����S�����ʒu���S�������ʒu�ɂȂ邩���f
// ���́F
//   nsc  : �V�[���`�F���W�ԍ�
//   dr   : ���������i�O�� / �㑤�j
//   edge : ���S�[�i0:�����オ��G�b�W  1:����������G�b�W  2:���G�b�W�j
// �Ԃ�l�F �S�������ʒu�ɂȂ邩�itrue=�����ʒu  false=�Ⴄ�ʒu�j
//---------------------------------------------------------------------
bool JlsDataset::isSameLocDirElg(Nsc nsc, SearchDirType dr, LogoEdgeType edge){
	//--- ������؂� ---
	if (getScpChap(nsc) == SCP_CHAP_DUNIT){
		if ((dr == SEARCH_DIR_PREV && edge == LOGO_EDGE_FALL) ||
			(dr == SEARCH_DIR_NEXT && edge == LOGO_EDGE_RISE)){
			return true;
		}
	}
	return false;
}

// �P�O�̈ʒu�擾
bool JlsDataset::isSameLocPrevElg(Nsc nsc, LogoEdgeType edge){
	return isSameLocDirElg(nsc, SEARCH_DIR_PREV, edge);
}

// �P��̈ʒu�擾
bool JlsDataset::isSameLocNextElg(Nsc nsc, LogoEdgeType edge){
	return isSameLocDirElg(nsc, SEARCH_DIR_NEXT, edge);
}

//---------------------------------------------------------------------
// ���O�������\���̃��S�����\�������f�i�����\�z�p�j
//---------------------------------------------------------------------
bool JlsDataset::isElgInScp(Nsc nsc){
	return isElgInScpForAll(nsc, false, false);
}

//---------------------------------------------------------------------
// ���O�������\���̃��S�����\�������f�i�I��t���j
//---------------------------------------------------------------------
bool JlsDataset::isElgInScpForAll(Nsc nsc, bool flag_border, bool flag_out){
	bool ret = false;
	int num_scpos = sizeDataScp();
	if (nsc > 0 && nsc < num_scpos){
		//--- �\����؂�łȂ���Ύ��̈ʒu�擾 ---
		if (isScpChapTypeDecide( getScpChap(nsc) ) == false){
			if (nsc < num_scpos-1){
				nsc = getNscNextScpDecide(nsc, SCP_END_EDGEIN);
			}
		}
		//--- ���S�����\�������f ---
		ScpArType arstat = getScpArstat(nsc);
		if (flag_border == false){
			ret = jlsd::isScpArTypeLogo(arstat);
		}
		else{
			ret = jlsd::isScpArTypeLogoBorder(arstat);
		}
		//--- �P���ݒ�Ƃ͋t�ɂȂ�o�� ---
		if (flag_out){
			switch(arstat){
				case SCP_AR_B_UNIT  :
				case SCP_AR_B_OTHER :
					ret = true;
					break;
				default:
					break;
			}
			ScpArExtType arext = getScpArext(nsc);
			switch(arext){
				case SCP_AREXT_L_TRKEEP :
				case SCP_AREXT_L_TRRAW :
				case SCP_AREXT_L_EC :
				case SCP_AREXT_L_LGADD :
					ret = true;
					break;
				case SCP_AREXT_L_TRCUT :
					if (getConfig(CONFIG_VAR_flagCutTR) == 1){
						ret = false;
					}
					else{
						ret = true;
					}
					break;
				case SCP_AREXT_L_SP :
					if (getConfig(CONFIG_VAR_flagCutSP) == 1){
						ret = false;
					}
					else{
						ret = true;
					}
					break;
				case SCP_AREXT_N_TRCUT :
				case SCP_AREXT_L_ECCUT :
				case SCP_AREXT_L_LGCUT :
				case SCP_AREXT_N_LGCUT :
					ret = false;
					break;
				case SCP_AREXT_N_LGADD :
					ret = true;
					break;
				default:
					break;
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �w��ʒu��Auto��؂肪�m���Ԃ����f
//---------------------------------------------------------------------
bool JlsDataset::isScpChapTypeDecideFromNsc(Nsc nsc){
	ScpChapType chap_nsc = getScpChap(nsc);
	return jlsd::isScpChapTypeDecide(chap_nsc);
}

//---------------------------------------------------------------------
// �w��ʒu�̖����\���������Ȃ������f
//---------------------------------------------------------------------
bool JlsDataset::isStillFromMsec(Msec msec_target){
	int num_scpos = sizeDataScp();
	bool det = true;
	bool over = false;
	int i = 1;
	while(i < num_scpos-1 && det == false && over == false){
		Msec msec_s = m_scp[i].msmute_s;
		Msec msec_e = m_scp[i].msmute_e;
		if (msec_s <= msec_target && msec_target <= msec_e){
			if (m_scp[i].still == 0){
				det = false;
			}
		}
		if (msec_s > msec_target){
			over = true;
		}
		i ++;
	}
	return det;
}

//---------------------------------------------------------------------
// �w��ʒu�������\�������f
//---------------------------------------------------------------------
bool JlsDataset::isSmuteFromMsec(Msec msec_target){
	int num_scpos = sizeDataScp();
	bool det = false;
	bool over = false;
	int i = 1;
	while(i < num_scpos-1 && det == false && over == false){
		int msec_s = m_scp[i].msmute_s;
		int msec_e = m_scp[i].msmute_e;
		if (msec_s <= msec_target && msec_target <= msec_e){
			det = true;
		}
		if (msec_s > msec_target){
			over = true;
		}
		i ++;
	}
	return det;
}

//---------------------------------------------------------------------
// �Q�n�_������������Ԃ̃f�[�^�����f
//---------------------------------------------------------------------
bool JlsDataset::isSmuteSameArea(Nsc nsc1, Nsc nsc2){
	int num_scpos = sizeDataScp();
	if (nsc1 > 0 && nsc1 < num_scpos-1 && nsc2 > 0 && nsc2 < num_scpos-1){
		if (m_scp[nsc1].msmute_s == m_scp[nsc2].msmute_s &&
			m_scp[nsc1].msmute_e == m_scp[nsc2].msmute_e){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------
// �͈͂�����
// ���́F
//    rmsec  : ���肷��͈�
// ���o�́F
//    wmsec  : �Ώۃf�[�^�i�͈͂Ȃ��̎��͏��������Ȃ��j
// �o��:
//   �Ԃ�l�F false=�͈͂Ȃ�  true=�͈͐ݒ�
//---------------------------------------------------------------------
bool JlsDataset::limitWideMsecFromRange(WideMsec wmsec, RangeMsec rmsec){
	if (wmsec.late  < rmsec.st && wmsec.late  >= 0 && rmsec.st >= 0) return false;
	if (wmsec.early > rmsec.ed && wmsec.early >= 0 && rmsec.ed >= 0) return false;
	if (rmsec.st >= 0){
		if (wmsec.early < rmsec.st) wmsec.early = rmsec.st;
		if (wmsec.just  < rmsec.st) wmsec.just  = rmsec.st;
	}
	if (rmsec.ed >= 0){
		if (wmsec.late > rmsec.ed || wmsec.late < 0) wmsec.late  = rmsec.ed;
		if (wmsec.just > rmsec.ed || wmsec.just < 0) wmsec.just  = rmsec.ed;
	}
	return true;
}



//=====================================================================
// Term�\������
//=====================================================================

//---------------------------------------------------------------------
// �������ɒ[���܂߂邩�I��
//---------------------------------------------------------------------
void JlsDataset::setTermEndtype(Term &term, ScpEndType endtype){
	term.endfix = true;
	term.endtype = endtype;
}

//---------------------------------------------------------------------
// ���̍\�����擾
//---------------------------------------------------------------------
bool JlsDataset::getTermNext(Term &term){
	bool ret = false;
	if (!term.valid){				// ������s
		term.valid = true;
		if (!term.endfix){
			term.endfix = true;
			term.endtype = SCP_END_NOEDGE;
		}
		if (term.ini > 0){		// �J�n�ʒu�ݒ莞
			term.nsc.ed = term.ini;
		}
		else{
			term.nsc.ed = -1;
			term.nsc.ed = pdata->getNscNextScpDecide(term.nsc.ed, term.endtype);
		}
		term.msec.ed = pdata->getMsecScp(term.nsc.ed);;
	}
	term.nsc.st = term.nsc.ed;
	term.msec.st = term.msec.ed;
	term.nsc.ed = pdata->getNscNextScpDecide(term.nsc.ed, term.endtype);
	term.msec.ed = pdata->getMsecScp(term.nsc.ed);
	if (term.nsc.ed >= 0){
		ret = true;
	}
	return ret;
}

//---------------------------------------------------------------------
// �O�̍\�����擾
//---------------------------------------------------------------------
bool JlsDataset::getTermPrev(Term &term){
	bool ret = false;
	if (!term.valid){				// ������s
		term.valid = true;
		if (!term.endfix){
			term.endfix = true;
			term.endtype = SCP_END_NOEDGE;
		}
		if (term.ini > 0){		// �J�n�ʒu�ݒ莞
			term.nsc.st = term.ini;
		}
		else{
			term.nsc.st = pdata->sizeDataLogo();
			term.nsc.st = pdata->getNscPrevScpDecide(term.nsc.st, term.endtype);
		}
		term.msec.st = pdata->getMsecScp(term.nsc.st);
	}
	term.nsc.ed = term.nsc.st;
	term.msec.ed = term.msec.st;
	term.nsc.st = pdata->getNscPrevScpDecide(term.nsc.st, term.endtype);
	term.msec.st = pdata->getMsecScp(term.nsc.st);
	if (term.nsc.st >= 0){
		ret = true;
	}
	return ret;
}

//---------------------------------------------------------------------
// �e�v�f�̐ݒ�E�擾
//---------------------------------------------------------------------
ScpArType JlsDataset::getScpArstat(Term term){
	return pdata->getScpArstat(term.nsc.ed);
}
ScpArExtType JlsDataset::getScpArext(Term term){
	return pdata->getScpArext(term.nsc.ed);
}
void JlsDataset::setScpArstat(Term term, ScpArType arstat){
	pdata->setScpArstat(term.nsc.ed, arstat);
}
void JlsDataset::setScpArext(Term term, ScpArExtType arext){
	pdata->setScpArext(term.nsc.ed, arext);
}

//---------------------------------------------------------------------
// CM�\��������
//---------------------------------------------------------------------
bool JlsDataset::isScpArstatCmUnit(Term term){
	ScpArType arstat_term = getScpArstat(term);
	if ((arstat_term == SCP_AR_N_UNIT)  ||
		(arstat_term == SCP_AR_N_AUNIT) ||
		(arstat_term == SCP_AR_N_BUNIT)){
		return true;
	}
	return false;
}

//---------------------------------------------------------------------
// �����͈͓��ɂ��邩����
//---------------------------------------------------------------------
bool JlsDataset::checkScopeTerm(Term term, RangeMsec scope){
	int msec_spc  = pdata->msecValSpc;
	return (term.msec.st >= scope.st - msec_spc && term.msec.ed <= scope.ed + msec_spc)? true : false;
}

bool JlsDataset::checkScopeRange(RangeMsec bounds, RangeMsec scope){
	int msec_spc  = pdata->msecValSpc;
	return (bounds.st >= scope.st - msec_spc && bounds.ed <= scope.ed + msec_spc)? true : false;
}


//=====================================================================
// �f�[�^�}��
//=====================================================================

//---------------------------------------------------------------------
// ���S�f�[�^�}��
// ���́F
//   msec_st : ���S�f�[�^�}���J�n�ʒu
//   msec_ed : ���S�f�[�^�}���I���ʒu
//   overlap : �������S�Əd�����i0=����  1=�d�����j
//   confirm : �m��������ɍs�����i0=�ݒ�̂�  1=�m�菈�������s�j
//   unit    : �}���ʒu�����œƗ��\���ɂ��邩�i0=�]�����S�ƍ���  1=�}�����S�����œƗ��\���j
// �o�́F
//   �Ԃ�l�F �}���ʒu�ԍ� �}�����Ȃ������ꍇ��-1
//---------------------------------------------------------------------
Nsc JlsDataset::insertLogo(Msec msec_st, Msec msec_ed, bool overlap, bool confirm, bool unit){
	int size_logo = sizeDataLogo();
	Msec wid_ovl = pdata->msecValSpc;

	// ���S�}���ӏ�������
	int num_ins = -1;
	for(int i=1; i<size_logo; i++){
		if (msec_st >= getMsecLogoFall(i-1) && msec_ed <= getMsecLogoRise(i)){
			num_ins = i;
		}
	}
	if (num_ins < 0){
		if (msec_ed <= getMsecLogoRise(0)){
			num_ins = 0;
		}
		else if (msec_st >= getMsecLogoFall(size_logo-1)){
			num_ins = size_logo;
		}
	}
	if (size_logo >= MAXSIZE_LOGO){
		num_ins = -1;
	}

	int total_ins = 1;
	// �}���ӏ��Ɠ����ʒu�̊m����ۑ��p
	Msec bak_result_rise, bak_result_fall;
	bool flag_bak_rise = false;
	bool flag_bak_fall = false;
	LogoResultType bak_outtype_rise = LOGO_RESULT_NONE;
	LogoResultType bak_outtype_fall = LOGO_RESULT_NONE;
	// overlap�����̒ǉ�����
	if (overlap && num_ins < 0 && size_logo < MAXSIZE_LOGO){
		for(int i=0; i<size_logo; i++){
			DataLogoRecord dt;
			getRecordLogo(dt, i);
			// �}���ӏ��Ɠ����ʒu�̊m����ۑ�
			if (abs(msec_st - dt.rise) <= wid_ovl){
				flag_bak_rise    = true;
				bak_outtype_rise = dt.outtype_rise;
				bak_result_rise  = dt.result_rise;
			}
			if (abs(msec_ed - dt.fall) <= wid_ovl){
				flag_bak_fall    = true;
				bak_outtype_fall = dt.outtype_fall;
				bak_result_fall  = dt.result_fall;
			}
			// �}�����S�ƈ�v����ꍇ
			if (msec_st >= dt.rise - wid_ovl && msec_st <= dt.rise + wid_ovl &&
				msec_ed >= dt.fall - wid_ovl && msec_ed <= dt.fall + wid_ovl){
				return 0;
			}
			// �}�����S�̑O���̂݃��S�Əd�Ȃ�ꍇ
			if (msec_st > dt.rise + wid_ovl && msec_st < dt.fall &&
				msec_ed > dt.fall - wid_ovl){
				dt.fall         = msec_st;
				dt.fall_l       = msec_st;
				dt.fall_r       = msec_st;
				dt.fade_fall    = 0;
				dt.intl_fall    = 0;
				dt.stat_fall    = LOGO_PRIOR_DECIDE;
				dt.outtype_fall = LOGO_RESULT_NONE;
				setRecordLogo(dt, i);
				if (confirm){
					setResultLogoAtNrf(dt.fall, LOGO_RESULT_DECIDE, i*2+1);
				}
			}
			// �}�����S�̌㔼�̂݃��S�Əd�Ȃ�ꍇ
			if (msec_st < dt.rise + wid_ovl &&
				msec_ed > dt.rise && msec_ed < dt.fall - wid_ovl){
				dt.rise         = msec_ed;
				dt.rise_l       = msec_ed;
				dt.rise_r       = msec_ed;
				dt.fade_rise    = 0;
				dt.intl_rise    = 0;
				dt.stat_rise    = LOGO_PRIOR_DECIDE;
				dt.outtype_rise = LOGO_RESULT_NONE;
				setRecordLogo(dt, i);
				if (confirm){
					setResultLogoAtNrf(dt.rise, LOGO_RESULT_DECIDE, i*2);
				}
			}
			// �}�����S���Ƀ��S�S�̂�����ꍇ
			if (msec_st - wid_ovl <= dt.rise && msec_ed + wid_ovl >= dt.fall){
					dt.outtype_rise = LOGO_RESULT_ABORT;			// abort
					dt.outtype_fall = LOGO_RESULT_ABORT;			// abort
					dt.result_rise  = -1;
					dt.result_fall  = -1;
					setRecordLogo(dt, i);
					if (num_ins < 0){
						num_ins = i;
						total_ins = 0;		// �}�����S�͏㏑������
					}
			}
			// �}�����S�S�̂����S�͈͓��ɓ���ꍇ
			if (msec_st > dt.rise + wid_ovl && msec_ed < dt.fall - wid_ovl){
				num_ins = i+1;
				total_ins = 2;				// �}�����S���Q�ɂȂ�
			}
			// �}���ʒu
			if (num_ins < 0 && msec_st <= dt.rise){
				num_ins = i;
			}
		}
		if (num_ins < 0){
			num_ins = size_logo;
		}
	}
	if (size_logo + total_ins > MAXSIZE_LOGO){
		num_ins = -1;
	}

	// ���ʂ��i�[
	int retval = -1;
	if (num_ins >= 0){
		DataLogoRecord dt;
		if (total_ins == 2){		// �������S�r���Ƀ��S�}�����Č����S���Q��������ꍇ
			getRecordLogo(dt, num_ins);
			dt.rise        = msec_ed;
			dt.rise_l      = msec_ed;
			dt.rise_r      = msec_ed;
			dt.fade_rise   = 0;
			dt.intl_rise   = 0;
			dt.stat_rise   = LOGO_PRIOR_DECIDE;
			if (confirm){
				dt.outtype_rise = LOGO_RESULT_DECIDE;
				dt.result_rise  = msec_ed;
			}
			else{
				dt.outtype_rise = LOGO_RESULT_NONE;
				dt.result_rise  = 0;
			}
			insertRecordLogo(dt, num_ins+1);

			getRecordLogo(dt, num_ins);
			dt.fall        = msec_st;
			dt.fall_l      = msec_st;
			dt.fall_r      = msec_st;
			dt.fade_fall   = 0;
			dt.intl_fall   = 0;
			dt.stat_fall   = LOGO_PRIOR_DECIDE;
			if (confirm){
				dt.outtype_fall = LOGO_RESULT_DECIDE;
				dt.result_fall  = msec_st;
			}
			else{
				dt.outtype_fall = LOGO_RESULT_NONE;
				dt.result_fall  = 0;
			}
			setRecordLogo(dt, num_ins);
			num_ins = num_ins + 1;
		}
		retval = num_ins;
		getRecordLogo(dt, num_ins);
		dt.rise        = msec_st;
		dt.rise_l      = msec_st;
		dt.rise_r      = msec_st;
		dt.fall        = msec_ed;
		dt.fall_l      = msec_ed;
		dt.fall_r      = msec_ed;
		dt.fade_rise   = 0;
		dt.fade_fall   = 0;
		dt.intl_rise   = 0;
		dt.intl_fall   = 0;
		dt.stat_rise   = LOGO_PRIOR_DECIDE;
		dt.stat_fall   = LOGO_PRIOR_DECIDE;
		if (confirm){
			dt.outtype_rise = LOGO_RESULT_DECIDE;
			dt.outtype_fall = LOGO_RESULT_DECIDE;
			dt.result_rise  = msec_st;
			dt.result_fall  = msec_ed;
		}
		else{
			dt.outtype_rise = LOGO_RESULT_NONE;
			dt.outtype_fall = LOGO_RESULT_NONE;
			dt.result_rise  = 0;
			dt.result_fall  = 0;
		}
		if (flag_bak_rise){
			dt.outtype_rise = bak_outtype_rise;
			dt.result_rise  = bak_result_rise;
		}
		if (flag_bak_fall){
			dt.outtype_fall = bak_outtype_fall;
			dt.result_fall  = bak_result_fall;
		}
		dt.unit        = LOGO_UNIT_NORMAL;
		if (unit){
			dt.unit    = LOGO_UNIT_DIVIDE;
		}
		if (total_ins == 0){
			setRecordLogo(dt, num_ins);
		}
		else{
			insertRecordLogo(dt, num_ins);
		}
	}
	return retval;
}


//---------------------------------------------------------------------
// �V�[���`�F���W��}��
// ���́F
//   msec_dst_s  : �}���ʒu
//   msec_dst_bk : ���O�I���ʒu
//   nsc_mute    : �����ʒu�֌W�̏����R�s�[���閳��SC�ԍ��i-1�̎������j
//   stat_scpos_dst : �}���\���ɐݒ肷��D��x
// �o�́F
//   �Ԃ�l�F�}�������ꏊ�ԍ�
//---------------------------------------------------------------------
Nsc JlsDataset::insertScpos(Msec msec_dst_s, Msec msec_dst_bk, Nsc nsc_mute, ScpPriorType stat_scpos_dst){
	int num_scpos = sizeDataScp();
	if (num_scpos <= 1){		// �V�[���`�F���W��ǂݍ��݂ł��ĂȂ��ꍇ
		return -1;
	}

	// �}���ꏊ������
	Nsc nsc_ins = 1;
	while( getMsecScp(nsc_ins) < msec_dst_s && nsc_ins < num_scpos - 1){
		nsc_ins ++;
	}

	// �}���ꏊ���m��
	bool flag_ins = 0;
	if ( getMsecScp(nsc_ins) != msec_dst_s || nsc_ins == num_scpos - 1){
		flag_ins = true;
	}

	// �������݈ʒu�I��
	DataScpRecord  dtscp;
	if (flag_ins){
		clearRecordScp(dtscp);
		dtscp.msec = msec_dst_s;
		dtscp.msbk = msec_dst_bk;
		if (nsc_mute >= 0 && nsc_mute < num_scpos){
			DataScpRecord dttmp;
			getRecordScp(dttmp, nsc_mute);
			dtscp.msmute_s = dttmp.msmute_s;
			dtscp.msmute_e = dttmp.msmute_e;
		}
		else{
			dtscp.msmute_s = msec_dst_bk;
			dtscp.msmute_e = msec_dst_s;
		}
		dtscp.statpos = stat_scpos_dst;
		insertRecordScp(dtscp, nsc_ins);			// �}���ɂ��X�V
	}
	else{
		getRecordScp(dtscp, nsc_ins);
		dtscp.statpos = stat_scpos_dst;
		setRecordScp(dtscp, nsc_ins);				// ���������ɂ��X�V
	}
	return nsc_ins;
}

//---------------------------------------------------------------------
// �w��ʒu�i�~���b�j�̖���SC�ԍ����擾
// �Ȃ���΋����I�ɍ쐬���Đݒ�
// ���́F
//   msec_in : �Ώۈʒu�i�~���b�j
//   edge    : �I���G�b�W�iLOGO_EDGE_RISE=�J�n���ALOGO_EDGE_FALL=�I�����j
// �o�́F
//   �Ԃ�l�F�Ή�����ꏊ�ԍ�
// ���ӓ_�F
//   �f�[�^�}���ɂ��V�[���`�F���W�ԍ�(nsc)�ύX����
//---------------------------------------------------------------------
Nsc JlsDataset::getNscForceMsec(Msec msec_in, LogoEdgeType edge){
	Msec msec_clr = msecValNear2;			// �d�Ȃ������̊m��ӏ�����������͈�
	int num_scpos = sizeDataScp();
	bool flag_search = true;

	Nsc nsc_nearest = 0;
	Msec difmsec_nearest = 0;
	Nsc nsc_mute = -1;
	if (msec_in == 0 || msec_in >= getMsecTotalMax()){
		flag_search = false;
	}
	//--- ��ԋ߂��������� ---
	int i = 1;
	while(flag_search){
		DataScpRecord dtscp;
		getRecordScp(dtscp, i);
		Msec msec_i = getMsecScpEdge(i, edge);
		Msec difmsec_i = abs(msec_in - msec_i);
		//--- ��ԋ߂��ꍇ�̍X�V ---
		if (difmsec_nearest > difmsec_i || nsc_nearest == 0){
			//--- �d�Ȃ��Ă���ΏۊO�ƂȂ����ꏊ���O�� ---
			if (nsc_nearest > 0 && difmsec_nearest <= msec_clr){
				setScpChap(nsc_nearest, SCP_CHAP_DUPE);
			}
			nsc_nearest = i;
			difmsec_nearest = difmsec_i;
		}
		else if (difmsec_i <= msec_clr){		// �d�Ȃ��Ă��鏊���O��
			setScpChap(i, SCP_CHAP_DUPE);
		}
		//--- �������ԗ̈�̊m�F ---
		if (dtscp.msmute_s <= msec_i && msec_i <= dtscp.msmute_e){
			if (msec_i <= msec_in || nsc_nearest == i){
				nsc_mute = i;
			}
		}
		//--- ���̈ʒu�ݒ� ---
		i ++;
		if (i >= num_scpos-1 || msec_i >= msec_in + msec_clr){
			flag_search = false;
		}
	}
	//--- �}���ʒu��ݒ� ---
	Msec msec_in_s  = msec_in;
	Msec msec_in_bk = msec_in;
	if (edge == LOGO_EDGE_RISE){
		msec_in_bk = cnv.getMsecAdjustFrmFromMsec(msec_in, -1);
	}
	else{
		msec_in_s  = cnv.getMsecAdjustFrmFromMsec(msec_in, +1);
	}
	Nsc nsc_ret = nsc_nearest;
	//--- ��������SC�ӏ����m�F ---
	bool flag_new = true;
	if (msec_in == 0 || msec_in >= getMsecTotalMax()){
		flag_new = false;
		if (msec_in == 0){
			nsc_ret = 0;
		}
		else{
			nsc_ret = num_scpos-1;
		}
	}
	else if (nsc_nearest > 0){
		int msec_near_s  = getMsecScp(nsc_nearest);
		int msec_near_bk = getMsecScpBk(nsc_nearest);
		if (msec_near_bk <= msec_in_s && msec_in_bk <= msec_near_s){
			flag_new = false;
		}
	}
	//--- �V�K�ӏ��Ȃ�}�� ---
	if (flag_new){
		nsc_ret = insertScpos(msec_in_s, msec_in_bk, nsc_mute, SCP_PRIOR_DECIDE);
	}
	return nsc_ret;
}




//=====================================================================
// �\�����̃��S�\�����Ԃ̎擾
//=====================================================================

//---------------------------------------------------------------------
// �͈͓��ɂ��郍�S�\�����Ԃ̕b�����擾
//---------------------------------------------------------------------
Sec JlsDataset::getSecLogoComponent(Msec msec_s, Msec msec_e){
	//--- �����S�f�[�^���g��Ȃ��ꍇ�̃��S���Ԑݒ� ---
	if ( isAutoLogoOnly() ){
		return getSecLogoComponentFromElg(msec_s, msec_e);
	}
	return getSecLogoComponentFromLogo(msec_s, msec_e);
}

// �����S�f�[�^����
Sec JlsDataset::getSecLogoComponentFromLogo(Msec msec_s, Msec msec_e){
	NrfCurrent logopt = {};
	Msec sum_msec = 0;
	//--- �\���̒[�������̂Ă锻�ʊ��� ---
	Msec msec_dif_mid = abs(msec_e - msec_s) / 2;
	Msec msec_dif_min = abs(msec_e - msec_s) / 4;
	if (msec_dif_mid > 11500){
		msec_dif_mid = 11500;
	}
	if (msec_dif_min > 4500){
		msec_dif_min = 4500;
	}
	while( getNrfptNext(logopt, LOGO_SELECT_VALID) ){
		//--- �͈͓��Ƀ��S�\�����Ԃ�����ꍇ ---
		if (logopt.msecRise + msecValSpc < msec_e && logopt.msecFall > msec_s + msecValSpc){
			Msec tmp_s = (logopt.msecRise < msec_s)? msec_s : logopt.msecRise;
			Msec tmp_e = (logopt.msecFall > msec_e)? msec_e : logopt.msecFall;
			//--- �[�����̏���(rise) ---
			{
				WideMsec wmsec;
				getWideMsecLogoNrf(wmsec, logopt.nrfRise);
				if (logopt.msecRise < msec_s + msec_dif_mid && logopt.msecFall >= msec_e){
					if (wmsec.early <= msec_s + msecValSpc && tmp_s > msec_s){
						tmp_s = msec_s;
					}
				}
				if (logopt.msecRise > msec_e - msec_dif_min){
					tmp_s = tmp_e;
				}
				else if (logopt.msecRise > msec_e - msec_dif_mid && logopt.msecFall >= msec_e){
					if (wmsec.late >= msec_e - msec_dif_min){
						tmp_s = tmp_e;
					}
				}
			}
			//--- �[�����̏���(fall) ---
			{
				WideMsec wmsec;
				getWideMsecLogoNrf(wmsec, logopt.nrfFall);
				if (logopt.msecFall > msec_e - msec_dif_mid && logopt.msecRise <= msec_s){
					if (wmsec.late >= msec_e - msecValSpc && tmp_e < msec_e){
						tmp_e = msec_e;
					}
				}
				if (logopt.msecFall < msec_s + msec_dif_min){
					tmp_e = tmp_s;
				}
				else if (logopt.msecFall < msec_s + msec_dif_mid && logopt.msecRise <= msec_s){
					if (wmsec.early <= msec_s + msec_dif_min){
						tmp_s = tmp_e;
					}
				}
			}
			// ���S�\�����Ԃ�ǉ�
			if (tmp_s < tmp_e){
				sum_msec += (tmp_e - tmp_s);
			}
		}
	}
	// �b���ɕϊ����ă��^�[��
	return cnv.getSecFromMsec(sum_msec);
}

// �����\���̃��S��������
Sec JlsDataset::getSecLogoComponentFromElg(Msec msec_s, Msec msec_e){
	Msec sum_msec = 0;
	ElgCurrent elg = {};
	while( getElgptNext(elg) ){
		// �͈͓��Ƀ��S�\�����Ԃ�����ꍇ
		if (elg.msecRise + msecValSpc < msec_e && elg.msecFall > msec_s + msecValSpc){
			Msec tmp_s, tmp_e;
			if (elg.msecRise < msec_s){
				tmp_s = msec_s;
			}
			else{
				tmp_s = elg.msecRise;
			}
			if (elg.msecFall > msec_e){
				tmp_e = msec_e;
			}
			else{
				tmp_e = elg.msecFall;
			}
			// ���S�\�����Ԃ�ǉ�
			if (tmp_s < tmp_e){
				sum_msec += (tmp_e - tmp_s);
			}
		}
	}
	// �b���ɕϊ����ă��^�[��
	return cnv.getSecFromMsec(sum_msec);
}



//=====================================================================
// ����Trim�쐬
//=====================================================================

//---------------------------------------------------------------------
// �J�b�g���ʍ쐬
//---------------------------------------------------------------------
void JlsDataset::outputResultTrimGen(){
	resultTrim.clear();
	if ( isAutoModeUse() ){
		outputResultTrimGenAuto();
	}
	else{
		outputResultTrimGenManual();
	}
}

// �J�b�g���ʍ쐬�i�\���������Ȃ��ꍇ�j
void JlsDataset::outputResultTrimGenManual(){
	Nlg nlg_fall = 0;
	do{
		Msec msec_rise, msec_fall;
		bool cont_next;
		nlg_fall = getResultLogoNext(msec_rise, msec_fall, cont_next, nlg_fall);
		if (nlg_fall >= 0){
			//--- ���S���؂�ڂȂ������ꍇ�͘A�� ---
			while(cont_next && nlg_fall >= 0){
				Msec msec_dmy;
				nlg_fall = getResultLogoNext(msec_dmy, msec_fall, cont_next, nlg_fall);
			}
			//--- ���ʃf�[�^�ǉ� ---
			resultTrim.push_back( msec_rise );
			resultTrim.push_back( msec_fall );
		}
	} while(nlg_fall >= 0);
}


// �J�b�g���ʍ쐬�i�\����������ꍇ�j
void JlsDataset::outputResultTrimGenAuto(){
	ElgCurrent elg = {};
	elg.outflag = true;					// �ŏI�o�͗p�̐ݒ�
	while ( getElgptNext(elg) ){
		resultTrim.push_back( elg.msecRise );
		resultTrim.push_back( elg.msecFall );
	}
}



//=====================================================================
// �ڍ׏��o�͍쐬
//=====================================================================

//---------------------------------------------------------------------
// �ڍ׏�񌋉ʂ̓ǂݏo���ʒu���Z�b�g
//---------------------------------------------------------------------
void JlsDataset::outputResultDetailReset(){
	m_nscOutDetail = 0;
}

//---------------------------------------------------------------------
// �ڍ׏�񌋉ʂ̕�����쐬
// �o��
//   �Ԃ�l�F 0=����  1=�I��
//   strBuf�F �o�͕�����
//---------------------------------------------------------------------
bool JlsDataset::outputResultDetailGetLine(string &strBuf){
	int num_scpos = sizeDataScp();
	//--- ���݈ʒu���擾 ---
	Nsc i = m_nscOutDetail;
	if (i < 0 || i >= num_scpos-1){
		return 1;
	}
	Msec msec_from = getMsecScp(i);
	//--- ���̈ʒu���擾 ---
	Msec msec_next;
	do{
		i = getNscNextScpOutput(i, SCP_END_EDGEIN);		// ���̍\����؂�ʒu�擾�i�[���܂ށj
		msec_next = getMsecScp(i);
	} while(msec_from == msec_next && i >= 0);
	//--- �ʒu��ݒ� ---
	m_nscOutDetail = i;
	if (i >= 0){
		//--- �O��Ԋu�̊��Ԏ擾 ---
		Msec msec_dif = msec_next - msec_from;
		Sec  sec_dif  = cnv.getSecFromMsec( msec_dif  );
		int frm_from = cnv.getFrmFromMsec( msec_from );
		int frm_next = cnv.getFrmFromMsec( msec_next );
		int frm_dif  = frm_next - cnv.getFrmFromMsec(sec_dif*1000 + msec_from);
		Sec sec_logo = getSecLogoComponent(msec_from, msec_next);
		//--- �I���n�_���擾 ---
		Msec msec_to  = getMsecScpBk(i);
		int frm_to   = cnv.getFrmFromMsec( msec_to   );
		//--- �\�� ---
		char buffer[80];
		snprintf(buffer, sizeof(buffer), "%6d %6d %4d %3d %4d ",
					frm_from, frm_to, sec_dif, frm_dif, sec_logo);
		strBuf = buffer;
		//--- �\�����̂��擾 ---
		if (m_flagSetupAuto > 1){
			outputResultDetailGetLineLabel(strBuf, getScpArstat(i), getScpArext(i));
		}
		return 0;
	}
	return 1;		// EOF
}

//---------------------------------------------------------------------
// �ڍ׏�񌋉ʂ̕�����쐬�i�����������́j
//---------------------------------------------------------------------
void JlsDataset::outputResultDetailGetLineLabel(string &strBuf, ScpArType arstat, ScpArExtType arext){
	const char *pstr_arstat = "";

	switch(arstat){
		case SCP_AR_N_UNIT :
		case SCP_AR_N_AUNIT :
		case SCP_AR_N_BUNIT :
			pstr_arstat = ":CM";
			break;
		case SCP_AR_N_OTHER :
			pstr_arstat = ":Nologo";
			break;
		case SCP_AR_B_UNIT  :
			pstr_arstat = ":Border15s";
			break;
		case SCP_AR_B_OTHER :
			pstr_arstat = ":Border";
			break;
		case SCP_AR_L_UNIT :
		case SCP_AR_L_OTHER :
			pstr_arstat = ":L";
			break;
		case SCP_AR_L_MIXED :
			pstr_arstat = ":Mix";
			break;
		default :
			pstr_arstat = ":";
	}
	switch(arext){
		case SCP_AREXT_L_TRKEEP :
			pstr_arstat = ":Trailer(add)";
			break;
		case SCP_AREXT_L_ECCUT :
			pstr_arstat = ":Trailer(cut)";
			break;
		case SCP_AREXT_L_TRRAW :
			pstr_arstat = ":Trailer";
			break;
		case SCP_AREXT_L_TRCUT :
			if (getConfig(CONFIG_VAR_flagCutTR) == 1){
				pstr_arstat = ":Trailer(cut)";
			}
			else{
				pstr_arstat = ":Trailer(cut-cancel)";
			}
			break;
		case SCP_AREXT_L_SP :
			if (getConfig(CONFIG_VAR_flagCutSP) == 1){
				pstr_arstat = ":Sponsor(cut)";
			}
			else{
				pstr_arstat = ":Sponsor(add)";
			}
			break;
		case SCP_AREXT_L_EC :
			pstr_arstat = ":Endcard(add)";
			break;
		case SCP_AREXT_L_LGCUT :
			pstr_arstat = ":L-Edge(cut)";
			break;
		case SCP_AREXT_L_LGADD :
			pstr_arstat = ":L-Edge(add)";
			break;
		case SCP_AREXT_N_TRCUT :
			pstr_arstat = ":Nologo(cut)";
			break;
		case SCP_AREXT_N_LGCUT :
			pstr_arstat = ":N-Edge(cut)";
			break;
		case SCP_AREXT_N_LGADD :
			pstr_arstat = ":N-Edge(add)";
			break;
		default:
			break;
	}
	strBuf += pstr_arstat;
}



//=====================================================================
// �f�o�b�O�p�\��
//=====================================================================

//---------------------------------------------------------------------
// �f�o�b�O�p�\���i���S�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::displayLogo(){
	int n = (int) m_logo.size();
	for(int i=0; i<n; i++){
		printf("%3d %6d %6d %6d %6d %6d %6d %d%d%d%d %d %d %d %d %d %6d %6d\n",
			i, cnv.getFrmFromMsec(m_logo[i].rise), cnv.getFrmFromMsec(m_logo[i].fall),
			cnv.getFrmFromMsec(m_logo[i].rise_l), cnv.getFrmFromMsec(m_logo[i].rise_r),
			cnv.getFrmFromMsec(m_logo[i].fall_l), cnv.getFrmFromMsec(m_logo[i].fall_r),
			m_logo[i].fade_rise, m_logo[i].fade_fall, m_logo[i].intl_rise, m_logo[i].intl_fall,
			m_logo[i].stat_rise, m_logo[i].stat_fall, m_logo[i].unit,
			m_logo[i].outtype_rise, m_logo[i].outtype_fall,
			cnv.getFrmFromMsec(m_logo[i].result_rise), cnv.getFrmFromMsec(m_logo[i].result_fall)
		);
	};
}

//---------------------------------------------------------------------
// �f�o�b�O�p�\���i����SC�f�[�^�j
//---------------------------------------------------------------------
void JlsDataset::displayScp(){
	int n = (int) m_scp.size();
	for(int i=0; i<n; i++){
		printf("%3d %6d %6d %6d %6d %d %d (%8d %8d %8d %8d)%4d %2d %2d %2d)\n",
			i, cnv.getFrmFromMsec(m_scp[i].msec), cnv.getFrmFromMsec(m_scp[i].msbk),
			cnv.getFrmFromMsec(m_scp[i].msmute_s), cnv.getFrmFromMsec(m_scp[i].msmute_e),
			m_scp[i].still, m_scp[i].statpos,
			m_scp[i].msec, m_scp[i].msbk, m_scp[i].msmute_s, m_scp[i].msmute_e,
			m_scp[i].score, m_scp[i].chap, m_scp[i].arstat, m_scp[i].arext
		);
	}
}

