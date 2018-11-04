//
// JLスクリプト用コマンド内容格納データ
//
#include "stdafx.h"
#include "CommonJls.hpp"
#include "JlsCmdSet.hpp"

///////////////////////////////////////////////////////////////////////
//
// JLスクリプトコマンド設定値
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// 初期設定
//---------------------------------------------------------------------
JlsCmdArg::JlsCmdArg(){
//	this->cmdset = this;
	clear();
}

//---------------------------------------------------------------------
// コマンド保持内容初期化
//---------------------------------------------------------------------
void JlsCmdArg::clear(){
	tack.floatBase   = 0;
	tack.virtualLogo = 0;
	tack.ignoreComp  = false;
	tack.limitByLogo = false;
	tack.onePoint    = false;
	tack.needAuto    = false;
	cond.posStr   = 0;
	cond.flagCond = false;

	cmdsel        = JLCMD_SEL_Nop;
	category      = JLCMD_CAT_NONE;
	wmsecDst      = {0, 0, 0};
	selectEdge    = LOGO_EDGE_RISE;
	selectAutoSub = JLCMD_SUB_TR;

	listLgVal.clear();
	listScOpt.clear();


	for(int i=0; i<SIZE_JLOPT_DATA; i++){
		optdata[i] = 0;
		flagset[i] = 0;
	}

	//--- 0以外の設定 ---
	optdata[JLOPT_DATA_MsecFrameLeft]  = -1;
	optdata[JLOPT_DATA_MsecFrameRight] = -1;
	optdata[JLOPT_DATA_MsecLenPMin]    = -1;
	optdata[JLOPT_DATA_MsecLenPMax]    = -1;
	optdata[JLOPT_DATA_MsecLenNMin]    = -1;
	optdata[JLOPT_DATA_MsecLenNMax]    = -1;
	optdata[JLOPT_DATA_MsecFromAbs]    = -1;
	optdata[JLOPT_DATA_MsecFromHead]   = -1;
	optdata[JLOPT_DATA_MsecFromTail]   = -1;
}

//---------------------------------------------------------------------
// オプションを設定
//---------------------------------------------------------------------
void JlsCmdArg::setOpt(int dselect, int val){
	if (dselect >= 0 && dselect < SIZE_JLOPT_DATA){
		optdata[dselect] = val;
		flagset[dselect] = 1;
	}
}

//---------------------------------------------------------------------
// 書き込み済みオプションかチェック
//---------------------------------------------------------------------
bool JlsCmdArg::isSetOpt(int dselect){
	if (dselect >= 0 && dselect < SIZE_JLOPT_DATA){
		if (flagset[dselect] != 0){
			return true;
		}
	}
	return false;
}

//---------------------------------------------------------------------
// オプションを取得
//---------------------------------------------------------------------
int JlsCmdArg::getOpt(int dselect){
	if (dselect >= 0 && dselect < SIZE_JLOPT_DATA){
		return optdata[dselect];
	}
	return 0;
}

//---------------------------------------------------------------------
// -SC系オプションの設定追加
//---------------------------------------------------------------------
void JlsCmdArg::addScOpt(int numdata, int min, int max){
	CmdArgSc scset;
	scset.type = numdata;
	scset.min  = min;
	scset.max  = max;
	listScOpt.push_back(scset);
}

//---------------------------------------------------------------------
// -SC系オプションを取得
//---------------------------------------------------------------------
//--- コマンド取得 ---
JlOptionArgScType JlsCmdArg::getScOptType(int num){
	if (num >= 0 && num < (int) listScOpt.size()){
		int typedata = listScOpt[num].type;
		if (typedata >= 16){
			typedata = typedata - 16;
		}
		return (JlOptionArgScType) typedata;
	}
	return CMDARG_SC_NONE;
}
//--- 相対位置コマンド(-RSC等）の判別 ---
bool JlsCmdArg::isScOptRelative(int num){
	if (num >= 0 && num < (int) listScOpt.size()){
		int typedata = listScOpt[num].type;
		if (typedata >= 16) return true;
	}
	return false;
}
//--- 設定範囲取得 ---
Msec JlsCmdArg::getScOptMin(int num){
	if (num >= 0 && num < (int) listScOpt.size()){
		return listScOpt[num].min;
	}
	return -1;
}
//--- 設定範囲取得 ---
Msec JlsCmdArg::getScOptMax(int num){
	if (num >= 0 && num < (int) listScOpt.size()){
		return listScOpt[num].max;
	}
	return -1;
}
//--- 格納数取得 ---
int JlsCmdArg::sizeScOpt(){
	return (int) listScOpt.size();
}

//---------------------------------------------------------------------
// -LG系オプションの設定追加
//---------------------------------------------------------------------
void JlsCmdArg::addLgOpt(int nlg){
	listLgVal.push_back(nlg);
}

//---------------------------------------------------------------------
// -LG系オプションを取得
//---------------------------------------------------------------------
//--- 値取得 ---
int JlsCmdArg::getLgOpt(int num){
	if (num >= 0 && num < (int) listLgVal.size()){
		return listLgVal[num];
	}
	return 0;
}
//--- 格納数取得 ---
int JlsCmdArg::sizeLgOpt(){
	return (int) listLgVal.size();
}

///////////////////////////////////////////////////////////////////////
//
// JLスクリプトコマンド設定反映用
//
///////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------
// 初期設定
//---------------------------------------------------------------------
JlsCmdLimit::JlsCmdLimit(){
	clear();
}

void JlsCmdLimit::clear(){
	process = 0;
	rmsecHeadTail = {-1, -1};
	rmsecFrameLimit = {-1, -1};
	listValidLogo.clear();
	nrfBase = -1;
	nscBase = -1;
	edgeBase = LOGO_EDGE_RISE;
	wmsecTarget = {-1, -1, -1};
	msecTargetFc = -1;
	fromLogo = false;
	listTLRange.clear();
	listScpEnable.clear();
	nscSel = -1;
	nscEnd = -1;
}

//---------------------------------------------------------------------
// 先頭と最後の位置
//---------------------------------------------------------------------
RangeMsec JlsCmdLimit::getHeadTail(){
	return rmsecHeadTail;
}

Msec JlsCmdLimit::getHead(){
	return rmsecHeadTail.st;
}

Msec JlsCmdLimit::getTail(){
	return rmsecHeadTail.ed;
}

bool JlsCmdLimit::setHeadTail(RangeMsec rmsec){
	process |= ARG_PROCESS_HEADTAIL;
	rmsecHeadTail = rmsec;
	return true;
}

//---------------------------------------------------------------------
// フレーム範囲(-Fオプション)
//---------------------------------------------------------------------
RangeMsec JlsCmdLimit::getFrameRange(){
	return rmsecFrameLimit;
}

bool JlsCmdLimit::setFrameRange(RangeMsec rmsec){
	if ((process & ARG_PROCESS_HEADTAIL) == 0){
		signalInternalError(ARG_PROCESS_FRAMELIMIT);
	}
	process |= ARG_PROCESS_FRAMELIMIT;
	rmsecFrameLimit = rmsec;
	return true;
}

//---------------------------------------------------------------------
// 有効なロゴ番号リスト
//---------------------------------------------------------------------
Msec JlsCmdLimit::getLogoListMsec(int nlist){
	if (nlist < 0 || nlist >= (int) listValidLogo.size()){
		return -1;
	}
	return listValidLogo[nlist].msec;
}

LogoEdgeType JlsCmdLimit::getLogoListEdge(int nlist){
	if (nlist < 0 || nlist >= (int) listValidLogo.size()){
		return LOGO_EDGE_RISE;
	}
	return listValidLogo[nlist].edge;
}

bool JlsCmdLimit::addLogoList(Msec &rmsec, jlsd::LogoEdgeType edge){
	if ((process & ARG_PROCESS_HEADTAIL) == 0){
		signalInternalError(ARG_PROCESS_VALIDLOGO);
	}
	process |= ARG_PROCESS_VALIDLOGO;
	ArgValidLogo argset = {rmsec, edge};
	listValidLogo.push_back(argset);
	return true;
}

int JlsCmdLimit::sizeLogoList(){
	return (int) listValidLogo.size();
}

//---------------------------------------------------------------------
// 対象とする基準ロゴ選択
//---------------------------------------------------------------------
Nrf JlsCmdLimit::getLogoBaseNrf(){
	return nrfBase;
}

Nsc JlsCmdLimit::getLogoBaseNsc(){
	return nscBase;
}

LogoEdgeType JlsCmdLimit::getLogoBaseEdge(){
	return edgeBase;
}

bool JlsCmdLimit::setLogoBaseNrf(Nrf nrf, jlsd::LogoEdgeType edge){
	if ((process & ARG_PROCESS_VALIDLOGO) == 0){
		signalInternalError(ARG_PROCESS_BASELOGO);
	}
	process |= ARG_PROCESS_BASELOGO;
	nrfBase = nrf;
	nscBase = -1;
	edgeBase = edge;
	return true;
}

bool JlsCmdLimit::setLogoBaseNsc(Nsc nsc, jlsd::LogoEdgeType edge){
	if ((process & ARG_PROCESS_VALIDLOGO) == 0){
		signalInternalError(ARG_PROCESS_BASELOGO);
	}
	process |= ARG_PROCESS_BASELOGO;
	nrfBase = -1;
	nscBase = nsc;
	edgeBase = edge;
	return true;
}

//---------------------------------------------------------------------
// ターゲット選択可能範囲
//---------------------------------------------------------------------
WideMsec JlsCmdLimit::getTargetRangeWide(){
	return wmsecTarget;
}

Msec JlsCmdLimit::getTargetRangeForce(){
	return msecTargetFc;
}

bool JlsCmdLimit::isTargetRangeLogo(){
	return fromLogo;
}

bool JlsCmdLimit::setTargetRange(WideMsec wmsec, Msec msec_force, bool from_logo){
	if ((process & ARG_PROCESS_BASELOGO) == 0 && from_logo){
		signalInternalError(ARG_PROCESS_TARGETRANGE);
	}
	process |= ARG_PROCESS_TARGETRANGE;
	wmsecTarget  = wmsec;
	msecTargetFc = msec_force;
	fromLogo     = from_logo;
	return true;
}

//---------------------------------------------------------------------
// ターゲット許可リスト
//---------------------------------------------------------------------
bool JlsCmdLimit::isTargetListed(Msec msec_target){
	int nsize = (int) listTLRange.size();
	//--- リストがなければ無条件で許可 ---
	if (nsize == 0) return true;
	//--- 各リスト検索 ---
	bool det = false;
	for(int i=0; i<nsize; i++){
		if (msec_target >= listTLRange[i].st && msec_target <= listTLRange[i].ed){
			det = true;
		}
	}
	return det;
}

void JlsCmdLimit::clearTargetList(){
	listTLRange.clear();
}

void JlsCmdLimit::addTargetList(RangeMsec rmsec){
	listTLRange.push_back(rmsec);
}

//---------------------------------------------------------------------
// 無音条件判定
//---------------------------------------------------------------------
bool JlsCmdLimit::getScpEnable(Nsc nsc){
	if (nsc < 0 || nsc >= (int) listScpEnable.size()){
		return false;
	}
	return listScpEnable[nsc];
}

bool JlsCmdLimit::setScpEnable(vector<bool> &listEnable){
	process |= ARG_PROCESS_SCPENABLE;
	listScpEnable = listEnable;
	return true;
}

int JlsCmdLimit::sizeScpEnable(){
	return (int) listScpEnable.size();
}

//---------------------------------------------------------------------
// ターゲットに一番近い位置
//---------------------------------------------------------------------
Nsc JlsCmdLimit::getResultTargetSel(){
	return nscSel;
}

Nsc JlsCmdLimit::getResultTargetEnd(){
	return nscEnd;
}

bool JlsCmdLimit::setResultTarget(Nsc nscSelIn, Nsc nscEndIn){
	if ((process & ARG_PROCESS_TARGETRANGE) == 0 ||
		(process & ARG_PROCESS_SCPENABLE  ) == 0){
		signalInternalError(ARG_PROCESS_RESULT);
	}
	process |= ARG_PROCESS_RESULT;
	nscSel = nscSelIn;
	nscEnd = nscEndIn;
	return true;
}

//---------------------------------------------------------------------
// エラー確認
//---------------------------------------------------------------------
void JlsCmdLimit::signalInternalError(CmdProcessFlag flags){
	cerr << "error:internal flow at ArgCmdLimit flag=" << flags << ",process=" << process << endl;
}

