//
#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsScript.hpp"
#include "JlsReformData.hpp"
#include "JlsAutoScript.hpp"
#include "JlsCmdSet.hpp"
#include "JlsDataset.hpp"


///////////////////////////////////////////////////////////////////////
//
// �ϐ��N���X
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// �ϐ���ݒ�
// ���́F
//   strName   : �ϐ���
//   strVal    : �ϐ��l
//   overwrite : 0=����`���̂ݐݒ�  1=�㏑�����ݒ�
// �o�́F
//   �Ԃ�l    : �ʏ�=true�A���s��=false
//---------------------------------------------------------------------
bool JlsRegFile::setRegVar(const string &strName, const string &strVal, bool overwrite){
	int n;
	int nloc   = -1;
	int nlenvar = (int) strName.size();
	int nMaxList = (int) m_strListVar.size();
	string strOrgName, strOrgVal;
	string strPair;

	//--- �����ϐ��̏����������`�F�b�N ---
	for(int i=0; i<nMaxList; i++){
		n = getRegNameVal(strOrgName, strOrgVal, m_strListVar[i]);
		if (nlenvar == n){
			if (_stricmp(strName.c_str(), strOrgName.c_str()) == 0){
				nloc = i;
			}
		}
	}
	//--- �ݒ蕶����쐬 ---
	strPair = strName + ":" + strVal;
	//--- �����ϐ��̏������� ---
	if (nloc >= 0){
		if (overwrite){
			m_strListVar[nloc] = strPair;
		}
	}
	//--- �V�K�ϐ��̒ǉ� ---
	else{
		if (nMaxList < SIZE_VARNUM_MAX){		// �O�̂��ߕϐ��ő吔�܂�
			m_strListVar.push_back(strPair);
		}
		else{
			return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------
// �ϐ���ǂݏo��
// ���́F
//   strCandName : �ǂݏo���ϐ����i���j
//   excact      : 0=���͕����ɍő�}�b�`����ϐ�  1=���͕����Ɗ��S��v����ϐ�
// �o�́F
//   �Ԃ�l  : �ϐ����̕������i0�̎��͑Ή��ϐ��Ȃ��j
//   strVal  : �ϐ��l
//---------------------------------------------------------------------
int JlsRegFile::getRegVar(string &strVal, const string &strCandName, bool exact){
	int n;
	int nmatch = 0;
	int nloc   = -1;
	int nlencand = (int) strCandName.size();
	int nMaxList = (int) m_strListVar.size();
	string strOrgName, strOrgVal;

	//--- ���O�ƃ}�b�`����ʒu������ ---
	for(int i=0; i<nMaxList; i++){
		//--- �ϐ����ƒl������e�[�u������擾 ---
		n = getRegNameVal(strOrgName, strOrgVal, m_strListVar[i]);
		//--- �����e�[�u���ϐ����������܂ł̍ő��v��蒷����Ό��� ---
		if (nmatch < n){
			if (_strnicmp(strCandName.c_str(), strOrgName.c_str(), n) == 0 &&	// �擪�ʒu����}�b�`
				(n == nlencand || exact == false)){								// ���ꕶ����exact=false
				nloc   = i;
				nmatch = n;
			}
		}
	}
	//--- �}�b�`�����ꍇ�̒l�̓ǂݏo�� ---
	if (nloc >= 0){
		n = getRegNameVal(strOrgName, strVal, m_strListVar[nloc]);			// �ϐ��l���o��
	}
	return nmatch;
}

//---------------------------------------------------------------------
// �i�[�ϐ��𖼑O�ƒl�ɕ����i�ϐ��ǂݏ����֐�����̃T�u���[�`���j
//---------------------------------------------------------------------
int JlsRegFile::getRegNameVal(string &strName, string &strVal, const string &strPair){
	//--- �ŏ��̃f���~�^���� ---
	int n = (int) strPair.find(":");
	//--- �f���~�^�𕪉����ďo�͂ɐݒ� ---
	if (n > 0){
		strName = strPair.substr(0, n);
		int nLenPair = (int) strPair.length();
		if (n < nLenPair-1){
			strVal = strPair.substr(n+1);
		}
		else{
			strVal = "";
		}
	}
	return n;
}


///////////////////////////////////////////////////////////////////////
//
// �X�N���v�g����
//
///////////////////////////////////////////////////////////////////////
JlsScriptState::JlsScriptState(){
	clear();
}

//---------------------------------------------------------------------
// ������
//---------------------------------------------------------------------
void JlsScriptState::clear(){
	m_ifSkip = false;
	m_listIfState.clear();
	m_repSkip = false;
	m_repLineReadCache = -1;
	m_listRepCmdCache.clear();
	m_listRepDepth.clear();
}

//---------------------------------------------------------------------
// If���ݒ�
//   ����:   flag_cond=������
//   �Ԃ�l: �G���[�ԍ��i0=����I���A1=�G���[�j
//---------------------------------------------------------------------
int JlsScriptState::ifBegin(bool flag_cond){
	CondIfState stat;
	if (m_ifSkip){
		m_ifSkip = true;
		stat = COND_IF_FINISHED;			// �����F�I����
	}
	else if (flag_cond == false){
		m_ifSkip = true;
		stat = COND_IF_PREPARE;				// �����F�����s
	}
	else{
		m_ifSkip = false;
		stat = COND_IF_RUNNING;				// �����F���s
	}
	m_listIfState.push_back(stat);			// ���X�g�ɕۑ�
	return 0;
}

//---------------------------------------------------------------------
// EndIf���ݒ�
//   �Ԃ�l: �G���[�ԍ��i0=����I���A1=�G���[�j
//---------------------------------------------------------------------
int JlsScriptState::ifEnd(){
	int depth = (int) m_listIfState.size();
	if (depth <= 0){
		return 1;
	}
	m_listIfState.pop_back();				// ���X�g����폜
	depth --;
	if (depth <= 0){
		m_ifSkip = false;
	}
	else{
		if (m_listIfState[depth-1] == COND_IF_RUNNING){
			m_ifSkip = false;
		}
		else{
			m_ifSkip = true;
		}
	}
	return 0;
}

//---------------------------------------------------------------------
// ElsIf���ݒ�
//   ����:   flag_cond=������
//   �Ԃ�l: �G���[�ԍ��i0=����I���A1=�G���[�j
//---------------------------------------------------------------------
int JlsScriptState::ifElse(bool flag_cond){
	int depth = (int) m_listIfState.size();
	if (depth <= 0){
		return 1;
	}
	CondIfState stat_cur = m_listIfState[depth-1];
	CondIfState stat_nxt = stat_cur;
	switch(stat_cur){
		case COND_IF_FINISHED:
		case COND_IF_RUNNING:
			m_ifSkip = true;
			stat_nxt = COND_IF_FINISHED;
			break;
		case COND_IF_PREPARE:
			if (flag_cond){
				m_ifSkip = false;
				stat_nxt = COND_IF_RUNNING;
			}
			else{
				m_ifSkip = true;
				stat_nxt = COND_IF_PREPARE;
			}
			break;
	}
	m_listIfState[depth-1] = stat_nxt;
	return 0;
}

//---------------------------------------------------------------------
// Repeat���ݒ�
//   ����:   �J��Ԃ���
//   �Ԃ�l: �G���[�ԍ��i0=����I���A1=�G���[�j
//---------------------------------------------------------------------
int JlsScriptState::repeatBegin(int num){
	//--- �ŏ��̃��s�[�g���� ---
	int depth = (int) m_listRepDepth.size();
	if (depth == 0){
		m_repSkip = false;					// ��΂��Ȃ�
		m_repLineReadCache = -1;			// �L���b�V���ǂݏo������
	}
	//--- �L���b�V���s���`�F�b�N ---
	int size_line = (int) m_listRepCmdCache.size();
	if (size_line >= SIZE_REPLINE){			// �ő�s���`�F�b�N
		return 1;
	}
	//--- �L���b�V�����ĂȂ��ꍇ�͌��݃R�}���h���L���b�V�� ---
	if (size_line == 0){
		string str_tmp = "Repeat " + to_string(num);
		m_listRepCmdCache.push_back(str_tmp);
		size_line ++;
	}
	//--- ���s�[�g����ݒ� ---
	if (num <= 0 || m_repSkip){				// �ŏ�������s�Ȃ��̏ꍇ
		num = -1;							// ���s�Ȃ����̉񐔂�-1�ɂ���
		m_repSkip = true;					// �R�}���h���΂�
	}
	int line_start = m_repLineReadCache;
	if (line_start < 0) line_start = size_line;
	//--- �ݒ�ۑ� ---
	RepDepthHold holdval;
	holdval.lineStart = line_start;			// �J�n�s��ݒ�
	holdval.countLoop = num;				// �J��Ԃ��񐔂�ݒ�
	m_listRepDepth.push_back(holdval);		// ���X�g�ɕۑ�
	return 0;
}

//---------------------------------------------------------------------
// EndRepeat���ݒ�
//   �Ԃ�l: �G���[�ԍ��i0=����I���A1=�G���[�j
//---------------------------------------------------------------------
int JlsScriptState::repeatEnd(){
	int depth = (int) m_listRepDepth.size();
	if (depth <= 0){
		return 1;
	}
	if (m_listRepDepth[depth-1].countLoop > 0){		// �J�E���g�_�E��
		m_listRepDepth[depth-1].countLoop --;
	}
	if (m_listRepDepth[depth-1].countLoop > 0){		// �J��Ԃ��p���̏ꍇ
		m_repLineReadCache = m_listRepDepth[depth-1].lineStart;	// �ǂݏo���s�ݒ�
	}
	else{											// �J��Ԃ��I���̏ꍇ
		m_listRepDepth.pop_back();					// ���X�g����폜
		depth --;
		//--- �S���s�[�g�I�����̏��� ---
		if (depth == 0){
			m_listRepCmdCache.clear();				// �L���b�V��������̏���
			m_repSkip = false;
		}
		//--- ��΂���Ԃ��X�V ---
		else if (m_repSkip){
			if (m_listRepDepth[depth-1].countLoop >= 0){
				m_repSkip = false;
			}
		}
	}
	return 0;
}

//---------------------------------------------------------------------
// �l�X�g��Ԃ��c���Ă��邩�m�F
//   �Ԃ�l: �G���[�ԍ��i0=����I���Abit0=If���l�X�g���Abit1=Repeat���l�X�g���j
//---------------------------------------------------------------------
int  JlsScriptState::isRemainNest(){
	int ret = 0;
	if ((int)m_listIfState.size() != 0) ret += 1;
	if ((int)m_listRepDepth.size() != 0) ret += 2;
	return ret;
}

//---------------------------------------------------------------------
// Cache����̓ǂݏo��
//   �Ԃ�l: �ǂݏo�����ʁifalse=�ǂݏo���Ȃ��Atrue=cache����̓ǂݏo���j
//   strBufOrg: �ǂݏo���ꂽ������
//---------------------------------------------------------------------
bool JlsScriptState::readCmdCache(string &strBufOrg){
	//--- �ǂݏo���\���`�F�b�N ---
	if ( (int)m_listRepDepth.size() <= 0 ){		// Repeat���ł͂Ȃ�
		return false;
	}
	if ( m_repLineReadCache >= (int)m_listRepCmdCache.size() ){
		m_repLineReadCache = -1;
	}
	if (m_repLineReadCache < 0) return false;

	//--- �ǂݏo�����s ---
	strBufOrg = m_listRepCmdCache[m_repLineReadCache];
	m_repLineReadCache ++;
	return true;
}

//---------------------------------------------------------------------
// Cache�ɕ�����i�[
//   ����:   strBufOrg=�i�[������
//   �Ԃ�l: �i�[���s�ifalse=�i�[�s�v�Atrue=�i�[�ς݁j
//---------------------------------------------------------------------
bool JlsScriptState::addCmdCache(string &strBufOrg){
	if ( (int)m_listRepDepth.size() <= 0 ){		// Repeat���ł͂Ȃ�
		return false;
	}
	//--- �L���b�V���s���`�F�b�N ---
	int size_line = (int) m_listRepCmdCache.size();
	if (size_line >= SIZE_REPLINE){			// �ő�s���`�F�b�N
		return false;
	}
	m_listRepCmdCache.push_back(strBufOrg);
	return true;
}

//---------------------------------------------------------------------
// ���ݍs�̐����Ԃ���̃R�}���h���s�L����
//   ���́F  ���ݍs�̃R�}���h���ށicategory_if=If�n�Acategory_repeat=Repeat�n�R�}���h�j
//   �Ԃ�l: �L�����ifalse=�����s�Atrue=�L���s�j
//---------------------------------------------------------------------
bool JlsScriptState::isValidCmdLine(bool category_if, bool category_repeat){
	if ((m_ifSkip && !category_if) || (m_repSkip && !category_repeat)){
		return false;
	}
	return true;
}

//---------------------------------------------------------------------
// �����s�̔���
//---------------------------------------------------------------------
bool JlsScriptState::isSkipCmd(){
	return m_ifSkip || m_repSkip;
}

///////////////////////////////////////////////////////////////////////
//
// JL�X�N���v�g���s�N���X
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// ������
//---------------------------------------------------------------------
JlsScript::JlsScript(JlsDataset *pdata){
	this->pdata  = pdata;

	// Auto�n�R�}���h���g���g�p
	m_funcAutoScript.reset(new JlsAutoScript(pdata));

	// �O�̂��ߓ����ݒ�ُ�̊m�F
	checkInitial();
}

JlsScript::~JlsScript() = default;


//---------------------------------------------------------------------
// �ϐ���ݒ�
// ���́F
//   strName   : �ϐ���
//   strVal    : �ϐ��l
//   overwrite : 0=����`���̂ݐݒ�  1=�㏑�����ݒ�
// �o�́F
//   �Ԃ�l    : �ʏ�=true�A���s��=false
//---------------------------------------------------------------------
bool JlsScript::setJlsRegVar(const string &strName, const string &strVal, bool overwrite){
	//--- �Œ���̈ᔽ�����m�F ---
	{
		string strCheckFull  = "!#$%&'()*+,-./:;<=>?";			// �ϐ�������Ƃ��Ďg�p�֎~
		string strCheckFirst = strCheckFull + "0123456789";		// �ϐ��擪�����Ƃ��Ďg�p�֎~
		string strFirst = strName.substr(0, 1);
		if ((int) strCheckFirst.find(strFirst) >= 0){
			cerr << "error: register setting, invalid first char(" << strName << ")" << endl;
			return false;
		}
		for(int i=0; i < (int)strCheckFull.length(); i++){
			string strNow = strCheckFull.substr(i, 1);
			if ((int) strName.find(strNow) >= 0){
				cerr << "error: register setting, bad char exist(" << strName << ")" << endl;
				return false;
			}
		}
	}
	//--- �ʏ�̃��W�X�^�������� ---
	bool ret = m_regvar.setRegVar(strName, strVal, overwrite);

	//--- �V�X�e���ϐ��̓��ꏈ�� ---
	int type_add = 0;
	string strAddName;
	if (_stricmp(strName.c_str(), "HEADFRAME") == 0){
		strAddName = "HEADTIME";
		type_add = 1;
	}
	else if (_stricmp(strName.c_str(), "TAILFRAME") == 0){
		strAddName = "TAILTIME";
		type_add = 1;
	}
	else if (_stricmp(strName.c_str(), "HEADTIME") == 0){
		strAddName = "HEADFRAME";
		type_add = 2;
	}
	else if (_stricmp(strName.c_str(), "TAILTIME") == 0){
		strAddName = "TAILFRAME";
		type_add = 2;
	}
	if (type_add > 0){
		int val;
		if (pdata->cnv.getStrValMsecM1(val, strVal, 0)){
			string strAddVal;
			if (type_add == 2){
				strAddVal = pdata->cnv.getStringFrameMsecM1(val);
			}
			else{
				strAddVal = pdata->cnv.getStringTimeMsecM1(val);
			}
			m_regvar.setRegVar(strAddName, strAddVal, overwrite);
		}
		//--- head/tail�����X�V ---
		{
			string strSub;
			if (getJlsRegVar(strSub, "HEADTIME", true) > 0){
				pdata->cnv.getStrValMsecM1(pdata->recHold.rmsecHeadTail.st, strSub, 0);
			}
			if (getJlsRegVar(strSub, "TAILTIME", true) > 0){
				pdata->cnv.getStrValMsecM1(pdata->recHold.rmsecHeadTail.ed, strSub, 0);
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �ϐ���ǂݏo��
// ���́F
//   strCandName : �ǂݏo���ϐ����i���j
//   excact      : 0=���͕����ɍő�}�b�`����ϐ�  1=���͕����Ɗ��S��v����ϐ�
// �o�́F
//   �Ԃ�l  : �ϐ����̕������i0�̎��͑Ή��ϐ��Ȃ��j
//   strVal  : �ϐ��l
//---------------------------------------------------------------------
int JlsScript::getJlsRegVar(string &strVal, const string &strCandName, bool exact){
	//--- �ʏ�̃��W�X�^�ǂݏo�� ---
	return m_regvar.getRegVar(strVal, strCandName, exact);
}

//---------------------------------------------------------------------
// �X�N���v�g���ŋL�ڂ���N���I�v�V����
// ���́F
//   argrest    �F�����c�萔
//   strv       �F�����R�}���h
//   str1       �F�����l�P
//   str2       �F�����l�Q
//   overwrite  �F�������ݍς݂̃I�v�V�����ݒ�ifalse=���Ȃ� true=����j
//   checklevel �F�G���[�m�F���x���i0=�Ȃ� 1=�F�������I�v�V�����`�F�b�N�j
// �o�́F
//   �Ԃ�l  �F�����擾��(-1�̎��擾�G���[�A0�̎��Y���R�}���h�Ȃ�)
//---------------------------------------------------------------------
int JlsScript::setOptionsGetOne(int argrest, const char* strv, const char* str1, const char* str2, bool overwrite){
	if (argrest <= 0){
		return 0;
	}
	bool exist2 = false;
	bool exist3 = false;
	if (argrest >= 2){
		exist2 = true;
	}
	if (argrest >= 3){
		exist3 = true;
	}
	int numarg = 0;
	if(strv[0] == '-' && strv[1] != '\0') {
		if (!_stricmp(strv, "-flags")){
			if (!exist2){
				fprintf(stderr, "-flags needs an argument\n");
				return -1;
			}
			else{
				if (setInputFlags(str1, overwrite) == false){
					fprintf(stderr, "-flags bad argument\n");
					return -1;
				}
			}
			numarg = 2;
		}
		else if (!_stricmp(strv, "-set")){
			if (!exist3){
				fprintf(stderr, "-set needs two arguments\n");
				return -1;
			}
			else{
				if (setInputReg(str1, str2, overwrite) == false){
					fprintf(stderr, "-set bad argument\n");
					return -1;
				}
			}
			numarg = 3;
		}
		else if (!_stricmp(strv, "-cutmrgin")){
			if (!exist2){
				fprintf(stderr, "-cutmrgin needs an argument\n");
				return -1;
			}
			else if (overwrite || pdata->extOpt.fixCutIn == 0){
				pdata->extOpt.msecCutIn = setOptionsCnvCutMrg(str1);
				pdata->extOpt.fixCutIn = 1;
			}
			numarg = 2;
		}
		else if (!_stricmp(strv, "-cutmrgout")){
			if (!exist2){
				fprintf(stderr, "-cutmrgout needs an argument\n");
				return -1;
			}
			else if (overwrite || pdata->extOpt.fixCutOut == 0){
				pdata->extOpt.msecCutOut = setOptionsCnvCutMrg(str1);
				pdata->extOpt.fixCutOut = 1;
			}
			numarg = 2;
		}
		else if (!_stricmp(strv, "-cutmrgwi")){
			if (!exist2){
				fprintf(stderr, "-cutmrgwi needs an argument\n");
				return -1;
			}
			else if (overwrite || pdata->extOpt.fixWidCutI == 0){
				int val = atoi(str1);
				pdata->extOpt.wideCutIn  = val;
				pdata->extOpt.fixWidCutI = 1;
			}
			numarg = 2;
		}
		else if (!_stricmp(strv, "-cutmrgwo")){
			if (!exist2){
				fprintf(stderr, "-cutmrgwo needs an argument\n");
				return -1;
			}
			else if (overwrite || pdata->extOpt.fixWidCutO == 0){
				int val = atoi(str1);
				pdata->extOpt.wideCutOut = val;
				pdata->extOpt.fixWidCutO = 1;
			}
			numarg = 2;
		}
	}
	return numarg;
}

//---------------------------------------------------------------------
// CutMrgIn / CutMrgOut �I�v�V���������p 30fps�t���[�������͂Ń~���b��Ԃ�
//---------------------------------------------------------------------
Msec JlsScript::setOptionsCnvCutMrg(const char* str){
	int num = atoi(str);
	int frac = 0;
	const char *tmpstr = strchr(str, '.');
	if (tmpstr != nullptr){
		if (tmpstr[1] >= '0' && tmpstr[1] <= '9'){
			frac = (tmpstr[1] - '0') * 10;
			if (tmpstr[2] >= '0' && tmpstr[2] <= '9'){
				frac += (tmpstr[2] - '0');
			}
		}
	}
	//--- 30fps�Œ�ϊ����� ---
	Msec msec_num  = (abs(num) * 1001 + 30/2) / 30;
	Msec msec_frac = (frac * 1001 + 30/2) / 30 / 100;
	Msec msec_result = msec_num + msec_frac;
	if (num < 0) msec_result = -1 * msec_result;
	return msec_result;
}

//---------------------------------------------------------------------
// �ϐ����O������ݒ�
// �o�́F
//   �Ԃ�l  �Ftrue=����I��  false=���s
//---------------------------------------------------------------------
bool JlsScript::setInputReg(const char *name, const char *val, bool overwrite){
	return setJlsRegVar(name, val, overwrite);
}

//---------------------------------------------------------------------
// �I�v�V�����t���O��ݒ�
// �o�́F
//   �Ԃ�l  �Ftrue=����I��  false=���s
//---------------------------------------------------------------------
bool JlsScript::setInputFlags(const char *flags, bool overwrite){
	bool ret = true;
	int pos = 0;
	string strBuf = flags;
	while(pos >= 0){
		string strFlag;
		pos = pdata->cnv.getStrWord(strFlag, strBuf, pos);
		if (pos >= 0){
			string strName, strVal;
			//--- �e�t���O�̒l��ݒ� ---
			int nloc = (int) strFlag.find(":");
			if (nloc >= 0){
				strName = strFlag.substr(0, nloc);
				strVal  = strFlag.substr(nloc+1);
			}
			else{
				strName = strFlag;
				strVal  = "1";
			}
			//--- �ϐ��i�[ ---
			bool flagtmp = setJlsRegVar(strName, strVal, overwrite);
			if (flagtmp == false) ret = false;
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �����ݒ�ُ̈�m�F
//---------------------------------------------------------------------
void JlsScript::checkInitial(){
	for(int i=0; i<SIZE_JLCMD_SEL; i++){
		if (CmdDefine[i].cmdsel != i){
			cerr << "error:internal mismatch at CmdDefine.cmdsel " << i << endl;
		}
	}
	for(int i=0; i<SIZE_CONFIG_VAR; i++){
		if (ConfigDefine[i].prmsel != i){
			cerr << "error:internal mismatch at ConfigDefine.prmsel " << i << endl;
		}
	}
	if (strcmp(OptDefine[SIZE_JLOPT_DEFINE-1].optname, "-relative") != 0){
		cerr << "error:internal mismatch at OptDefine.data1 ";
		cerr << OptDefine[SIZE_JLOPT_DEFINE-1].optname << endl;
	}
}


//=====================================================================
// �R�}���h���s�J�n����
//=====================================================================

//---------------------------------------------------------------------
// �R�}���h�J�n
// �o�́F
//   �Ԃ�l  �F0=����I�� 2=�t�@�C���ُ�
//---------------------------------------------------------------------
int JlsScript::startCmd(const string &fname){
	//--- Call���ߗp��Path�ݒ� ---
	pdata->cnv.getStrFilePath(m_pathNameJL, fname);

	//--- �V�X�e���ϐ��̏����l��ݒ� ---
	setSystemRegInit();

	//--- JL�X�N���v�g���s ---
	int errnum = startCmdLoop(fname, 0);

	//--- �f�o�b�O�p�̕\�� ---
	if (pdata->extOpt.verbose > 0 && errnum == 0){
		pdata->displayLogo();
		pdata->displayScp();
	}

	return errnum;
}


//---------------------------------------------------------------------
// �R�}���h�ǂݍ��݁E���s�J�n
// ���́F
//   fname   : �X�N���v�g�t�@�C����
//   loop    : 0=������s 1-:Call�R�}���h�ŌĂ΂ꂽ�ꍇ�̃l�X�g��
// �o�́F
//   �Ԃ�l  �F0=����I�� 2=�t�@�C���ُ�
//---------------------------------------------------------------------
int JlsScript::startCmdLoop(const string &fname, int loop){
	bool exe_command = false;			// �O��R�}���h�̎��s���

	//--- ������s ---
	if (loop == 0){
		m_exe1st = true;
	}
	//--- ����M�� ---
	JlsScriptState state;
	//--- �t�@�C���ǂݍ��� ---
	ifstream ifs(fname.c_str());
	if (ifs.fail()){
		cerr << "error: failed to open " << fname << "\n";
		return 2;
	}
	string strBufOrg;
	while( startCmdGetLine(ifs, strBufOrg, state) ){
		//--- �O�R�}���h�̎��s�L������ ---
		setSystemRegLastexe(exe_command);
		//--- �ϐ���u�� ---
		string strBuf;
		replaceBufVar(strBuf, strBufOrg, state.isSkipCmd());
//printf("%s\n",strBuf.c_str());
		//--- �f�R�[�h���� ---
		JlsCmdSet cmdset;								// �R�}���h�i�[
		int errval = decodeCmd(cmdset.arg, strBuf);			// �R�}���h���
		//--- �f�R�[�h���� ---
		JlcmdSelType      cmdsel   = cmdset.arg.cmdsel;
		JlcmdCategoryType category = cmdset.arg.category;
		bool enable_exe = true;
		//--- ���s�}�X�N���� ---
		{
			//--- ����ɂ��L���s���f ---
			bool category_if     = (category == JLCMD_CAT_COND)? true : false;
			bool category_repeat = (category == JLCMD_CAT_REP )? true : false;
			if (state.isValidCmdLine(category_if, category_repeat) == false){
				//--- ���s���Ȃ��s�̓G���[���o���Ȃ� ---
				enable_exe = false;
				errval = JLCMD_ERR_None;
			}
			//--- �G���[���͎��s���Ȃ� ---
			if (errval != JLCMD_ERR_None){
				enable_exe = false;
			}
		}
		//--- �R�}���h���� ---
		if (enable_exe){
			switch(category){
				case JLCMD_CAT_NONE:					// �R�}���h�Ȃ�
					break;
				case JLCMD_CAT_COND:					// ��������
					setCmdCondIf(cmdsel, cmdset.arg.cond.flagCond, state);
					break;
				case JLCMD_CAT_CALL:					// Call��
					errval = setCmdCall(cmdsel, strBuf, cmdset.arg.cond.posStr, loop);
					break;
				case JLCMD_CAT_REP:						// �J��Ԃ���
					errval = setCmdRepeat(cmdsel, strBuf, cmdset.arg.cond.posStr, state);
					break;
				case JLCMD_CAT_SYS:						// �V�X�e���R�}���h
					errval = setCmdSys(cmdsel, strBuf, cmdset.arg.cond.posStr);
					break;
				case JLCMD_CAT_REG:						// �ϐ��ݒ�
					errval = setCmdReg(cmdsel, strBuf, cmdset.arg.cond.posStr);
					break;
				default:								// ��ʃR�}���h
					if (m_exe1st){						// ����݂̂̃`�F�b�N
						m_exe1st = false;
						if ( pdata->isSetupAdjInitial() ){
							pdata->setFlagSetupAdj( true );
							//--- �ǂݍ��݃f�[�^������ ---
							JlsReformData func_reform(pdata);
							func_reform.adjustData();
							setSystemRegNologo(true);
						}
					}
					exe_command = exeCmd(cmdset);
					break;
			}
		}
		//--- �G���[�`�F�b�N ---
		if (errval != JLCMD_ERR_None){
			exe_command = false;
			startCmdDispErr(strBuf, errval);
		}
	}
	{						// �l�X�g�G���[�m�F
		int flags_remain = state.isRemainNest();
		if (flags_remain & 0x01){
			fprintf(stderr, "error : EndIf is not found\n");
		}
		if (flags_remain & 0x02){
			fprintf(stderr, "error : EndRepeat is not found\n");
		}
	}

	return 0;
}

//---------------------------------------------------------------------
// ���̕�����擾
// ���o�́F
//   ifs          : �t�@�C�����
//   state        : ������
// �o�́F
//   �Ԃ�l    : ������擾���ʁi0=�擾�Ȃ�  1=�擾����j
//   strBufOrg : �擾������
//---------------------------------------------------------------------
bool JlsScript::startCmdGetLine(ifstream &ifs, string &strBufOrg, JlsScriptState &state){
	bool ret = false;

	//--- �L���b�V���ǂݍ��� ---
	if ( state.readCmdCache(strBufOrg) ){
		ret = true;
	}
	//--- �ʏ�̓ǂݍ��� ---
	else{
		if ( getline(ifs, strBufOrg) ){
			ret = true;
			//--- �L���b�V���ɕۑ� ---
			state.addCmdCache(strBufOrg);
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �G���[�\��
//---------------------------------------------------------------------
void JlsScript::startCmdDispErr(const string &strBuf, int errval){
	string strErr = "";
	switch(errval){
		case JLCMD_ERR_ErrOpt:
			strErr = "error: wrong argument";
			break;
		case JLCMD_ERR_ErrRange:
			strErr = "error: wrong range argument";
			break;
		case JLCMD_ERR_ErrSEB:
			strErr = "error: need Start or End";
			break;
		case JLCMD_ERR_ErrVar:
			strErr = "error: failed variable setting";
			break;
		case JLCMD_ERR_ErrTR:
			strErr = "error: need auto command TR/SP/EC";
			break;
		case JLCMD_ERR_ErrCmd:
			strErr = "error: wrong command";
			break;
		default:
			break;
	}
	if (strErr.length() > 0){
		cerr << strErr << " in " << strBuf << "\n";
	}
}

//---------------------------------------------------------------------
// �ϐ�������u������������o��
// ���́F
//   strBuf : ������
// �o�́F
//   �Ԃ�l  �F�u�����ʁi0=����  -1=���s�j
//   dstBuf  : �o�͕�����
//---------------------------------------------------------------------
int JlsScript::replaceBufVar(string &dstBuf, const string &srcBuf, int nowarn){
	string strName, strVal;
	int len_var, pos_var;

	dstBuf.clear();
	int ret = 0;
	int pos_cmt = (int) srcBuf.find("#");
	int pos_base = 0;
	while(pos_base >= 0){
		//--- �ϐ������̒u�� ---
		pos_var = (int) srcBuf.find("$", pos_base);
		if (pos_var >= 0){
			//--- $��O�܂ł̕�������m�� ---
			if (pos_var > pos_base){
				dstBuf += srcBuf.substr(pos_base, pos_var-pos_base);
				pos_base = pos_var;
			}
			//--- �ϐ����������Ēu�� ---
			len_var = replaceRegVarInBuf(strVal, srcBuf, pos_var);
			if (len_var > 0){
				dstBuf += strVal;
				pos_base += len_var;
			}
			else{
				if (pos_var < pos_cmt || pos_cmt < 0){		// �R�����g�łȂ���Βu�����s
					ret = -1;
					if (nowarn == 0){
						cerr << "error: not defined variable in " << srcBuf << endl;
					}
				}
				pos_var = -1;
			}
		}
		//--- �ϐ����Ȃ���Ύc�肷�ׂăR�s�[ ---
		if (pos_var < 0){
			dstBuf += srcBuf.substr(pos_base);
			pos_base = -1;
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// �Ώۈʒu�̕ϐ���ǂݏo��
// ���́F
//   strBuf : ������
//   pos    : �F���J�n�ʒu
// �o�́F
//   �Ԃ�l  �F�ϐ������̕�����
//   strVal  : �ϐ��l
//---------------------------------------------------------------------
int JlsScript::replaceRegVarInBuf(string &strVal, const string &strBuf, int pos){
	int var_st, var_ed;
	bool exact;

	int ret = 0;
	if (strBuf[pos] == '$'){
		//--- �ϐ��������擾 ---
		pos ++;
		if (strBuf[pos] == '{'){		// ${�ϐ���}�t�H�[�}�b�g���̏���
			exact = true;
			pos ++;
			var_st = pos;
			while(strBuf[pos] != '}' && strBuf[pos] != '\0'){
				pos ++;
			}
		}
		else{							// $�ϐ����t�H�[�}�b�g���̏���
			exact = false;
			var_st = pos;
			while(strBuf[pos] != ' ' && strBuf[pos] != '\0'){
				pos ++;
			}
		}
		var_ed = pos;
		if (strBuf[pos] == '}' || strBuf[pos] == ' '){
			var_ed -= 1;
		}
		//--- �ϐ��ǂݏo�����s ---
		if (var_st <= var_ed){
			string strCandName = strBuf.substr(var_st, var_ed-var_st+1);
			int nmatch = getJlsRegVar(strVal, strCandName, exact);
			if (nmatch > 0){
				ret = nmatch + 1 + (exact*2);	// �ϐ����� + $ + {}
			}
		}
	}
	return ret;
}



//=====================================================================
// �f�R�[�h����
//=====================================================================

//---------------------------------------------------------------------
// �R�}���h���e�𕶎���P�s������
// �o�́F
//   �Ԃ�l�F�G���[���
//   cmdarg: �R�}���h��͌���
//---------------------------------------------------------------------
int JlsScript::decodeCmd(JlsCmdArg &cmdarg, const string &strBuf){
	int retval = JLCMD_ERR_None;

	//--- �R�}���h���e������ ---
	cmdarg.clear();
	//--- �R�}���h��t(cmdsel) ---
	string strCmd;
	int csel = 0;
	int pos = pdata->cnv.getStrItem(strCmd, strBuf, 0);
	if (pos >= 0){
		csel = decodeCmdId(strCmd);
	}
	//--- �R�}���h�ُ펞�̏I�� ---
	if (csel < 0){
		retval = JLCMD_ERR_ErrCmd;
		return retval;
	}

	//--- �R�}���h���ݒ� ---
	JlcmdSelType      cmdsel   = CmdDefine[csel].cmdsel;
	JlcmdCategoryType category = CmdDefine[csel].category;
	int mustchar  = CmdDefine[csel].mustchar;
	int mustrange = CmdDefine[csel].mustrange;
	int needopt   = CmdDefine[csel].needopt;
	cmdarg.cmdsel = cmdsel;
	cmdarg.category = category;

	//--- ���ޕʓ��� ---
	switch(category){
		case JLCMD_CAT_NONE:
			break;
		case JLCMD_CAT_COND:					// ��������
			if (cmdsel == JLCMD_SEL_If   ||
				cmdsel == JLCMD_SEL_ElsIf){
				cmdarg.cond.flagCond = getCondFlag(strBuf, pos);
			}
			else{
				cmdarg.cond.flagCond = 1;
			}
			break;
		case JLCMD_CAT_CALL:					// Call��
		case JLCMD_CAT_REP:						// ���s�[�g�R�}���h
		case JLCMD_CAT_SYS:						// �V�X�e���R�}���h
		case JLCMD_CAT_REG:						// ���W�X�^�ݒ�
			cmdarg.cond.posStr = pos;
			break;
		default:
			break;
	}

	//--- �R�}���h��� ---
	if (mustchar > 0 || mustrange > 0){
		pos = decodeCmdArgMust(cmdarg, retval, strBuf, pos, mustchar, mustrange);
	}

	//--- �I�v�V������t ---
	if (needopt > 0 && pos >= 0){
		while(pos >= 0){
			pos = decodeCmdArgOpt(cmdarg, retval, strBuf, pos);
		}
		decodeCmdTackOpt(cmdarg);
	}

	return retval;
}

//---------------------------------------------------------------------
// �R�}���h�����擾
// �o�́F
//   �Ԃ�l  �F�擾�R�}���h�ԍ��i���s����-1�j
//---------------------------------------------------------------------
int JlsScript::decodeCmdId(const string &cstr){
	int det = -1;
	const char *cmdname = cstr.c_str();

	if (cmdname[0] == '\0' || cmdname[0] == '#'){
		det = 0;
	}
	else{
		for(int i=0; i<SIZE_JLCMD_SEL; i++){
			if (_stricmp(cmdname, CmdDefine[i].cmdname) == 0){
				det = i;
				break;
			}
		}
		//--- ������Ȃ���Εʖ������� ---
		if (det < 0){
			for(int i=0; i<SIZE_JLSCR_CMDALIAS; i++){
				if (_stricmp(cmdname, CmdAlias[i].cmdname) == 0){
					det = CmdAlias[i].cmdsel;
					break;
				}
			}
		}
	}
	return det;
}

//---------------------------------------------------------------------
// �K�{�����̎擾
// ���́F
//   strBuf : ������
//   pos    : �F���J�n�ʒu
//   tpc: ��ސݒ�i0=�ݒ�Ȃ�  1=S/E/B  2=TR/SP/EC 3=�ȗ��\��S/E/B�j
//   tpw: ���Ԑݒ�i0=�ݒ�Ȃ�  1=center  3=center+left+right�j
// �o�́F
//   �Ԃ�l  : �ǂݍ��݈ʒu�i-1=�I�v�V�����ُ�j
//   errval  : �G���[�ԍ�
//   cmdarg  : �R�}���h��͌���
//---------------------------------------------------------------------
int JlsScript::decodeCmdArgMust(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos, int tpc, int tpw){
	if (tpc > 0 && pos >= 0){
		string strTmp;
		int posbak = pos;
		pos = pdata->cnv.getStrItem(strTmp, strBuf, pos);
		if (pos >= 0){
			//--- ���ڂP�i�����w��j ---
			if (tpc == 1 || tpc == 3){
				if (strTmp[0] == 'S' || strTmp[0] == 's'){
					cmdarg.selectEdge = LOGO_EDGE_RISE;
				}
				else if (strTmp[0] == 'E' || strTmp[0] == 'e'){
					cmdarg.selectEdge = LOGO_EDGE_FALL;
				}
				else if (strTmp[0] == 'B' || strTmp[0] == 'b'){
					cmdarg.selectEdge = LOGO_EDGE_BOTH;
				}
				else{
					if (tpc == 3) pos = posbak;
					else{
						pos    = -1;
						errval = JLCMD_ERR_ErrSEB;
					}
				}
			}
			else if (tpc == 2){
				const char *pstr = strTmp.c_str();
				if (!_stricmp(pstr, "TR")){
					cmdarg.selectAutoSub = JLCMD_SUB_TR;
				}
				else if (!_stricmp(pstr, "SP")){
					cmdarg.selectAutoSub = JLCMD_SUB_SP;
				}
				else if (!_stricmp(pstr, "EC")){
					cmdarg.selectAutoSub = JLCMD_SUB_EC;
				}
				else{
					pos    = -1;
					errval = JLCMD_ERR_ErrTR;
				}
			}
		}
	}
	//--- ���ڂQ�i�͈͎w��j ---
	if (tpw > 0 && pos >= 0){
		if (tpw == 1 || tpw == 3){
			WideMsec wmsec = {};
			pos = pdata->cnv.getStrValMsec(wmsec.just, strBuf, pos);
			if (tpw == 3){
				pos = pdata->cnv.getStrValMsec(wmsec.early,  strBuf, pos);
				pos = pdata->cnv.getStrValMsec(wmsec.late, strBuf, pos);
				// �J�n�ƏI�����t�̏ꍇ�͔��]
				if (wmsec.early > wmsec.late){
					Msec ms = wmsec.early;
					wmsec.early = wmsec.late;
					wmsec.late  = ms;
				}
			}
			cmdarg.wmsecDst = wmsec;
		}
		if (pos < 0){
			errval = JLCMD_ERR_ErrRange;
		}
	}

	return pos;
}

//---------------------------------------------------------------------
// �����I�v�V�����̎擾
// �o�b�t�@�c�蕔������P�ݒ������
// �o�́F
//   �Ԃ�l  : �ǂݍ��݈ʒu�i-1=�I�v�V�����ُ�j
//   errval  : �G���[�ԍ�
//   cmdarg  : �R�}���h��͌���
//---------------------------------------------------------------------
int JlsScript::decodeCmdArgOpt(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos){
	string strWord;
	pos = pdata->cnv.getStrItem(strWord, strBuf, pos);
	if (pos >= 0){
		//--- �R�����g���� ---
		int poscut = (int) strWord.find("#");
		if (poscut == 0){
			pos = -1;
		}
		else if (poscut > 0){
			strWord.substr(0, poscut);
		}
	}
	int optsel = -1;
	if (pos >= 0){
		//--- �I�v�V�������� ---
		const char *pstr = strWord.c_str();
		for(int i=0; i<SIZE_JLOPT_DEFINE; i++){
			if (!_stricmp(pstr, OptDefine[i].optname)){
				optsel = i;
			}
		}
		if (optsel < 0){
			pos = -1;
			errval = JLCMD_ERR_ErrOpt;
		}
	}
	if (pos >= 0){
		//--- �I�v�V�����ɑΉ������ݒ�擾 ---
		JlOptionCategoryType category = OptDefine[optsel].category;
		int                  numdata  = OptDefine[optsel].num;
		int                  dsort    = OptDefine[optsel].sort;
		JlOptionDataType     dselect1 = OptDefine[optsel].data1;
		JlOptionDataType     dselect2 = dselect1;
		JlOptionDataType     dselect3 = dselect1;
		switch(dselect1){
			case JLOPT_DATA_MsecFrameLeft:
				dselect2 = JLOPT_DATA_MsecFrameRight;
				break;
			case JLOPT_DATA_MsecEndlenC:
				dselect2 = JLOPT_DATA_MsecEndlenL;
				dselect3 = JLOPT_DATA_MsecEndlenR;
				break;
			case JLOPT_DATA_MsecSftC:
				dselect2 = JLOPT_DATA_MsecSftL;
				dselect3 = JLOPT_DATA_MsecSftR;
				break;
			case JLOPT_DATA_MsecTLHoldL:
				dselect2 = JLOPT_DATA_MsecTLHoldR;
				break;
			case JLOPT_DATA_MsecLenPMin:
				dselect2 = JLOPT_DATA_MsecLenPMax;
				break;
			case JLOPT_DATA_MsecLenNMin:
				dselect2 = JLOPT_DATA_MsecLenNMax;
				break;
			case JLOPT_DATA_MsecLogoExtL:
				dselect2 = JLOPT_DATA_MsecLogoExtR;
				break;
			default:
				break;
		}
		//--- �ǂݍ��� ---
		int val1, val2, val3;
		switch(category){
			case JLOPT_CAT_NUMLOGO:					// ���S�ԍ��̌���
				if (cmdarg.isSetOpt(JLOPT_DATA_TypeNLogo) == false ||
					cmdarg.getOpt(JLOPT_DATA_TypeNLogo) == numdata){
					// ��ނ�ݒ�
					cmdarg.setOpt(JLOPT_DATA_TypeNLogo, numdata);
					// �ԍ���ݒ�
					string strSub;
					pos = pdata->cnv.getStrItem(strSub, strBuf, pos);
					int possub = 0;
					string strTmp;
					while(possub >= 0){		// comma��؂�ŕ����l�ǂݍ���
						possub = pdata->cnv.getStrWord(strTmp, strSub, possub);
						if (possub >= 0){
							if (pdata->cnv.getStrValNum(val1, strTmp, 0) >= 0){
								cmdarg.addLgOpt(val1);
							}
						}
					}
				}
				break;
			case JLOPT_CAT_FRAME:					// �t���[���ʒu�ɂ�����
				pos = pdata->cnv.getStrValMsecM1(val1, strBuf, pos);
				pos = pdata->cnv.getStrValMsecM1(val2, strBuf, pos);
				if (dsort == 12){
					sortTwoValM1(val1, val2);	// ���ёւ�
				}
				cmdarg.setOpt(dselect1, val1);
				cmdarg.setOpt(dselect2, val2);
				cmdarg.setOpt(JLOPT_DATA_TypeFrame, numdata);
				break;
			case JLOPT_CAT_SC:						// ����SC�ɂ�����
				pos = pdata->cnv.getStrValMsecM1(val1, strBuf, pos);
				pos = pdata->cnv.getStrValMsecM1(val2, strBuf, pos);
				if (dsort == 12){
					sortTwoValM1(val1, val2);	// ���ёւ�
				}
				cmdarg.addScOpt(numdata, val1, val2);
				break;
			case JLOPT_CAT_MSECM1:					// �~���b���i-1�͂��̂܂܁j
				if (numdata > 0){
					pos = pdata->cnv.getStrValMsecM1(val1,  strBuf, pos);
					if (pos >= 0){
						cmdarg.setOpt(dselect1, val1);
					}
				}
				if (numdata > 1){
					pos = pdata->cnv.getStrValMsecM1(val2,  strBuf, pos);
					if (pos >= 0){
						if (dsort == 12){
							sortTwoValM1(val1, val2);	// ���ёւ�
							cmdarg.setOpt(dselect1, val1);
						}
						cmdarg.setOpt(dselect2, val2);
					}
				}
				if (numdata > 2){
					pos = pdata->cnv.getStrValMsecM1(val3,  strBuf, pos);
					if (pos >= 0){
						if (dsort == 23){
							sortTwoValM1(val2, val3);	// ���ёւ�
							cmdarg.setOpt(dselect2, val2);
						}
						cmdarg.setOpt(dselect3, val3);
					}
				}
				break;
			case JLOPT_CAT_NUM:						// ���l
				if (numdata > 0){
					pos = pdata->cnv.getStrValNum(val1, strBuf, pos);
					if (pos >= 0){
						cmdarg.setOpt(dselect1, val1);
					}
				}
				if (numdata > 1){
					cerr << "error:internal setting(OptDefine-NUM)" << strBuf << endl;
				}
				break;
			case JLOPT_CAT_SEC:						// �b��
				if (numdata > 0){
					pos = pdata->cnv.getStrValSecFromSec(val1, strBuf, pos);
					if (pos >= 0){
						cmdarg.setOpt(dselect1, val1);
					}
				}
				if (numdata > 1){
					cerr << "error:internal setting(OptDefine-SEC)" << strBuf << endl;
				}
				break;
			case JLOPT_CAT_FLAG:					// �t���O
				if (numdata == 0){
					cmdarg.setOpt(dselect1, 1);
				}
				else{
					cerr << "error:internal setting(OptDefine-FLAG)" << strBuf << endl;
				}
				break;
			default:
				cerr << "error:internal setting(OptDefine)" << strBuf << endl;
				cerr << "(optsel, category)" << optsel << "," << category << endl;
				break;
		}
		//--- �����s�����̃G���[ ---
		if (pos < 0){
			errval = JLCMD_ERR_ErrOpt;
		}
	}

	return pos;
}

//---------------------------------------------------------------------
// �R�}���h�I�v�V�������e������s�I�v�V�����̐ݒ�
// �o�́F
//   cmdarg.tack  : �R�}���h��͌���
//---------------------------------------------------------------------
void JlsScript::decodeCmdTackOpt(JlsCmdArg &cmdarg){
	JlcmdSelType      cmdsel    = cmdarg.cmdsel;
	JlcmdCategoryType category  = CmdDefine[cmdsel].category;
	bool floatbase = (CmdDefine[cmdsel].floatbase != 0)? true : false;
	bool vtlogo    = false;
	bool igncomp   = false;
	bool limbylogo = false;
	bool onepoint  = false;
	bool needauto  = false;
	//--- ��r�ʒu��Ώۈʒu�ɕύX ---
	if (cmdarg.isSetOpt(JLOPT_DATA_MsecSftC) ||			// -shift
		cmdarg.getOpt(JLOPT_DATA_FlagRelative)){		// -relative
		floatbase = true;
	}
	//--- ���S�𐄑��ʒu�ɕύX ---
	if (category == JLCMD_CAT_AUTO ||
		category == JLCMD_CAT_AUTOEACH){				// Auto�n
		vtlogo = true;
	}
	if (category == JLCMD_CAT_AUTOLOGO &&				// ���S������Auto�n
		cmdarg.getOpt(JLOPT_DATA_TypeNLogo) != CMDARG_LG_NLOGO){	// -Nlogo�ȊO
		vtlogo = true;
	}
	//--- ���S�m���Ԃł����s����R�}���h ---
	if (cmdsel == JLCMD_SEL_MkLogo  ||
		cmdsel == JLCMD_SEL_DivLogo ||
		cmdsel == JLCMD_SEL_GetPos){
		igncomp = true;
	}
	//--- �O��̃��S�ʒu�ȓ��ɔ͈͌��肷��ꍇ�i��Βl�ʒu�w���DivLogo�R�}���h�j ---
	if (cmdsel == JLCMD_SEL_DivLogo){
		limbylogo = true;
	}
	if ( cmdarg.isSetOpt(JLOPT_DATA_MsecFromAbs)  ||	// -fromabs
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromHead) ||	// -fromhead
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromTail) ||	// -fromtail
		 cmdarg.getOpt(JLOPT_DATA_FlagAutoChg) ){		// -autochg
		limbylogo = true;
	}
	//--- ��Έʒu�w�莞�̃��S�����͂P�ӏ��݂̂ɂ��� ---
	if ( cmdarg.isSetOpt(JLOPT_DATA_MsecFromAbs)  ||	// -fromabs
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromHead) ||	// -fromhead
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromTail) ){	// -fromtail
		onepoint = true;
	}
	if (cmdsel == JLCMD_SEL_GetPos){	// �P�ӏ��̂݌���
		onepoint = true;
	}
	//--- Auto�\����K�v�Ƃ���R�}���h ---
	int numlist = cmdarg.sizeScOpt();
	if (numlist > 0){
		for(int i=0; i<numlist; i++){
			JlOptionArgScType sctype = cmdarg.getScOptType(i);
			if (sctype == CMDARG_SC_AC || sctype == CMDARG_SC_NOAC){
				needauto = true;
			}
		}
	}
	//--- �e���S�ʃI�v�V������Auto�R�}���h ---
	if (cmdsel == JLCMD_SEL_AutoCut ||
		cmdsel == JLCMD_SEL_AutoAdd){
		if (cmdarg.getOpt(JLOPT_DATA_FlagAutoEach) > 0){
			category = JLCMD_CAT_AUTOEACH;
		}
	}
	//--- �ݒ�i�[ ---
	cmdarg.tack.floatBase   = floatbase;
	cmdarg.tack.virtualLogo = vtlogo;
	cmdarg.tack.ignoreComp  = igncomp;
	cmdarg.tack.limitByLogo = limbylogo;
	cmdarg.tack.onePoint    = onepoint;
	cmdarg.tack.needAuto    = needauto;
	cmdarg.category         = category;		// �I�v�V����(-autoeach)�ɂ��R�}���h�̌n�ύX
}



//=====================================================================
// �ϐ��W�J���܂߂������񏈗�
//=====================================================================

//---------------------------------------------------------------------
// ������Ώۈʒu�ȍ~�̃t���O�𔻒�
// ���́F
//   strBuf : ������
//   pos    : �F���J�n�ʒu
// �o�́F
//   �Ԃ�l  �F�t���O����i0=false  1=true�j
//---------------------------------------------------------------------
int JlsScript::getCondFlag(const string &strBuf, int pos){
	string strItem;
	string strCalc = "";
	string strBufRev = strBuf;
	//--- �R�����g�J�b�g ---
	int ntmp = (int) strBuf.find("#");
	if (ntmp >= 0){
		strBufRev = strBuf.substr(0, ntmp);
	}
	//--- �P�P�ꂸ�m�F ---
	while(pos >= 0){
		pos = pdata->cnv.getStrItem(strItem, strBufRev, pos);
		if (pos >= 0){
			getCondFlagConnectWord(strCalc, strItem);
		}
	}
	int pos_calc = 0;
	int val;
	if ((int)strCalc.find(":") >= 0 || (int)strCalc.find(".") >= 0){		// ���ԕ\�L�������ꍇ
		pos_calc = pdata->cnv.getStrValMsec(val, strCalc, 0);	// ���ԒP�ʂŔ�r
	}
	else{
		pos_calc = pdata->cnv.getStrValNum(val, strCalc, 0);	// strCalc�̐擪����擾
	}
	if (pos_calc < 0){
		val = 0;
		cerr << "error: can not evaluate(" << strCalc << ") in " << strBuf << endl;
	}
	int flag = (val != 0)? 1 : 0;
	return flag;
}

//---------------------------------------------------------------------
// �t���O�p�ɕ������A��
// ���o�́F
//   strCalc : �A���敶����
// ���́F
//   strItem : �ǉ�������
//---------------------------------------------------------------------
void JlsScript::getCondFlagConnectWord(string &strCalc, const string &strItem){

	//--- �A�������̒ǉ��i��r���Z�q���Q���ԂɂȂ����OR(||)��ǉ�����j ---
	char chPrevBack  = strCalc.back();
	char chNextFront = strItem.front();
	char chNextFr2   = strItem[1];
	if (strCalc.length() > 0 && strItem.length() > 0){
		if (chPrevBack  != '=' && chPrevBack  != '<' && chPrevBack  != '>' &&
			chPrevBack  != '|' && chPrevBack  != '&' &&
			chNextFront != '|' && chNextFront != '&' &&
			chNextFront != '=' && chNextFront != '<' && chNextFront != '>'){
			if (chNextFront == '!' && chNextFr2 == '='){
			}
			else{
				strCalc += "||";
			}
		}
	}
	//--- ���]���Z�̔��� ---
	string strRemain;
	if (chNextFront == '!'){
		strCalc += "!";
		strRemain = strItem.substr(1);
	}
	else{
		strRemain = strItem;
	}
	//--- �t���O�ϐ��̔��� ---
	char chFront = strRemain.front();
	if ((chFront >= 'A' && chFront <= 'Z') || (chFront >= 'a' && chFront <= 'z')){
		string strVal;
		//--- �ϐ�����t���O�̒l���擾 ---
		int nmatch = getJlsRegVar(strVal, strRemain, true);
		if (nmatch > 0 && strVal != "0"){	// �ϐ������݂���0�ȊO�̏ꍇ
			strVal = "1";
		}
		else{
			strVal = "0";
		}
		strCalc += strVal;				// �t���O�̒l�i0�܂���1�j��ǉ�
	}
	else{
		strCalc += strRemain;			// �ǉ�����������̂܂ܒǉ�
	}
//printf("(rstr:%s)",strCalc.c_str());
}



//=====================================================================
// �V�X�e���ϐ��ݒ�
//=====================================================================

//---------------------------------------------------------------------
// �����ݒ�ϐ�
//---------------------------------------------------------------------
void JlsScript::setSystemRegInit(){
	setSystemRegHeadtail(-1, -1);
	setSystemRegUpdate();
}

//---------------------------------------------------------------------
// �����ݒ�ϐ��̌��ݒl�ɂ��ύX
//---------------------------------------------------------------------
void JlsScript::setSystemRegUpdate(){
	int n = pdata->getMsecTotalMax();
	string str_val = pdata->cnv.getStringFrameMsecM1(n);
	string str_time = pdata->cnv.getStringTimeMsecM1(n);
	setJlsRegVar("MAXFRAME", str_val, true);
	setJlsRegVar("MAXTIME", str_time, true);
	setJlsRegVar("NOLOGO", to_string(pdata->extOpt.flagNoLogo), true);
}

//---------------------------------------------------------------------
// HEADFRAME/TAILFRAME��ݒ�
//---------------------------------------------------------------------
void JlsScript::setSystemRegHeadtail(int headframe, int tailframe){
	string str_head = pdata->cnv.getStringTimeMsecM1(headframe);
	string str_tail = pdata->cnv.getStringTimeMsecM1(tailframe);
	setJlsRegVar("HEADTIME", str_head, true);
	setJlsRegVar("TAILTIME", str_tail, true);
}

//---------------------------------------------------------------------
// �V�X�e���ϐ��̌��ݒl�ɂ��ύX
//---------------------------------------------------------------------
void JlsScript::setSystemRegPoshold(int msec_pos){
	string str_val = pdata->cnv.getStringTimeMsecM1(msec_pos);
	setJlsRegVar("POSHOLD", str_val, true);
}

//---------------------------------------------------------------------
// �V�X�e���ϐ��̌��ݒl�ɂ��ύX
//---------------------------------------------------------------------
void JlsScript::setSystemRegListhold(int msec_pos, bool clear){
	string str_list = "";
	if (clear == false){
		getJlsRegVar(str_list, "LISTHOLD", true);
	}
	if (msec_pos != -1){
		if (str_list.length() > 0){
			str_list += ",";
		}
		string str_val = pdata->cnv.getStringTimeMsecM1(msec_pos);
		str_list += str_val;
	}
	setJlsRegVar("LISTHOLD", str_list, true);
}


//---------------------------------------------------------------------
// �����ȃ��S�̊m�F
//---------------------------------------------------------------------
void JlsScript::setSystemRegNologo(bool need_check){
	bool flag_nologo = false;
	//--- ���S���Ԃ��ɒ[�ɏ��Ȃ��ꍇ�Ƀ��S����������ꍇ�̏��� ---
	if (need_check == true && pdata->extOpt.flagNoLogo == 0){
		int msec_sum = 0;
		int nrf_rise = -1;
		int nrf_fall = -1;
		do{
			nrf_rise = pdata->getNrfNextLogo(nrf_fall, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
			nrf_fall = pdata->getNrfNextLogo(nrf_rise, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
			if (nrf_rise >= 0 && nrf_fall >= 0){
				msec_sum += pdata->getMsecLogoNrf(nrf_fall) - pdata->getMsecLogoNrf(nrf_rise);
			}
		} while(nrf_rise >= 0 && nrf_fall >= 0);
		if (msec_sum < pdata->getConfig(CONFIG_VAR_msecWLogoSumMin)){
			flag_nologo = true;
		}
	}
	else{		// �`�F�b�N�Ȃ��Ń��S�����̏ꍇ
			flag_nologo = true;
	}
	if (flag_nologo == true){
		// ���S�ǂݍ��݂Ȃ��ɕύX
		pdata->extOpt.flagNoLogo = 1;
		// �V�X�e���ϐ����X�V
		setJlsRegVar("NOLOGO", "1", true);	// �㏑������"1"�ݒ�
	}
}

//---------------------------------------------------------------------
// �O��̎��s��Ԃ�ݒ�
//---------------------------------------------------------------------
void JlsScript::setSystemRegLastexe(bool exe_command){
	//--- �O��̎��s��Ԃ�ϐ��ɐݒ� ---
	setJlsRegVar("LASTEXE", to_string(exe_command), true);
	//--- �����ϐ��ɐݒ� ---
	m_lastexe = exe_command;
}

//---------------------------------------------------------------------
// �X�N���v�g���ŋL�ڂ���N���I�v�V����
// ���́F
//   strBuf     �F�I�v�V�������܂ޕ�����
//   pos        �F�ǂݍ��݊J�n�ʒu
//   overwrite  �F�������ݍς݂̃I�v�V�����ݒ�ifalse=���Ȃ� true=����j
// �o�́F
//   �Ԃ�l  �F0=����I�� -1=�ݒ�G���[
//---------------------------------------------------------------------
int JlsScript::setSystemRegOptions(const string &strBuf, int pos, bool overwrite){
	//--- �������؂�F�� ---
	vector <string> listarg;
	string strWord;
	while(pos >= 0){
		pos = pdata->cnv.getStrItem(strWord, strBuf, pos);
		if (pos >= 0){
			listarg.push_back(strWord);
		}
	}
	int argc = (int) listarg.size();
	if (argc <= 0){
		return 0;
	}
	//--- �X�N���v�g���Őݒ�\�ȃI�v�V���� ---
	int i = 0;
	while(i >= 0 && i < argc){
		int argrest = argc - i;
		const char* strv = listarg[i].c_str();
		const char* str1 = nullptr;
		const char* str2 = nullptr;
		if (argrest >= 2){
			str1 = listarg[i+1].c_str();
		}
		if (argrest >= 3){
			str2 = listarg[i+2].c_str();
		}
		int numarg = setOptionsGetOne(argrest, strv, str1, str2, overwrite);
		if (numarg < 0){
			return -1;
		}
		if (numarg > 0){
			i += numarg;
		}
		else{		// ���s�\�R�}���h�łȂ���Ύ��Ɉڍs
			i ++;
		}
	}
	return 0;
}



//=====================================================================
// �ݒ�R�}���h����
//=====================================================================

//---------------------------------------------------------------------
// If������
//---------------------------------------------------------------------
void JlsScript::setCmdCondIf(JlcmdSelType cmdsel, bool flag_cond, JlsScriptState &state){
	int errno;
	switch(cmdsel){
		case JLCMD_SEL_If:						// If��
			state.ifBegin(flag_cond);
			break;
		case JLCMD_SEL_EndIf:					// EndIf��
			errno = state.ifEnd();
			if (errno > 0){
				fprintf(stderr, "error: too many EndIf.\n");
			}
			break;
		case JLCMD_SEL_Else:					// Else��
		case JLCMD_SEL_ElsIf:					// ElsIf��
			errno = state.ifElse(flag_cond);
			if (errno > 0){
				fprintf(stderr, "error: not exist 'If' but exist 'Else/ElsIf' .\n");
			}
			break;
		default:
			break;
	}
}

//---------------------------------------------------------------------
// Call����
//---------------------------------------------------------------------
int JlsScript::setCmdCall(JlcmdSelType cmdsel, const string &strBuf, int pos, int loop){
	int errval = JLCMD_ERR_None;

	if (cmdsel == JLCMD_SEL_Call){
		string strTmp;
		pos = pdata->cnv.getStrItem(strTmp, strBuf, pos);
		if (pos >= 0){
			string strFileName = m_pathNameJL + strTmp;
			loop ++;
			if (loop < SIZE_CALL_LOOP){				// �ċA�Ăяo���͉񐔐���
				startCmdLoop(strFileName, loop);
			}
			else{
				// �����Ăяo���ɂ��o�b�t�@�I�[�o�[�t���[�h�~�̂���
				fprintf(stderr, "error: many recursive call(%s)\n", strBuf.c_str());
			}
		}
		else{
			fprintf(stderr, "error: wrong argument(%s)\n", strBuf.c_str());
		}
	}
	return errval;
}

//---------------------------------------------------------------------
// ���s�[�g�R�}���h����
//---------------------------------------------------------------------
int JlsScript::setCmdRepeat(JlcmdSelType cmdsel, const string &strBuf, int pos, JlsScriptState &state){
	int errval = JLCMD_ERR_None;
	switch(cmdsel){
		case JLCMD_SEL_Repeat:				// Repeat��
			{
				int val;
				pos = pdata->cnv.getStrValNum(val, strBuf, pos);
				if (pos >= 0){
					int errno;
					errno = state.repeatBegin(val);
					if (errno > 0){
						fprintf(stderr, "error: overflow at Repeat(%s)\n", strBuf.c_str());
						errval = JLCMD_ERR_ErrCmd;
					}
				}
				else{
					fprintf(stderr, "error: need number argument(%s)\n", strBuf.c_str());
					errval = JLCMD_ERR_ErrVar;
				}
			}
			break;
		case JLCMD_SEL_EndRepeat:			// EndRepeat��
			{
				int errno;
				errno = state.repeatEnd();
				if (errno > 0){
					fprintf(stderr, "error: Repeat - EndRepeat not match\n");
				}
			}
			break;
		default:
			break;
	}
	return errval;
}

//---------------------------------------------------------------------
// �V�X�e���֘A�R�}���h����
//---------------------------------------------------------------------
int JlsScript::setCmdSys(JlcmdSelType cmdsel, const string &strBuf, int pos){
	int errval = JLCMD_ERR_None;
	int val = 0;

	switch (cmdsel){
		case JLCMD_SEL_Echo:
			while( strBuf[pos] == ' ' ) pos++;
			cout << strBuf.substr(pos) << endl;
			break;
		case JLCMD_SEL_LogoOff:
			setSystemRegNologo(false);
			break;
		case JLCMD_SEL_OldAdjust:
			pos = pdata->cnv.getStrValNum(val, strBuf, pos);
			if (pos > 0){
				pdata->extOpt.oldAdjust = val;
			}
			else{
				fprintf(stderr, "error: need value for OldAdjust %s\n", strBuf.c_str());
			}
			break;
		default:
			cerr << "error:internal setting(SysCmd)" << strBuf << endl;
			break;
	}
	return errval;
}

//---------------------------------------------------------------------
// ���W�X�^�ݒ�֘A����
//---------------------------------------------------------------------
int JlsScript::setCmdReg(JlcmdSelType cmdsel, const string &strBuf, int pos){
	int errval = JLCMD_ERR_None;
	string strName, strVal;
	int val;

	switch(cmdsel){
		case JLCMD_SEL_Set:
		case JLCMD_SEL_Default:
			pos = pdata->cnv.getStrItem(strName, strBuf, pos);
			pos = pdata->cnv.getStrItem(strVal,  strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				bool overwrite = (cmdsel == JLCMD_SEL_Default)? false : true;
				if (setJlsRegVar(strName, strVal, overwrite)){
					errval = JLCMD_ERR_None;
				}
			}
			break;
		case JLCMD_SEL_EvalFrame:
			pos = pdata->cnv.getStrItem(strName, strBuf, pos);
			pos = pdata->cnv.getStrValMsecM1(val,  strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				strVal = pdata->cnv.getStringFrameMsecM1(val);
				if (setJlsRegVar(strName, strVal, true)){
					errval = JLCMD_ERR_None;
				}
			}
			break;
		case JLCMD_SEL_EvalTime:
			pos = pdata->cnv.getStrItem(strName, strBuf, pos);
			pos = pdata->cnv.getStrValMsecM1(val,  strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				strVal = pdata->cnv.getStringTimeMsecM1(val);
				if (setJlsRegVar(strName, strVal, true)){
					errval = JLCMD_ERR_None;
				}
			}
			break;
		case JLCMD_SEL_EvalNum:
			pos = pdata->cnv.getStrItem(strName, strBuf, pos);
			pos = pdata->cnv.getStrValNum(val,  strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				strVal = std::to_string(val);
				if (setJlsRegVar(strName, strVal, true)){
					errval = JLCMD_ERR_None;
				}
			}
			break;
		case JLCMD_SEL_SetParam:
			pos = pdata->cnv.getStrItem(strName, strBuf, pos);
			pos = pdata->cnv.getStrItem(strVal,  strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				errval = setCmdRegParam(strName, strVal);
			}
			break;
		case JLCMD_SEL_OptSet:
		case JLCMD_SEL_OptDefault:
			if (pos >= 0){
				bool overwrite = (cmdsel == JLCMD_SEL_OptDefault)? false : true;
				setSystemRegOptions(strBuf, pos, overwrite);
			}
			break;
		case JLCMD_SEL_UnitSec:					// ���背�W�X�^�ݒ�
			pos = pdata->cnv.getStrValNum(val, strBuf, pos);
			errval = JLCMD_ERR_ErrVar;
			if (pos >= 0){
				if (cmdsel == JLCMD_SEL_UnitSec){
					pdata->cnv.changeUnitSec(val);
					setSystemRegUpdate();
					errval = JLCMD_ERR_None;
				}
			}
			break;
		default:
			cerr << "error:internal setting(RegCmd)" << strBuf << endl;
			break;
	}
	return errval;
}

//---------------------------------------------------------------------
// SetParam�R�}���h�ɂ��p�����[�^�ݒ�
//---------------------------------------------------------------------
int JlsScript::setCmdRegParam(const string &strName, const string &strVal){
	int errval = JLCMD_ERR_None;
	const char *varname = strName.c_str();

	//--- �����񂩂�p�����[�^������ ---
	int csel = -1;
	for(int i=0; i<SIZE_CONFIG_VAR; i++){
		if (_stricmp(varname, ConfigDefine[i].namestr) == 0){
			csel = i;
			break;
		}
	}
	//--- �Ή��p�����[�^�ݒ� ---
	if (csel >= 0){
		ConfigVarType    prmsel   = ConfigDefine[csel].prmsel;
		ConfigStrValType type_val = ConfigDefine[csel].valsel;
		int pos = 0;
		int val;
		switch(type_val){
			case CONFIG_STR_MSEC:
				pos = pdata->cnv.getStrValMsec(val, strVal, pos);
				break;
			case CONFIG_STR_MSECM1:
				pos = pdata->cnv.getStrValMsecM1(val, strVal, pos);
				break;
			case CONFIG_STR_SEC:
				pos = pdata->cnv.getStrValSecFromSec(val, strVal, pos);
				break;
			case CONFIG_STR_NUM:
				pos = pdata->cnv.getStrValNum(val, strVal, pos);
				break;
			default:
				pos = -1;
				break;
		}
		if (pos >= 0){
			pdata->setConfig(prmsel, val);
		}
		else{
			errval = JLCMD_ERR_ErrVar;
		}
	}
	else{
		errval = JLCMD_ERR_ErrVar;
	}
	return errval;
}



//=====================================================================
// �R�}���h���s����
//=====================================================================

//---------------------------------------------------------------------
// �X�N���v�g�e�s�̃R�}���h���s
//---------------------------------------------------------------------
bool JlsScript::exeCmd(JlsCmdSet &cmdset){
	//--- �R�}���h���s�̊m�F�t���O ---
	bool valid_exe = true;				// ����̎��s
	bool exe_command = false;			// ���s���
	//--- �O�R�}���h���s�ς݂��m�F (-else option) ---
	if (cmdset.arg.getOpt(JLOPT_DATA_FlagElse) > 0){
		if (m_lastexe){					// ���O�R�}���h�����s�����ꍇ
			valid_exe = false;			// ����R�}���h�͎��s���Ȃ���
			exe_command = true;			// ���s�ς݈���
		}
	}
	//--- �O�R�}���h���s�ς݂��m�F (-cont option) ---
	if (cmdset.arg.getOpt(JLOPT_DATA_FlagCont) > 0){
		if (m_lastexe == false){		// ���O�R�}���h�����s���Ă��Ȃ��ꍇ
			valid_exe = false;			// ����R�}���h�����s���Ȃ�
		}
	}
	//--- �R�}���h���s ---
	if (valid_exe){
		//--- ���ʐݒ� ---
		limitHeadTail(cmdset);
		limitWindow(cmdset);						// -F�n�I�v�V�����ݒ�
		limitListForTarget(cmdset);					// -TLhold�ɂ��͈͐ݒ�

		//--- �I�v�V������Auto�n�\�����K�v�ȏꍇ(-AC -NoAC)�̍\���쐬 ---
		if ( cmdset.arg.tack.needAuto ){
			exeCmdCallAutoSetup(cmdset);
		}
		//--- ���ޕʂɃR�}���h���s ---
		JlcmdCategoryType category = cmdset.arg.category;
		switch(category){
			case JLCMD_CAT_AUTO:
				exe_command = exeCmdCallAutoScript(cmdset);		// Auto�����N���X�Ăяo��
				break;
			case JLCMD_CAT_AUTOEACH:
				exe_command = exeCmdAutoEach(cmdset);			// �e���S���Ԃ�Auto�n����
				break;
			case JLCMD_CAT_LOGO:
			case JLCMD_CAT_AUTOLOGO:
				exe_command = exeCmdLogo(cmdset);				// ���S�ʂɎ��s
				break;
			case JLCMD_CAT_NEXT:
				exe_command = exeCmdNextTail(cmdset);			// ���̈ʒu�擾����
				break;
			default:
				break;
		}
	}

	return exe_command;
}

//---------------------------------------------------------------------
// AutoScript�g�������s
//---------------------------------------------------------------------
//--- �R�}���h��͌�̎��s ---
bool JlsScript::exeCmdCallAutoScript(JlsCmdSet &cmdset){
	bool setup_only = false;
	return exeCmdCallAutoMain(cmdset, setup_only);
}
//--- �R�}���h��͂̂� ---
bool JlsScript::exeCmdCallAutoSetup(JlsCmdSet &cmdset){
	bool setup_only = true;
	return exeCmdCallAutoMain(cmdset, setup_only);
}

//--- ���s���C������ ---
bool JlsScript::exeCmdCallAutoMain(JlsCmdSet &cmdset, bool setup_only){
	//--- ����̂ݎ��s ---
	if ( pdata->isAutoModeInitial() ){
		//--- ���S�g�p���x����ݒ� ---
		if (pdata->isExistLogo() == false){		// ���S���Ȃ��ꍇ�̓��S�Ȃ��ɐݒ�
			pdata->setLevelUseLogo(CONFIG_LOGO_LEVEL_UNUSE_ALL);
		}
		else{
			int level = pdata->getConfig(CONFIG_VAR_LogoLevel);
			if (level <= CONFIG_LOGO_LEVEL_DEFAULT){		// ���ݒ莞�͒l��ݒ�
				level = CONFIG_LOGO_LEVEL_USE_HIGH;
			}
			pdata->setLevelUseLogo(level);
		}
		if (pdata->isUnuseLogo()){				// ���S�g�p���Ȃ��ꍇ
			pdata->extOpt.flagNoLogo = 1;		// ���S�Ȃ��ɐݒ�
			setSystemRegUpdate();				// NOLOGO�X�V
		}
	}
	//--- Auto�R�}���h���s ---
	return m_funcAutoScript->startCmd(cmdset, setup_only);		// AutoScript�N���X�Ăяo��
}

//---------------------------------------------------------------------
// �e���S���Ԃ�͈͂Ƃ��Ď��s����Auto�R�}���h (-autoeach�I�v�V����)
//---------------------------------------------------------------------
bool JlsScript::exeCmdAutoEach(JlsCmdSet &cmdset){
	bool exeflag_total = false;
	NrfCurrent logopt = {};
	while( pdata->getNrfptNext(logopt, LOGO_SELECT_VALID) ){
		RangeMsec rmsec_logo;
		LogoResultType rtype_rise;
		LogoResultType rtype_fall;
		//--- �m���Ԃ��m�F ---
		pdata->getResultLogoAtNrf(rmsec_logo.st, rtype_rise, logopt.nrfRise);
		pdata->getResultLogoAtNrf(rmsec_logo.ed, rtype_fall, logopt.nrfFall);
		//--- �m�莞�ȊO�͌��ꏊ�ɂ��� ---
		if (rtype_rise != LOGO_RESULT_DECIDE){
			rmsec_logo.st = logopt.msecRise;
		}
		if (rtype_fall != LOGO_RESULT_DECIDE){
			rmsec_logo.ed = logopt.msecFall;
		}
		//--- �e���S���Ԃ�͈͂Ƃ��Ĉʒu��ݒ� ---
		limitHeadTailImm(cmdset, rmsec_logo);
		limitWindow(cmdset);						// �͈͂��ēx����
		//--- Auto�R�}���h���s ---
		int exeflag = exeCmdCallAutoScript(cmdset);	// Auto�����N���X�Ăяo��
		//--- ���s���Ă�������s�t���O�ݒ� ---
		if (exeflag){
			exeflag_total = true;
		}
	};
	return exeflag_total;
}

//---------------------------------------------------------------------
// �S���S�̒��őI�����S�����s
//---------------------------------------------------------------------
bool JlsScript::exeCmdLogo(JlsCmdSet &cmdset){
	//--- ���S�ԍ��I�v�V��������L���ȃ��S�ԍ��ʒu�����ׂĎ擾 ---
	int nmax_list = limitLogoList(cmdset);
	//--- �P�ӏ������̃R�}���h�� ---
	bool flag_onepoint = cmdset.arg.tack.onePoint;
	//--- �P�ӏ������R�}���h�̏ꍇ�͈�ԋ߂���₾����L���ɂ��� ---
	int nlist_base = -1;
	if (flag_onepoint){
		int difmsec_base = 0;
		for(int i=0; i<nmax_list; i++){
			//--- ��������𖞂����Ă��郍�S���m�F ---
			bool exeflag = exeCmdLogoCheckTarget(cmdset, i);
			if (exeflag){
				WideMsec wmsec = cmdset.limit.getTargetRangeWide();
				Msec msec_target = wmsec.just;
				Nsc  nsc_sel = cmdset.limit.getResultTargetSel();
				Msec msec_sel = pdata->getMsecScp(nsc_sel);
				Msec difmsec_sel = abs(msec_target - msec_sel);
				if (difmsec_sel < difmsec_base || nlist_base < 0){
					nlist_base = i;
					difmsec_base = difmsec_sel;
				}
			}
		}
	}
	//--- ���X�g�쐬���͊J�n�O�ɓ��e���� ---
	if (cmdset.arg.cmdsel == JLCMD_SEL_GetList){
		setSystemRegListhold(-1, true);				// �ϐ�($LISTHOLD)�N���A
	}
	//--- �e�L�����S�����s ---
	bool exeflag_total = false;
	for(int i=0; i<nmax_list; i++){
		if (flag_onepoint == false || nlist_base == i){
			//--- ��������𖞂����Ă��郍�S���m�F ---
			bool exeflag = exeCmdLogoCheckTarget(cmdset, i);
			//--- ���s���� ---
			if (exeflag){
				switch(cmdset.arg.category){
					case JLCMD_CAT_LOGO :
						//--- ���S�ʒu�𒼐ڐݒ肷��R�}���h ---
						exeflag = exeCmdLogoTarget(cmdset);
						break;
					case JLCMD_CAT_AUTOLOGO :
						//--- �����\�����琶������R�}���h ---
						exeflag = exeCmdCallAutoScript(cmdset);
						break;
					default:
						break;
				}
			}
			//--- ���s���Ă�������s�t���O�ݒ� ---
			if (exeflag){
				exeflag_total = true;
			}
		}
	}

	return exeflag_total;
}

//---------------------------------------------------------------------
// �Ώۃ��S�ɂ��Đ���������������đΏۈʒu�擾
//---------------------------------------------------------------------
bool JlsScript::exeCmdLogoCheckTarget(JlsCmdSet &cmdset, int nlist){
	//--- ����S��I�� ---
	bool exeflag = limitTargetLogo(cmdset, nlist);
	//--- �����Ώ۔͈͂�ݒ�i����S�ʒu���x�[�X�j ---
	if (exeflag){
		exeflag = limitTargetRangeByLogo(cmdset);
	}
	//--- �^�[�Q�b�g�Ɉ�ԋ߂��V�[���`�F���W�ʒu���擾 ---
	if (exeflag){
		getTargetPoint(cmdset);
	}
	return exeflag;
}

//---------------------------------------------------------------------
// ���S�ʒu�ʂ̎��s�R�}���h
//---------------------------------------------------------------------
bool JlsScript::exeCmdLogoTarget(JlsCmdSet &cmdset){
	bool exe_command = false;
	Msec msec_force   = cmdset.limit.getTargetRangeForce();
	Nsc nsc_scpos_sel = cmdset.limit.getResultTargetSel();
	Nsc nsc_scpos_end = cmdset.limit.getResultTargetEnd();
	Nrf nrf = cmdset.limit.getLogoBaseNrf();

	switch(cmdset.arg.cmdsel){
		case JLCMD_SEL_Find:
			if (nsc_scpos_sel >= 0){
				if (cmdset.arg.getOpt(JLOPT_DATA_FlagAutoChg) > 0){	// �����\���ɔ��f
					exeCmdCallAutoScript(cmdset);					// Auto�����N���X�Ăяo��
				}
				else{								// �]���\���ɔ��f
					Msec msec_tmp = pdata->getMsecScpEdge(nsc_scpos_sel, jlsd::edgeFromNrf(nrf));
					pdata->setResultLogoAtNrf(msec_tmp, LOGO_RESULT_DECIDE, nrf);
				}
				exe_command = true;
			}
			break;
		case JLCMD_SEL_MkLogo:
			if (nsc_scpos_sel >= 0 && nsc_scpos_end >= 0){
				int msec_st, msec_ed;
				if (nsc_scpos_sel < nsc_scpos_end){
					msec_st = pdata->getMsecScp(nsc_scpos_sel);
					msec_ed = pdata->getMsecScpBk(nsc_scpos_end);
				}
				else{
					msec_st = pdata->getMsecScp(nsc_scpos_end);
					msec_ed = pdata->getMsecScpBk(nsc_scpos_sel);
				}
				int nsc_ins = pdata->insertLogo(msec_st, msec_ed,
								cmdset.arg.getOpt(JLOPT_DATA_FlagOverlap),
								cmdset.arg.getOpt(JLOPT_DATA_FlagConfirm),
								cmdset.arg.getOpt(JLOPT_DATA_FlagUnit));
				exe_command = (nsc_ins >= 0)? true : false;
			}
			break;
		case JLCMD_SEL_DivLogo:
			if (nsc_scpos_sel >= 0 || msec_force >= 0){
				//--- �Ώۈʒu�擾 ---
				Msec msec_target;
				Msec msec_st, msec_ed;
				if (nsc_scpos_sel >= 0){
					msec_target = pdata->getMsecScp(nsc_scpos_sel);
				}
				else{
					msec_target = msec_force;
				}
				if (jlsd::isLogoEdgeRiseFromNrf(nrf)){		// start edge
					msec_st = pdata->getMsecLogoNrf(nrf);
					msec_ed = pdata->cnv.getMsecAdjustFrmFromMsec(msec_target, -1);
				}
				else{										// end edge
					msec_st = pdata->cnv.getMsecAlignFromMsec(msec_target);
					msec_ed = pdata->getMsecLogoNrf(nrf);
				}
				bool overlap = true;
				bool unit = true;
				Nsc nsc_ins = pdata->insertLogo(msec_st, msec_ed,
								overlap, cmdset.arg.getOpt(JLOPT_DATA_FlagConfirm), unit);
				exe_command = (nsc_ins >= 0)? true : false;
			}
			break;
		case JLCMD_SEL_Select:
			if (nsc_scpos_sel >= 0){
				// �]���̊m��ʒu������
				Nsc nsc_scpos = pdata->sizeDataScp();
				Msec msec_nrf = pdata->getMsecLogoNrf(nrf);
				for(int j=1; j<nsc_scpos - 1; j++){
					Msec msec_j = pdata->getMsecScp(j);
					if (msec_j == msec_nrf){
						if (pdata->getScpStatpos(j) > SCP_PRIOR_NONE){
							pdata->setScpStatpos(j, SCP_PRIOR_NONE);
						}
					}
				}
				// �擪��؂�ʒu�̕ێ�
				if (nrf == 0){
					pdata->recHold.msecSelect1st = msec_nrf;
				}
				// �V�����m��ʒu��ݒ�
				Msec msec_tmp = pdata->getMsecScp(nsc_scpos_sel);
				pdata->setMsecLogoNrf(nrf, msec_tmp);
				pdata->setScpStatpos(nsc_scpos_sel, SCP_PRIOR_DECIDE);
				exe_command = true;
				if (cmdset.arg.getOpt(JLOPT_DATA_FlagConfirm) > 0){
					pdata->setResultLogoAtNrf(msec_tmp, LOGO_RESULT_DECIDE, nrf);
				}
			}
			break;
		case JLCMD_SEL_Force:
			{
				WideMsec wmsec = cmdset.limit.getTargetRangeWide();
				Msec msec_tmp = pdata->cnv.getMsecAlignFromMsec( wmsec.just );
				if (msec_tmp >= 0){
					exe_command = true;
					if (cmdset.arg.getOpt(JLOPT_DATA_FlagAutoChg) > 0){		// �����\���ɔ��f
						exeCmdCallAutoScript(cmdset);						// Auto�����N���X�Ăяo��
					}
					else{
						pdata->setResultLogoAtNrf(msec_tmp, LOGO_RESULT_DECIDE, nrf);
					}
				}
			}
			break;
		case JLCMD_SEL_Abort:
			exe_command = true;
			pdata->setResultLogoAtNrf(-1, LOGO_RESULT_ABORT, nrf);
			if (cmdset.arg.getOpt(JLOPT_DATA_FlagWithN) > 0){
				pdata->setResultLogoAtNrf(-1, LOGO_RESULT_ABORT, nrf+1);
			}
			if (cmdset.arg.getOpt(JLOPT_DATA_FlagWithP) > 0){
				pdata->setResultLogoAtNrf(-1, LOGO_RESULT_ABORT, nrf-1);
			}
			break;
		case JLCMD_SEL_GetPos:
			if (nsc_scpos_sel >= 0){
				Msec msec_tmp = pdata->getMsecScp(nsc_scpos_sel);
				setSystemRegPoshold(msec_tmp);					// �ϐ��ɐݒ�
				exe_command = true;
			}
			else if (msec_force >= 0){
				// �����V�[���`�F���W�Ȃ�-force�I�v�V�����ŋ����쐬��
				Msec msec_tmp = msec_force;
				msec_tmp = pdata->cnv.getMsecAlignFromMsec(msec_tmp);
				Msec msec_st, msec_ed;
				if (jlsd::isLogoEdgeRiseFromNrf(nrf)){			// start edge
					msec_st = msec_tmp;
					msec_ed = pdata->cnv.getMsecAdjustFrmFromMsec(msec_tmp, -1);
				}
				else{
					msec_st = pdata->cnv.getMsecAdjustFrmFromMsec(msec_tmp, +1);
					msec_ed = msec_tmp;
				}
				if (cmdset.arg.getOpt(JLOPT_DATA_FlagForce) > 0){
					pdata->insertScpos(msec_st, msec_ed, -1, SCP_PRIOR_DECIDE);
				}
				setSystemRegPoshold(msec_st);					// �ϐ��ɐݒ�
				exe_command = true;
			}
			break;
		case JLCMD_SEL_GetList:
			if (nsc_scpos_sel >= 0){
				Msec msec_tmp = pdata->getMsecScp(nsc_scpos_sel);
				setSystemRegListhold(msec_tmp, false);			// �ϐ��ɐݒ�
				exe_command = true;
			}
			else if (msec_force >= 0){
				// �����V�[���`�F���W�Ȃ�-force�I�v�V�����ŋ����쐬��
				Msec msec_tmp = msec_force;
				msec_tmp = pdata->cnv.getMsecAlignFromMsec(msec_tmp);
				Msec msec_st, msec_ed;
				if (jlsd::isLogoEdgeRiseFromNrf(nrf)){			// start edge
					msec_st = msec_tmp;
					msec_ed = pdata->cnv.getMsecAdjustFrmFromMsec(msec_tmp, -1);
				}
				else{
					msec_st = pdata->cnv.getMsecAdjustFrmFromMsec(msec_tmp, +1);
					msec_ed = msec_tmp;
				}
				if (cmdset.arg.getOpt(JLOPT_DATA_FlagForce) > 0){
					pdata->insertScpos(msec_st, msec_ed, -1, SCP_PRIOR_DECIDE);
				}
				setSystemRegListhold(msec_st, false);			// �ϐ��ɐݒ�
				exe_command = true;
			}
		default:
			break;
	}
	return exe_command;
}

//---------------------------------------------------------------------
// ����HEADTIME/TAILTIME���擾
//---------------------------------------------------------------------
bool JlsScript::exeCmdNextTail(JlsCmdSet &cmdset){
	//--- TAILFRAME������HEADFRAME�� ---
	string cstr;
	Msec msec_headframe = -1;
	if (getJlsRegVar(cstr, "TAILTIME", true) > 0){
		pdata->cnv.getStrValMsecM1(msec_headframe, cstr, 0);
	}
	//--- �͈͂��擾 ---
	WideMsec wmsec_target;
	wmsec_target.just  = msec_headframe + cmdset.arg.wmsecDst.just;
	wmsec_target.early = msec_headframe + cmdset.arg.wmsecDst.early;
	wmsec_target.late  = msec_headframe + cmdset.arg.wmsecDst.late;
	bool force = false;
	limitTargetRangeByImm(cmdset, wmsec_target, force);

	//--- ��ԋ߂��V�[���`�F���W�ʒu���擾 ---
	getTargetPoint(cmdset);

	//--- ���ʂ��i�[ --
	bool exeflag = false;
	Nsc nsc_scpos_sel = cmdset.limit.getResultTargetSel();
	if (nsc_scpos_sel >= 0){
		Msec msec_tailframe = pdata->getMsecScp(nsc_scpos_sel);
		setSystemRegHeadtail(msec_headframe, msec_tailframe);
		exeflag = true;
	}
	return exeflag;
}



//---------------------------------------------------------------------
// HEADTIME/TAILTIME��`�ɂ��t���[���ʒu����
// �o�́F
//    cmdset.limit.setHeadTail()
//---------------------------------------------------------------------
void JlsScript::limitHeadTail(JlsCmdSet &cmdset){
	RangeMsec rmsec;
	rmsec.st = pdata->recHold.rmsecHeadTail.st;
	if (rmsec.st == -1){
		rmsec.st = 0;
	}
	rmsec.ed = pdata->recHold.rmsecHeadTail.ed;
	if (rmsec.ed == -1){
		rmsec.ed = pdata->getMsecTotalMax();
	}
	cmdset.limit.setHeadTail(rmsec);
}

//--- ���ڐ��l�ݒ� ---
void JlsScript::limitHeadTailImm(JlsCmdSet &cmdset, RangeMsec rmsec){
	cmdset.limit.setHeadTail(rmsec);
}

//---------------------------------------------------------------------
// -F�n�I�v�V�����ɂ��t���[���ʒu����
// �o�́F
//    cmdset.limit.setFrameRange()
//---------------------------------------------------------------------
void JlsScript::limitWindow(JlsCmdSet &cmdset){
	//--- �t���[�������l��ݒ� ---
	Msec msec_opt_left  = cmdset.arg.getOpt(JLOPT_DATA_MsecFrameLeft);
	Msec msec_opt_right = cmdset.arg.getOpt(JLOPT_DATA_MsecFrameRight);
	Msec msec_limit_left  = msec_opt_left;
	Msec msec_limit_right = msec_opt_right;
	//--- -FR�I�v�V�����̃t���[�����������A�t���[�������l���擾 ---
	int type_frame = cmdset.arg.getOpt(JLOPT_DATA_TypeFrame);
	if (type_frame == CMDARG_FR_FR){
		int nrf_1st_rise = pdata->getNrfNextLogo(-1, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		if (nrf_1st_rise >= 0){
			int msec_tmp = pdata->getMsecLogoNrf(nrf_1st_rise);
			if (msec_limit_left != -1){
				msec_limit_left += msec_tmp;
			}
			if (msec_limit_right != -1){
				msec_limit_right += msec_tmp;
			}
		}
	}
	//--- ���Ԓl�������̎擾 ---
	bool flag_midext = false;
	if (type_frame == CMDARG_FR_FHEADX){
		type_frame = CMDARG_FR_FHEAD;
		flag_midext = true;
	}
	else if (type_frame == CMDARG_FR_FTAILX){
		type_frame = CMDARG_FR_FTAIL;
		flag_midext = true;
	}
	else if (type_frame == CMDARG_FR_FMIDX){
		type_frame = CMDARG_FR_FMID;
		flag_midext = true;
	}
	// -Fhead,-Ftail,-Fmid�Ńt���[���w�莞�̃t���[���v�Z
	if (type_frame == CMDARG_FR_FHEAD ||
		type_frame == CMDARG_FR_FTAIL ||
		type_frame == CMDARG_FR_FMID){
		//--- head/tail�擾 ---
		RangeMsec wmsec_headtail = cmdset.limit.getHeadTail();
		Msec msec_head = wmsec_headtail.st;
		Msec msec_tail = wmsec_headtail.ed;
		//--- ���Ԓn�_�̎擾 ---
		// �ŏ��̃��S�J�n����Ō�̃��S�I���̒��Ԓn�_���擾
		Nrf nrf_1st_rise = pdata->getNrfNextLogo(-1, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		Nrf nrf_end_fall = pdata->getNrfPrevLogo(pdata->sizeDataLogo()*2, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
		//--- �J�n�n�_���� ---
		Msec msec_window_start = 0;
		Msec msec_window_midst = 0;
		if (nrf_1st_rise >= 0)  msec_window_midst = pdata->getMsecLogoNrf(nrf_1st_rise);
		if (msec_window_midst < msec_head)  msec_window_midst = msec_head;
		if (msec_window_start < msec_head)  msec_window_start = msec_head;
		//--- �I���n�_���� ---
		Msec msec_window_mided = pdata->getMsecTotalMax();
		Msec msec_window_end   = pdata->getMsecTotalMax();
		if (nrf_end_fall >= 0) msec_window_mided = pdata->getMsecLogoNrf(nrf_end_fall);
		if (msec_window_mided > msec_tail) msec_window_mided = msec_tail;
		if (msec_window_end > msec_tail) msec_window_end = msec_tail;
		//--- ���Ԓn�_���� ---
		Msec msec_window_md = (msec_window_midst +msec_window_mided) / 2;
		//--- �t���[�������͈͂�ݒ� ---
		if (type_frame == CMDARG_FR_FHEAD){
			msec_limit_left  = msec_window_start + msec_opt_left;
			msec_limit_right = msec_window_start + msec_opt_right;
			if (msec_limit_right > msec_window_md && !flag_midext){
				msec_limit_right = msec_window_md;
			}
		}
		else if (type_frame == CMDARG_FR_FTAIL){
			msec_limit_left  = msec_window_end - msec_opt_right;
			msec_limit_right = msec_window_end - msec_opt_left;
			if (msec_limit_left < msec_window_md && !flag_midext){
				msec_limit_left = msec_window_md;
			}
		}
		else if (type_frame == CMDARG_FR_FMID){
			msec_limit_left  = msec_window_start + msec_opt_left;
			msec_limit_right = msec_window_end   - msec_opt_right;
			if (msec_limit_left > msec_window_md && !flag_midext){
				msec_limit_left = msec_window_md;
			}
			if (msec_limit_right < msec_window_md && !flag_midext){
				msec_limit_right = msec_window_md;
			}
		}
	}
	//--- ���ʊi�[ ---
	RangeMsec rmsecLimit = {msec_limit_left, msec_limit_right};
	cmdset.limit.setFrameRange(rmsecLimit);
}


//---------------------------------------------------------------------
// -TLhold�I�v�V�����őΏۈʒu�����肷��ꍇ�̈ʒu���X�g�쐬
// �o�́F
//    cmdset.limit.addTargetList
//---------------------------------------------------------------------
void JlsScript::limitListForTarget(JlsCmdSet &cmdset){
	//--- �I�v�V�����Ȃ���Ή������Ȃ� ---
	if (cmdset.arg.isSetOpt(JLOPT_DATA_MsecTLHoldL) == false) return;
	string cstr;
	if (getJlsRegVar(cstr, "LISTHOLD", true) > 0){
		cmdset.limit.clearTargetList();		// ���X�g������
		int pos = 0;
		string dstr;
		while ( (pos = pdata->cnv.getStrWord(dstr, cstr, pos)) > 0 ){
			int val;
			if (pdata->cnv.getStrValMsecM1(val, dstr, 0) > 0){
				if (val != -1){
					Msec arg_l = (Msec) cmdset.arg.getOpt(JLOPT_DATA_MsecTLHoldL);
					Msec arg_r = (Msec) cmdset.arg.getOpt(JLOPT_DATA_MsecTLHoldR);
					RangeMsec rmsec = {arg_l, arg_r};
					if (arg_l != -1) rmsec.st += val;
					if (arg_r != -1) rmsec.ed += val;
					if (arg_l != -1 || arg_r != -1){
						cmdset.limit.addTargetList(rmsec);
					}
				}
			}
			while (cstr[pos] == ',') pos++;
		}
	}
}


//---------------------------------------------------------------------
// -N -NR -LG�I�v�V�����ɑΉ�����L�����S���X�g���쐬
// �o�́F
//   �Ԃ�l�F ���X�g��
//    cmdset.limit.addLogoList()
//---------------------------------------------------------------------
int JlsScript::limitLogoList(JlsCmdSet &cmdset){
	Msec msec_spc = pdata->msecValSpc;
	//--- �R�}���h�ݒ��� ---
	LogoEdgeType edgesel = cmdset.arg.selectEdge;
	bool virtual_logo    = cmdset.arg.tack.virtualLogo;
	RangeMsec rmsec_headtail = cmdset.limit.getHeadTail();
	Msec msec_head = rmsec_headtail.st;
	Msec msec_tail = rmsec_headtail.ed;

	//--- �J�E���g�ő�l ---
	LogoMarkCount max_nrise = {};
	LogoMarkCount max_nfall = {};
	//--- �P��ڂ͑S�̐��m�F�p�łQ��ڂɃ}�C�i�X�ݒ���\�ɂ��� ---
	for(int k=0; k<2; k++){
		//--- �e��ԃJ�E���g ---
		LogoMarkCount nrise = {};
		LogoMarkCount nfall = {};
		//--- �ŏ��̈ʒu�ݒ� ---
		NrfCurrent logopt = {};
		ElgCurrent elg = {};
		bool flag_cont = true;
		while(flag_cont){
			Msec msec_rise, msec_fall;
			LogoMarkExist exist_rise = {};
			LogoMarkExist exist_fall = {};
			//--- ���ۂ̃��S�ʒu�����S�ԍ��Ƃ���ꍇ ---
			if (virtual_logo == false){
				flag_cont = pdata->getNrfptNext(logopt, LOGO_SELECT_ALL);
				if (flag_cont){
					msec_rise = logopt.msecRise;
					msec_fall = logopt.msecFall;
					if ( isLogoEdgeRise(edgesel) ){					// �����オ��G�b�W
						exist_rise.abs = true;						// -N
						exist_rise.rel = pdata->isValidLogoNrf(logopt.nrfRise);	// -NR
					}
					if ( isLogoEdgeFall(edgesel) ){					// �����オ��G�b�W
						exist_fall.abs = true;						// -N
						exist_fall.rel = pdata->isValidLogoNrf(logopt.nrfFall);	// -NR
					}
				}
			}
			//--- �����\���ω��_�����S�ԍ��Ƃ���ꍇ ---
			else{
				flag_cont = pdata->getElgptNext(elg);
				if (flag_cont){
					msec_rise = elg.msecRise;						// �ʒu
					msec_fall = elg.msecFall;						// �ʒu
					if ( isLogoEdgeRise(edgesel) ){					// �����オ��G�b�W
						exist_rise.abs = true;						// -N
						exist_rise.rel = true;						// -NR
					}
					if ( isLogoEdgeFall(edgesel) ){					// ����������G�b�W
						exist_fall.abs = true;						// -N
						exist_fall.rel = true;						// -NR
					}
				}
			}
			//--- �L���ʒu�J�E���g ---
			if (exist_rise.abs){										// �����オ��G�b�W
				nrise.abs ++;
				if (exist_rise.rel){
					nrise.rel ++;
					if (msec_head <= msec_rise + msec_spc &&
						msec_rise <= msec_tail + msec_spc){
						exist_rise.lg = true;						// -LG
						nrise.lg ++;
					}
				}
			}
			if (exist_fall.abs){										// ����������G�b�W
				nfall.abs ++;
				if (exist_fall.rel){
					nfall.rel ++;
					if (msec_head <= msec_fall + msec_spc &&
						msec_fall <= msec_tail + msec_spc){
						exist_fall.lg = true;						// -LG
						nfall.lg ++;
					}
				}
			}
			//--- 2��ڂɎ��s���S��ݒ� ---
			if (k == 1){
				bool result = limitLogoListSub(cmdset.arg, exist_rise, nrise, max_nrise);
				if (result){
					cmdset.limit.addLogoList(msec_rise, LOGO_EDGE_RISE);
				}
				result = limitLogoListSub(cmdset.arg, exist_fall, nfall, max_nfall);
				if (result){
					cmdset.limit.addLogoList(msec_fall, LOGO_EDGE_FALL);
				}
			}
		}
		//--- �P��ڂ̌����ōő�l�ݒ� ---
		if (k==0){
			max_nrise = nrise;
			max_nfall = nfall;
		}
	}
	return cmdset.limit.sizeLogoList();
}

// ���݃��S�ԍ����I�v�V�����w�胍�S�ԍ����`�F�b�N
bool JlsScript::limitLogoListSub(JlsCmdArg &cmdarg, LogoMarkExist exist, LogoMarkCount ncur, LogoMarkCount nmax){
	bool result = false;
	if (exist.abs){
		int numlist = (int) cmdarg.sizeLgOpt();
		if (numlist == 0){				// �w��Ȃ���Ώ������f�͑S���L������
			result = true;
		}
		for(int m=0; m<numlist; m++){
			int val = cmdarg.getLgOpt(m);
			switch( cmdarg.getOpt(JLOPT_DATA_TypeNLogo) ){
				case CMDARG_LG_N:						// -N�I�v�V����
					if ((val == ncur.abs) || (nmax.abs + val + 1 == ncur.abs)){
						result = true;
					}
					break;
				case CMDARG_LG_NR:						// -NR�I�v�V����
					if (exist.rel){
						if ((val == 0) ||
							(val == ncur.rel) || (nmax.rel + val + 1 == ncur.rel)){
						result = true;
						}
					}
					break;
				case CMDARG_LG_NLOGO:					// -Nlogo�I�v�V����
				case CMDARG_LG_NAUTO:					// -Nauto�I�v�V����
					if (exist.lg){
						if ((val == 0) ||
							(val == ncur.lg) || (nmax.lg + val + 1 == ncur.lg)){
						result = true;
						}
					}
					break;
				default:
					break;
			}
		}
	}
	return result;
}

//---------------------------------------------------------------------
// ����S��I��
// ���́F
//    nlist: �L�����S���X�g����I������ԍ�
// �o�́F
//   �Ԃ�l�F ���񖞂������S��񔻒�ifalse=���񖞂����Ȃ� true=���񖞂������S���擾�j
//    cmdset.limit.setLogoBaseNrf()
//    cmdset.limit.setLogoBaseNsc()
//---------------------------------------------------------------------
bool JlsScript::limitTargetLogo(JlsCmdSet &cmdset, int nlist){
	bool exeflag = limitTargetLogoGet(cmdset, nlist);
	if (exeflag){
		exeflag = limitTargetLogoCheck(cmdset);
	}
	return exeflag;
}

// ����S�ʒu���擾
bool JlsScript::limitTargetLogoGet(JlsCmdSet &cmdset, int nlist){
	//--- �R�}���h�ݒ���擾 ---
	Msec msec_target  = cmdset.limit.getLogoListMsec(nlist);
	LogoEdgeType edge = cmdset.limit.getLogoListEdge(nlist);
	//--- ���X�g���Ȃ���ΏI�� ---
	if (msec_target < 0) return false;
	//--- ���X�g�i�[�ʒu�ɑΉ����郍�S�ԍ����擾 ---
	bool det = false;
	if (cmdset.arg.tack.virtualLogo == false){
		Nrf nrf = -1;
		do{
			nrf = pdata->getNrfNextLogo(nrf, edge, LOGO_SELECT_ALL);
			if (nrf >= 0){
				Msec msec_cur = pdata->getMsecLogoNrf(nrf);
				if (msec_cur == msec_target){
					det = true;
					cmdset.limit.setLogoBaseNrf(nrf, edge);
				}
			}
		} while(nrf >= 0 && det == false);
	}
	else{
		Nsc nsc = -1;
		do{
			nsc = pdata->getNscNextElg(nsc, edge);
			if (nsc >= 0){
				Msec msec_cur = pdata->getMsecScp(nsc);
				if (msec_cur == msec_target){
					det = true;
					cmdset.limit.setLogoBaseNsc(nsc, edge);
				}
			}
		} while(nsc >= 0 && det == false);
	}
	return det;
}

// ����S�ʒu�ɑΉ���������ݒ���m�F
bool JlsScript::limitTargetLogoCheck(JlsCmdSet &cmdset){
	//--- �R�}���h�ݒ���擾 ---
	Nrf nrf_base = cmdset.limit.getLogoBaseNrf();
	Nsc nsc_base = cmdset.limit.getLogoBaseNsc();
	LogoEdgeType edge_base = cmdset.limit.getLogoBaseEdge();
	bool exeflag = false;
	//--- ���S�ʒu�𒼐ڐݒ肷��R�}���h�ɕK�v�ȃ`�F�b�N ---
	if (nrf_base >= 0){
		exeflag = true;
		//--- �m�茟�o�ς݃��S���m�F ---
		Msec msec_tmp;
		LogoResultType outtype_rf;
		pdata->getResultLogoAtNrf(msec_tmp, outtype_rf, nrf_base);
		//--- �m�胍�S�ʒu�����o����R�}���h�� ---
		bool igncomp = cmdset.arg.tack.ignoreComp;
		if (outtype_rf == LOGO_RESULT_NONE || (outtype_rf == LOGO_RESULT_DECIDE && igncomp)){
		}
		else{
			exeflag = false;
		}
		//--- select�p�m���⑶�ݎ��͏��� ---
		if (cmdset.arg.cmdsel == JLCMD_SEL_Select &&
			cmdset.arg.getOpt(JLOPT_DATA_FlagReset) == 0 &&
			pdata->getPriorLogo(nrf_base) >= LOGO_PRIOR_DECIDE){
			exeflag = false;
		}
	}
	else if (nsc_base >= 0){
		exeflag = true;
	}
	//--- �O��̃��S�ʒu���擾 ---
	WideMsec wmsec_lg;
	if (exeflag){
		getPrevNextLogo(wmsec_lg, nrf_base, nsc_base, edge_base);
		//--- �t���[���͈̓`�F�b�N ---
		{
			RangeMsec rmsec_frame = cmdset.limit.getFrameRange();
			//--- ���S���͈͓����m�F ---
			if ((rmsec_frame.st > wmsec_lg.just && rmsec_frame.st >= 0) ||
				(rmsec_frame.ed < wmsec_lg.just && rmsec_frame.ed >= 0)){
				exeflag = false;
			}
		}
		//--- �I�v�V�����Ɣ�r(-LenP, -LenN) ---
		if (exeflag){
			Msec msec_diflg_prev = wmsec_lg.just - wmsec_lg.early;
			Msec msec_diflg_next = wmsec_lg.late - wmsec_lg.just;
			if (wmsec_lg.early >= 0){
				int lenp_min = cmdset.arg.getOpt(JLOPT_DATA_MsecLenPMin);
				int lenp_max = cmdset.arg.getOpt(JLOPT_DATA_MsecLenPMax);
				if (msec_diflg_prev < lenp_min && lenp_min >= 0){
					exeflag = false;
				}
				if (msec_diflg_prev > lenp_max && lenp_max >= 0){
					exeflag = false;
				}
			}
			if (wmsec_lg.late >= 0){
				int lenn_min = cmdset.arg.getOpt(JLOPT_DATA_MsecLenNMin);
				int lenn_max = cmdset.arg.getOpt(JLOPT_DATA_MsecLenNMax);
				if (msec_diflg_next < lenn_min && lenn_min >= 0){
					exeflag = false;
				}
				if (msec_diflg_next > lenn_max && lenn_max >= 0){
					exeflag = false;
				}
			}
		}
	}
	//--- ���S�ʒu����-SC�n�I�v�V����������ꍇ�̊m�F ---
	if (cmdset.arg.tack.floatBase == false && exeflag){
		bool chk_base = true;
		bool chk_rel  = false;
		exeflag = checkOptScpFromMsec(cmdset.arg, wmsec_lg.just, edge_base, chk_base, chk_rel);
	}
	return exeflag;
}

//---------------------------------------------------------------------
// �����Ώ۔͈͂�ݒ�i����S�ʒu���x�[�X�j
// �o�́F
//   �Ԃ�l�F���񖞂����͈͊m�F�i0:�Y���Ȃ�  1:�Ώ۔͈͎擾�j
//   cmdset.limit.setTargetRange()
//---------------------------------------------------------------------
bool JlsScript::limitTargetRangeByLogo(JlsCmdSet &cmdset){
	//--- �R�}���h�ݒ���擾 ---
	Nrf nrf_base = cmdset.limit.getLogoBaseNrf();
	Nsc nsc_base = cmdset.limit.getLogoBaseNsc();
	LogoEdgeType edge_base = cmdset.limit.getLogoBaseEdge();
	bool exeflag = true;
	//--- ��Ƃ��郍�S�f�[�^�̈ʒu�͈͂�ǂݍ��� ---
	WideMsec wmsec_lg_org;
	{
		if (nrf_base >= 0){
			Nrf nrf_rev = nrf_base;
			if (cmdset.arg.getOpt(JLOPT_DATA_FlagFromLast) > 0){	// �P�O�̃��S����鎞�͋t�G�b�W
				nrf_rev = pdata->getNrfPrevLogo(nrf_base, jlsd::edgeInvert(edge_base), LOGO_SELECT_VALID);
			}
			wmsec_lg_org = {0, 0, 0};
			pdata->getWideMsecLogoNrf(wmsec_lg_org, nrf_rev);	// �Ώۃ��S�f�[�^�ʒu
		}
		else{
			Msec msec_tmp = pdata->getMsecScpEdge(nsc_base, edge_base);
			wmsec_lg_org = {msec_tmp, msec_tmp, msec_tmp};
		}
	}
	//--- �I�v�V�����ɂ��ʒu�I�� ---
	WideMsec wmsec_base;
	{
		if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromAbs) ){
			Msec msec_tmp = cmdset.arg.getOpt(JLOPT_DATA_MsecFromAbs);
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_tmp < 0) exeflag = false;				// ���̒l�̎��̓R�}���h�𖳌��Ƃ���
		}
		else if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromHead) ){
			Msec msec_opt = cmdset.arg.getOpt(JLOPT_DATA_MsecFromHead);
			Msec msec_tmp = cmdset.limit.getHead() + msec_opt;
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_opt < 0) exeflag = false;				// ���̒l�̎��̓R�}���h�𖳌��Ƃ���
		}
		else if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromTail) ){
			Msec msec_opt = cmdset.arg.getOpt(JLOPT_DATA_MsecFromTail);
			Msec msec_tmp = cmdset.limit.getTail() - msec_opt;
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_opt < 0) exeflag = false;				// ���̒l�̎��̓R�}���h�𖳌��Ƃ���
		}
		else{
			if (cmdset.arg.getOpt(JLOPT_DATA_FlagWide) > 0){
				wmsec_base = wmsec_lg_org;
			}
			else{
				Msec msec_tmp = wmsec_lg_org.just;
				wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			}
		}
	}
	//--- �R�}���h�w��͈̔͂��t���[���͈͂ɒǉ� ---
	WideMsec wmsec_find;
	{
		wmsec_find.just  = wmsec_base.just  + cmdset.arg.wmsecDst.just;	// set point to find
		wmsec_find.early = wmsec_base.early + cmdset.arg.wmsecDst.early;
		wmsec_find.late  = wmsec_base.late  + cmdset.arg.wmsecDst.late;
		//--- shift�R�}���h�ʒu���f ---
		if (cmdset.arg.isSetOpt(JLOPT_DATA_MsecSftC) != 0){
			wmsec_find.just  += cmdset.arg.getOpt(JLOPT_DATA_MsecSftC);
			wmsec_find.early += cmdset.arg.getOpt(JLOPT_DATA_MsecSftL);
			wmsec_find.late  += cmdset.arg.getOpt(JLOPT_DATA_MsecSftR);
		}
		//--- ���S�����Ɍ��肷��Select�R�}���h�p�͈̔� ---
		if (cmdset.arg.cmdsel == JLCMD_SEL_Select){
			Msec msec_ext_l = wmsec_lg_org.early + cmdset.arg.getOpt(JLOPT_DATA_MsecLogoExtL);
			Msec msec_ext_r = wmsec_lg_org.late  + cmdset.arg.getOpt(JLOPT_DATA_MsecLogoExtR);
			if (wmsec_find.early > msec_ext_r || wmsec_find.late < msec_ext_l){
				exeflag = false;
			}
			else{
				if (wmsec_find.early < msec_ext_l)  wmsec_find.early = msec_ext_l;
				if (wmsec_find.just  < msec_ext_l)  wmsec_find.just  = msec_ext_l;
				if (wmsec_find.late  > msec_ext_r)  wmsec_find.late  = msec_ext_r;
				if (wmsec_find.just  > msec_ext_r)  wmsec_find.just  = msec_ext_r;
			}
		}
		//--- �O��̃��S�ʒu�ȓ��ɔ͈͌��肷��ꍇ ---
		if (cmdset.arg.tack.limitByLogo){
			WideMsec wmsec_lgpn;
			getPrevNextLogo(wmsec_lgpn, nrf_base, nsc_base, edge_base);
			if (wmsec_lgpn.early >= 0){
				if (wmsec_find.early < wmsec_lgpn.early) wmsec_find.early = wmsec_lgpn.early;
				if (wmsec_find.just  < wmsec_lgpn.early) wmsec_find.just  = wmsec_lgpn.early;
			}
			if (wmsec_lgpn.late >= 0){
				if (wmsec_find.late > wmsec_lgpn.late) wmsec_find.late = wmsec_lgpn.late;
				if (wmsec_find.just > wmsec_lgpn.late) wmsec_find.just = wmsec_lgpn.late;
			}
		}
	}
	//--- �t���[���w��͈͓��Ɍ��� ---
	if (exeflag){
		RangeMsec rmsec_window = cmdset.limit.getFrameRange();
		exeflag = pdata->limitWideMsecFromRange(wmsec_find, rmsec_window);
	}
	//--- �͈͂����݂��Ȃ���Ζ����� ---
	if (wmsec_find.early > wmsec_find.late){
		exeflag = false;
	}
	//--- ���ʂ��i�[ ---
	Msec msec_force = -1;
	if ((cmdset.arg.getOpt(JLOPT_DATA_FlagForce) > 0) ||
		(cmdset.arg.getOpt(JLOPT_DATA_FlagNoForce) > 0)){
		msec_force = wmsec_find.just;
	}
	bool from_logo = true;
	cmdset.limit.setTargetRange(wmsec_find, msec_force, from_logo);

	return exeflag;
}

//--- ���ڐ��l�ݒ� ---
void JlsScript::limitTargetRangeByImm(JlsCmdSet &cmdset, WideMsec wmsec, bool force){
	Msec msec_force = (force)? wmsec.just : -1;
	bool from_logo = false;
	cmdset.limit.setTargetRange(wmsec, msec_force, from_logo);
}

//---------------------------------------------------------------------
// �^�[�Q�b�g�Ɉ�ԋ߂��V�[���`�F���W�ʒu���擾
// �o�́F
//   cmdset.list.setResultTarget() : �I���V�[���`�F���W�ʒu
//---------------------------------------------------------------------
void JlsScript::getTargetPoint(JlsCmdSet &cmdset){
	//--- �͈͂��擾 ---
	WideMsec wmsec_target = cmdset.limit.getTargetRangeWide();
	LogoEdgeType edge_sel = cmdset.limit.getLogoBaseEdge();

	Nsc nsc_scpos_tag = -1;
	Nsc nsc_scpos_end = -1;
	int flag_noedge = cmdset.arg.getOpt(JLOPT_DATA_FlagNoEdge);

	//--- -SC, -NoSC���I�v�V�����ɑΉ�����V�[���`�F���W�L������ ---
	getTargetPointSetScpEnable(cmdset);

	//--- NextTail�R�}���h�p ---
	bool flag_nexttail = false;
	bool flag_logorise = false;
	if (cmdset.arg.cmdsel == JLCMD_SEL_NextTail){
		flag_nexttail = true;
		flag_noedge = 0;
		edge_sel = LOGO_EDGE_RISE;
		if (cmdset.arg.selectEdge == LOGO_EDGE_RISE){
			flag_logorise = true;
		}
	}
	//--- ��ԋ߂��V�[���`�F���W�ʒu���擾 ---
	int size_scp = pdata->sizeDataScp();
	int jfrom = 0 + flag_noedge;
	int jsize = size_scp - flag_noedge;

	int val_difmin = 0;
	ScpPriorType stat_scpos = SCP_PRIOR_DUPE;
	//--- ���S����̏��擾�p(Next�R�}���h�p�j ---
	Nrf nrf_logo = 0;
	Msec msec_logo = 0;
	bool flag_logo = false;
	//--- ��ԋ߂��ӏ��̒T�� ---
	for(int j=jfrom; j<jsize; j++){
		Msec         msec_now = pdata->getMsecScp(j);
		ScpPriorType stat_now = pdata->getPriorScp(j);
		Msec         msec_now_edge = pdata->getMsecScpEdge(j, edge_sel);
		Msec         val_dif = abs(msec_now_edge - wmsec_target.just);
		//--- �Ώۉӏ��̃I�v�V��������m�F ---
		if ( (cmdset.limit.getScpEnable(j) && cmdset.limit.isTargetListed(msec_now)) || 
			 (j == size_scp-1 && flag_nexttail) ){
			//--- ���S����̏��g�p��(NextTail�R�}���h�p�j ---
			bool flag_now_logo = false;
			if (flag_logorise){
				while (msec_logo + pdata->msecValSpc < msec_now && nrf_logo >= 0){
					nrf_logo = pdata->getNrfNextLogo(nrf_logo, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
					if (nrf_logo >= 0){
						msec_logo = pdata->getMsecLogoNrf(nrf_logo);
					}
				}
				if (abs(msec_logo - msec_now) <= pdata->msecValSpc){
					if ( pdata->isAutoModeUse() == false ||
						 pdata->isScpChapTypeDecideFromNsc(j) ){	// �m���؂莞�̂ݗD��
						flag_now_logo = true;
					}
				}
				if (j == size_scp-1) flag_now_logo = true;			// �ŏI�ʒu�̓��S����
				if (flag_now_logo == false && flag_logo == true){	// ���ƌ��ʒu�̗D���Ԕ��f
					stat_now = SCP_PRIOR_DUPE;
				}
			}
			//--- �ŏ������̈ʒu��T�� ---
			if (val_difmin > val_dif || nsc_scpos_tag < 0){
				if (msec_now >= wmsec_target.early && msec_now <= wmsec_target.late){
					//--- ����Ԃ̊m�F ---
					bool chk_stat = false;
					if (stat_now >= stat_scpos || cmdset.arg.getOpt(JLOPT_DATA_FlagFlat) > 0){
						chk_stat = true;
					}
					else if (cmdset.arg.cmdsel == JLCMD_SEL_Select){
						chk_stat = true;
					}
					else if (j == size_scp-1){						// �ŏI�ʒu�͊m�舵��
						chk_stat = true;
					}
					if (chk_stat){
						//--- -EndLen�I�v�V�����m�F ---
						int nend = -2;
						if (cmdset.arg.getOpt(JLOPT_DATA_MsecEndlenC) != 0){
							nend = getTargetPointEndlen(cmdset.arg, msec_now);
						}
						else{
							nend = getTargetPointEndArg(cmdset.arg, msec_now);
						}
						if (nend != -1){			// End�ʒu���������Ė������̏ꍇ�̂ݏ���
							val_difmin = val_dif;
							nsc_scpos_tag = j;
							stat_scpos = stat_now;
							flag_logo = flag_now_logo;
							if (nend >= 0){
								nsc_scpos_end = nend;
							}
						}
					}
				}
			}
		}
	}
	//--- ���ʂ��i�[ ---
	cmdset.limit.setResultTarget(nsc_scpos_tag, nsc_scpos_end);
}


//---------------------------------------------------------------------
// -EndLen�I�v�V�����ɑΉ�����V�[���`�F���W�ʒu�擾
// ���́F
//   msec_base  : ��ƂȂ�t���[��
// �Ԃ�l�F
//   -1    : �Y���Ȃ�
//   0�ȏ� : ��v����V�[���`�F���W�ԍ�
//---------------------------------------------------------------------
Nsc JlsScript::getTargetPointEndlen(JlsCmdArg &cmdarg, int msec_base){
	Msec msec_endlen_c = msec_base + cmdarg.getOpt(JLOPT_DATA_MsecEndlenC);
	Msec msec_endlen_l = msec_base + cmdarg.getOpt(JLOPT_DATA_MsecEndlenL);
	Msec msec_endlen_r = msec_base + cmdarg.getOpt(JLOPT_DATA_MsecEndlenR);

	int size_scp = pdata->sizeDataScp();
	int jfrom = 0 + cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);
	int jsize = size_scp - cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);

	Nsc  nsc_scpos_end = -1;
	Msec val_difmin = 0;
	ScpPriorType stat_scpos = SCP_PRIOR_NONE;
	for(int j=jfrom; j<jsize; j++){
		int msec_now = pdata->getMsecScp(j);
		ScpPriorType stat_now = pdata->getPriorScp(j);
		int val_dif = abs(msec_now - msec_endlen_c);
		if (val_difmin > val_dif || nsc_scpos_end < 0){
			if (msec_now >= msec_endlen_l && msec_now <= msec_endlen_r){
				if (stat_now >= stat_scpos || cmdarg.getOpt(JLOPT_DATA_FlagFlat) > 0){
					val_difmin = val_dif;
					nsc_scpos_end = j;
					stat_scpos = stat_now;
				}
			}
		}
	}
	return nsc_scpos_end;
}

//---------------------------------------------------------------------
// End�n�I�v�V��������̈ʒu�擾�i-EndLen�ȊO�j
// ���́F
//   msec_base  : ��ƂȂ�t���[��
// �Ԃ�l�F
//   -2    : �R�}���h�Ȃ�
//   -1    : �w��ɑΉ�����ʒu�͊Y���Ȃ�
//   0�ȏ� : ��v����V�[���`�F���W�ԍ�
//---------------------------------------------------------------------
Nsc JlsScript::getTargetPointEndArg(JlsCmdArg &cmdarg, int msec_base){
	int  errnum = -2;
	Msec msec_target = 0;
	Msec msec_th = pdata->msecValExact;
	string cstr;

	//--- �I�v�V�������@�擾 ---
	if ( cmdarg.isSetOpt(JLOPT_DATA_MsecEndAbs) ){			// -EndAbs
		msec_target = cmdarg.getOpt(JLOPT_DATA_MsecEndAbs);
		if (msec_target >= 0){
			errnum = 0;
		}
		else{
			errnum = -1;
		}
	}
	else if ( cmdarg.isSetOpt(JLOPT_DATA_FlagEndHead) ){	// -EndHead
		Msec val = pdata->recHold.rmsecHeadTail.st;
		errnum = 0;
		if (val != -1){
			msec_target = val;
		}
		else{
			msec_target = 0;
		}
	}
	else if ( cmdarg.isSetOpt(JLOPT_DATA_FlagEndTail) ){	// -EndTail
		Msec val = pdata->recHold.rmsecHeadTail.ed;
		errnum = 0;
		if (val != -1){
			msec_target = val;
		}
		else{
			msec_target = pdata->getMsecTotalMax();
		}
	}
	else if ( cmdarg.isSetOpt(JLOPT_DATA_FlagEndHold) ){	// -EndHold
		int val = -1;
		if (getJlsRegVar(cstr, "POSHOLD", true) > 0){
			pdata->cnv.getStrValMsecM1(val, cstr, 0);
		}
		msec_target = val;
		if (val >= 0){
			errnum = 0;
		}
		else{
			errnum = -1;
		}
	}
	//--- �I�v�V�����ݒ� ---
	Nsc nsc_scpos_end;
	if (errnum == 0){
		nsc_scpos_end = pdata->getNscFromMsecFull(
							msec_target, msec_th, SCP_CHAP_NONE, SCP_END_EDGEIN);
	}
	else{
		nsc_scpos_end = errnum;
	}
	return nsc_scpos_end;
}

//---------------------------------------------------------------------
// -SC, -NoSC���I�v�V�����ɑΉ�����V�[���`�F���W�L������i�S�����V�[���`�F���W�ʒu�Ŋm�F�j
// �o�́F
//   cmdset.limit.setScpEnable()
//---------------------------------------------------------------------
void JlsScript::getTargetPointSetScpEnable(JlsCmdSet &cmdset){
	//--- �X�V���f ---
	int size_scp = pdata->sizeDataScp();
	int size_enable = cmdset.limit.sizeScpEnable();
	if (size_scp == size_enable) return;	// �����Ȃ���ΑO�񂩂�ύX�Ȃ�

	//--- ���΃R�}���h�͏�Ƀ`�F�b�N�B�ʏ�R�}���h�͐ݒ�ɂ��`�F�b�N ---
	bool chk_base = false;
	bool chk_rel  = true;
	if (cmdset.arg.tack.floatBase){
		chk_base = true;
	}
	//--- �S�����V�[���`�F���W�ʒu�Ń`�F�b�N ---
	vector <bool> list_enable;
	for(int m=0; m<size_scp; m++){
		int msec_base = pdata->getMsecScp(m);
		bool result = checkOptScpFromMsec(cmdset.arg, msec_base, LOGO_EDGE_RISE, chk_base, chk_rel);
		list_enable.push_back(result);
	}
	cmdset.limit.setScpEnable(list_enable);
}


//---------------------------------------------------------------------
// �O��̃��S�ʒu���擾
// ����
//   nrf     : �ʎZ���S�ԍ�
//   nsc     : �����\�����S�ԍ�
//   edge    : �����オ��^����������
// �o��
//   wmsec_lg
//      justr  : ���݃��S�ʒu�i�~���b�j
//      early  : �P�O�̗L�����S�ʒu�i�~���b�j
//      late   : �P��̗L�����S�ʒu�i�~���b�j
//---------------------------------------------------------------------
void JlsScript::getPrevNextLogo(WideMsec &wmsec_lg, int nrf, int nsc, LogoEdgeType edge){

	wmsec_lg = {-1, -1, -1};
	if (nrf >= 0){								// ���ۂ̃��S
		//--- �O�ネ�S�ʒu�擾 ---
		LogoEdgeType edge_side = jlsd::edgeInvert(edge);
		int nrf_prev = pdata->getNrfPrevLogo(nrf, edge_side, LOGO_SELECT_VALID);
		int nrf_next = pdata->getNrfNextLogo(nrf, edge_side, LOGO_SELECT_VALID);
		//--- �ʒu�i�~���b�j�擾 ---
		if (nrf >= 0){
			wmsec_lg.just  = pdata->getMsecLogoNrf(nrf);
		}
		if (nrf_prev >= 0){
			wmsec_lg.early = pdata->getMsecLogoNrf(nrf_prev);
		}
		if (nrf_next >= 0){
			wmsec_lg.late = pdata->getMsecLogoNrf(nrf_next);
		}
	}
	else{										// �������S�����\��
		//--- �O�ネ�S�ʒu�擾 ---
		LogoEdgeType edge_side = jlsd::edgeInvert(edge);
		int nsc_prev = pdata->getNscPrevElg(nsc, edge_side);
		int nsc_next = pdata->getNscNextElg(nsc, edge_side);
		//--- �Ɨ����S�œ����ʒu�ɂȂ�ꍇ�̕␳ ---
		if ( pdata->isSameLocPrevElg(nsc, edge_side) ){
			nsc_prev = nsc;
		}
		if ( pdata->isSameLocNextElg(nsc, edge_side) ){
			nsc_next = nsc;
		}
		//--- �ʒu�i�~���b�j�擾 ---
		if (nsc >= 0){
			wmsec_lg.just = pdata->getMsecScp(nsc);
		}
		if (nsc_prev >= 0){
			wmsec_lg.early = pdata->getMsecScp(nsc_prev);
		}
		if (nsc_next >= 0){
			wmsec_lg.late = pdata->getMsecScp(nsc_next);
		}
	}
}


//---------------------------------------------------------------------
// -SC, -NoSC�n�I�v�V�����ɑΉ�����V�[���`�F���W�L������
// ���́F
//   msec_base  : ��ƂȂ�t���[��
//   edge      : 0:start edge  1:end edge
//   chk_base  : �ʏ�R�}���h�̔�����{(false=���Ȃ� true=����)
//   chk_rel   : ���Έʒu�R�}���h�̔�����{(false=���Ȃ� true=����)
// �Ԃ�l�F
//   false : ��v����
//   true  : ��v�m�F
//---------------------------------------------------------------------
bool JlsScript::checkOptScpFromMsec(JlsCmdArg &cmdarg, int msec_base, LogoEdgeType edge, bool chk_base, bool chk_rel){
	int size_scp = pdata->sizeDataScp();
	int jfrom = 0 + cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);
	int jsize = size_scp - cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);
	bool result = true;
	int numlist = cmdarg.sizeScOpt();
	for(int k=0; k<numlist; k++){
		//--- ���Έʒu�R�}���h���菈�� ---
		JlOptionArgScType sctype = cmdarg.getScOptType(k);
		if (cmdarg.isScOptRelative(k)){			// ���Έʒu�����R�}���h
			if (chk_rel == false){				// ���Έʒu�`�F�b�N���Ȃ����͒��~
				sctype = CMDARG_SC_NONE;
			}
		}
		else{									// �ʏ�ݒ�
			if (chk_base == false){				// �ʏ�ݒ�̃`�F�b�N�łȂ����͒��~
				sctype = CMDARG_SC_NONE;
			}
		}
		//--- �Ώۂł���΃`�F�b�N ---
		if (sctype != CMDARG_SC_NONE){
			DataScpRecord dt;
			Nsc nsc_scpos_sc   = -1;
			Nsc nsc_smute_all  = -1;
			Nsc nsc_smute_part = -1;
			Nsc nsc_chap_auto  = -1;
			Msec lens_min = cmdarg.getScOptMin(k);
			Msec lens_max = cmdarg.getScOptMax(k);
			for(int j=jfrom; j<jsize; j++){
				pdata->getRecordScp(dt, j);
				int msec_now;
				if ( isLogoEdgeRise(edge) ){
					msec_now = dt.msec;
				}
				else{
					msec_now = dt.msbk;
				}
				if ((msec_now - msec_base >= lens_min || lens_min == -1) &&
					(msec_now - msec_base <= lens_max || lens_min == -1)){
					nsc_scpos_sc = j;
					// for -AC option
					ScpChapType chap_now = dt.chap;
					if (chap_now >= SCP_CHAP_DECIDE || chap_now == SCP_CHAP_CDET){
						nsc_chap_auto = j;
					}
				}
				// �����n
				int msec_smute_s = dt.msmute_s;
				int msec_smute_e = dt.msmute_e;
				if (msec_smute_s < 0 || msec_smute_e < 0){
					msec_smute_s = msec_now;
					msec_smute_e = msec_now;
				}
				// for -SMA option �i������񂪂���ꍇ�̂݌��o�j
				if ((msec_smute_s - msec_base <= lens_min) &&
					(msec_smute_e - msec_base >= lens_max)){
					nsc_smute_all = j;
				}
				//for -SM option
				if ((msec_smute_s - msec_base <= lens_max || lens_max == -1) &&
					(msec_smute_e - msec_base >= lens_min || lens_min == -1)){
					nsc_smute_part = j;
				}
			}
			if (nsc_scpos_sc < 0 && sctype == CMDARG_SC_SC){	// -SC
				result = false;
			}
			else if (nsc_scpos_sc >= 0 && sctype == CMDARG_SC_NOSC){	// -NoSC
				result = false;
			}
			else if (nsc_smute_part < 0 && sctype == CMDARG_SC_SM){	// -SM
				result = false;
			}
			else if (nsc_smute_part >= 0 && sctype == CMDARG_SC_NOSM){	// -NoSM
				result = false;
			}
			else if (nsc_smute_all < 0 && sctype == CMDARG_SC_SMA){	// -SMA
				result = false;
			}
			else if (nsc_smute_all >= 0 && sctype == CMDARG_SC_NOSMA){	// -NoSMA
				result = false;
			}
			else if (nsc_chap_auto < 0 && sctype == CMDARG_SC_AC){	// -AC
				result = false;
			}
			else if (nsc_chap_auto >= 0 && sctype == CMDARG_SC_NOAC){	// -NoAC
				result = false;
			}
		}
		if (result == false){
			break;
		}
	}
	return result;
}

//---------------------------------------------------------------------
// �����I�v�V�����̕��ёւ�
// ����-1�ȊO�̎��A�������l���ɂ���
//---------------------------------------------------------------------
void JlsScript::sortTwoValM1(int &val_a, int &val_b){
	if (val_a != -1 && val_b != -1){
		if (val_a > val_b){
			int tmp = val_a;
			val_a = val_b;
			val_b = tmp;
		}
	}
}

