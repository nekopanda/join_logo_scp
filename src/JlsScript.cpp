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
// 変数クラス
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// 変数を設定
// 入力：
//   strName   : 変数名
//   strVal    : 変数値
//   overwrite : 0=未定義時のみ設定  1=上書き許可設定
// 出力：
//   返り値    : 通常=true、失敗時=false
//---------------------------------------------------------------------
bool JlsRegFile::setRegVar(const string &strName, const string &strVal, bool overwrite){
	int n;
	int nloc   = -1;
	int nlenvar = (int) strName.size();
	int nMaxList = (int) m_strListVar.size();
	string strOrgName, strOrgVal;
	string strPair;

	//--- 既存変数の書き換えかチェック ---
	for(int i=0; i<nMaxList; i++){
		n = getRegNameVal(strOrgName, strOrgVal, m_strListVar[i]);
		if (nlenvar == n){
			if (_stricmp(strName.c_str(), strOrgName.c_str()) == 0){
				nloc = i;
			}
		}
	}
	//--- 設定文字列作成 ---
	strPair = strName + ":" + strVal;
	//--- 既存変数の書き換え ---
	if (nloc >= 0){
		if (overwrite){
			m_strListVar[nloc] = strPair;
		}
	}
	//--- 新規変数の追加 ---
	else{
		if (nMaxList < SIZE_VARNUM_MAX){		// 念のため変数最大数まで
			m_strListVar.push_back(strPair);
		}
		else{
			return false;
		}
	}
	return true;
}

//---------------------------------------------------------------------
// 変数を読み出し
// 入力：
//   strCandName : 読み出し変数名（候補）
//   excact      : 0=入力文字に最大マッチする変数  1=入力文字と完全一致する変数
// 出力：
//   返り値  : 変数名の文字数（0の時は対応変数なし）
//   strVal  : 変数値
//---------------------------------------------------------------------
int JlsRegFile::getRegVar(string &strVal, const string &strCandName, bool exact){
	int n;
	int nmatch = 0;
	int nloc   = -1;
	int nlencand = (int) strCandName.size();
	int nMaxList = (int) m_strListVar.size();
	string strOrgName, strOrgVal;

	//--- 名前とマッチする位置を検索 ---
	for(int i=0; i<nMaxList; i++){
		//--- 変数名と値を内部テーブルから取得 ---
		n = getRegNameVal(strOrgName, strOrgVal, m_strListVar[i]);
		//--- 内部テーブル変数名長が今までの最大一致より長ければ検索 ---
		if (nmatch < n){
			if (_strnicmp(strCandName.c_str(), strOrgName.c_str(), n) == 0 &&	// 先頭位置からマッチ
				(n == nlencand || exact == false)){								// 同一文字列かexact=false
				nloc   = i;
				nmatch = n;
			}
		}
	}
	//--- マッチした場合の値の読み出し ---
	if (nloc >= 0){
		n = getRegNameVal(strOrgName, strVal, m_strListVar[nloc]);			// 変数値を出力
	}
	return nmatch;
}

//---------------------------------------------------------------------
// 格納変数を名前と値に分解（変数読み書き関数からのサブルーチン）
//---------------------------------------------------------------------
int JlsRegFile::getRegNameVal(string &strName, string &strVal, const string &strPair){
	//--- 最初のデリミタ検索 ---
	int n = (int) strPair.find(":");
	//--- デリミタを分解して出力に設定 ---
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
// スクリプト制御
//
///////////////////////////////////////////////////////////////////////
JlsScriptState::JlsScriptState(){
	clear();
}

//---------------------------------------------------------------------
// 初期化
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
// If文設定
//   入力:   flag_cond=条件式
//   返り値: エラー番号（0=正常終了、1=エラー）
//---------------------------------------------------------------------
int JlsScriptState::ifBegin(bool flag_cond){
	CondIfState stat;
	if (m_ifSkip){
		m_ifSkip = true;
		stat = COND_IF_FINISHED;			// 条件：終了後
	}
	else if (flag_cond == false){
		m_ifSkip = true;
		stat = COND_IF_PREPARE;				// 条件：未実行
	}
	else{
		m_ifSkip = false;
		stat = COND_IF_RUNNING;				// 条件：実行
	}
	m_listIfState.push_back(stat);			// リストに保存
	return 0;
}

//---------------------------------------------------------------------
// EndIf文設定
//   返り値: エラー番号（0=正常終了、1=エラー）
//---------------------------------------------------------------------
int JlsScriptState::ifEnd(){
	int depth = (int) m_listIfState.size();
	if (depth <= 0){
		return 1;
	}
	m_listIfState.pop_back();				// リストから削除
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
// ElsIf文設定
//   入力:   flag_cond=条件式
//   返り値: エラー番号（0=正常終了、1=エラー）
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
// Repeat文設定
//   入力:   繰り返し回数
//   返り値: エラー番号（0=正常終了、1=エラー）
//---------------------------------------------------------------------
int JlsScriptState::repeatBegin(int num){
	//--- 最初のリピート処理 ---
	int depth = (int) m_listRepDepth.size();
	if (depth == 0){
		m_repSkip = false;					// 飛ばさない
		m_repLineReadCache = -1;			// キャッシュ読み出し無効
	}
	//--- キャッシュ行数チェック ---
	int size_line = (int) m_listRepCmdCache.size();
	if (size_line >= SIZE_REPLINE){			// 最大行数チェック
		return 1;
	}
	//--- キャッシュしてない場合は現在コマンドをキャッシュ ---
	if (size_line == 0){
		string str_tmp = "Repeat " + to_string(num);
		m_listRepCmdCache.push_back(str_tmp);
		size_line ++;
	}
	//--- リピート情報を設定 ---
	if (num <= 0 || m_repSkip){				// 最初から実行なしの場合
		num = -1;							// 実行なし時の回数は-1にする
		m_repSkip = true;					// コマンドを飛ばす
	}
	int line_start = m_repLineReadCache;
	if (line_start < 0) line_start = size_line;
	//--- 設定保存 ---
	RepDepthHold holdval;
	holdval.lineStart = line_start;			// 開始行を設定
	holdval.countLoop = num;				// 繰り返し回数を設定
	m_listRepDepth.push_back(holdval);		// リストに保存
	return 0;
}

//---------------------------------------------------------------------
// EndRepeat文設定
//   返り値: エラー番号（0=正常終了、1=エラー）
//---------------------------------------------------------------------
int JlsScriptState::repeatEnd(){
	int depth = (int) m_listRepDepth.size();
	if (depth <= 0){
		return 1;
	}
	if (m_listRepDepth[depth-1].countLoop > 0){		// カウントダウン
		m_listRepDepth[depth-1].countLoop --;
	}
	if (m_listRepDepth[depth-1].countLoop > 0){		// 繰り返し継続の場合
		m_repLineReadCache = m_listRepDepth[depth-1].lineStart;	// 読み出し行設定
	}
	else{											// 繰り返し終了の場合
		m_listRepDepth.pop_back();					// リストから削除
		depth --;
		//--- 全リピート終了時の処理 ---
		if (depth == 0){
			m_listRepCmdCache.clear();				// キャッシュ文字列の消去
			m_repSkip = false;
		}
		//--- 飛ばし状態を更新 ---
		else if (m_repSkip){
			if (m_listRepDepth[depth-1].countLoop >= 0){
				m_repSkip = false;
			}
		}
	}
	return 0;
}

//---------------------------------------------------------------------
// ネスト状態が残っているか確認
//   返り値: エラー番号（0=正常終了、bit0=If文ネスト中、bit1=Repeat文ネスト中）
//---------------------------------------------------------------------
int  JlsScriptState::isRemainNest(){
	int ret = 0;
	if ((int)m_listIfState.size() != 0) ret += 1;
	if ((int)m_listRepDepth.size() != 0) ret += 2;
	return ret;
}

//---------------------------------------------------------------------
// Cacheからの読み出し
//   返り値: 読み出し結果（false=読み出しなし、true=cacheからの読み出し）
//   strBufOrg: 読み出された文字列
//---------------------------------------------------------------------
bool JlsScriptState::readCmdCache(string &strBufOrg){
	//--- 読み出し可能かチェック ---
	if ( (int)m_listRepDepth.size() <= 0 ){		// Repeat中ではない
		return false;
	}
	if ( m_repLineReadCache >= (int)m_listRepCmdCache.size() ){
		m_repLineReadCache = -1;
	}
	if (m_repLineReadCache < 0) return false;

	//--- 読み出し実行 ---
	strBufOrg = m_listRepCmdCache[m_repLineReadCache];
	m_repLineReadCache ++;
	return true;
}

//---------------------------------------------------------------------
// Cacheに文字列格納
//   入力:   strBufOrg=格納文字列
//   返り値: 格納実行（false=格納不要、true=格納済み）
//---------------------------------------------------------------------
bool JlsScriptState::addCmdCache(string &strBufOrg){
	if ( (int)m_listRepDepth.size() <= 0 ){		// Repeat中ではない
		return false;
	}
	//--- キャッシュ行数チェック ---
	int size_line = (int) m_listRepCmdCache.size();
	if (size_line >= SIZE_REPLINE){			// 最大行数チェック
		return false;
	}
	m_listRepCmdCache.push_back(strBufOrg);
	return true;
}

//---------------------------------------------------------------------
// 現在行の制御状態からのコマンド実行有効性
//   入力：  現在行のコマンド分類（category_if=If系、category_repeat=Repeat系コマンド）
//   返り値: 有効性（false=無効行、true=有効行）
//---------------------------------------------------------------------
bool JlsScriptState::isValidCmdLine(bool category_if, bool category_repeat){
	if ((m_ifSkip && !category_if) || (m_repSkip && !category_repeat)){
		return false;
	}
	return true;
}

//---------------------------------------------------------------------
// 無効行の判定
//---------------------------------------------------------------------
bool JlsScriptState::isSkipCmd(){
	return m_ifSkip || m_repSkip;
}

///////////////////////////////////////////////////////////////////////
//
// JLスクリプト実行クラス
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// 初期化
//---------------------------------------------------------------------
JlsScript::JlsScript(JlsDataset *pdata){
	this->pdata  = pdata;

	// Auto系コマンドを拡張使用
	m_funcAutoScript.reset(new JlsAutoScript(pdata));

	// 念のため内部設定異常の確認
	checkInitial();
}

JlsScript::~JlsScript() = default;


//---------------------------------------------------------------------
// 変数を設定
// 入力：
//   strName   : 変数名
//   strVal    : 変数値
//   overwrite : 0=未定義時のみ設定  1=上書き許可設定
// 出力：
//   返り値    : 通常=true、失敗時=false
//---------------------------------------------------------------------
bool JlsScript::setJlsRegVar(const string &strName, const string &strVal, bool overwrite){
	//--- 最低限の違反文字確認 ---
	{
		string strCheckFull  = "!#$%&'()*+,-./:;<=>?";			// 変数文字列として使用禁止
		string strCheckFirst = strCheckFull + "0123456789";		// 変数先頭文字として使用禁止
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
	//--- 通常のレジスタ書き込み ---
	bool ret = m_regvar.setRegVar(strName, strVal, overwrite);

	//--- システム変数の特殊処理 ---
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
		//--- head/tail情報を更新 ---
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
// 変数を読み出し
// 入力：
//   strCandName : 読み出し変数名（候補）
//   excact      : 0=入力文字に最大マッチする変数  1=入力文字と完全一致する変数
// 出力：
//   返り値  : 変数名の文字数（0の時は対応変数なし）
//   strVal  : 変数値
//---------------------------------------------------------------------
int JlsScript::getJlsRegVar(string &strVal, const string &strCandName, bool exact){
	//--- 通常のレジスタ読み出し ---
	return m_regvar.getRegVar(strVal, strCandName, exact);
}

//---------------------------------------------------------------------
// スクリプト内で記載する起動オプション
// 入力：
//   argrest    ：引数残り数
//   strv       ：引数コマンド
//   str1       ：引数値１
//   str2       ：引数値２
//   overwrite  ：書き込み済みのオプション設定（false=しない true=する）
//   checklevel ：エラー確認レベル（0=なし 1=認識したオプションチェック）
// 出力：
//   返り値  ：引数取得数(-1の時取得エラー、0の時該当コマンドなし)
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
// CutMrgIn / CutMrgOut オプション処理用 30fpsフレーム数入力でミリ秒を返す
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
	//--- 30fps固定変換処理 ---
	Msec msec_num  = (abs(num) * 1001 + 30/2) / 30;
	Msec msec_frac = (frac * 1001 + 30/2) / 30 / 100;
	Msec msec_result = msec_num + msec_frac;
	if (num < 0) msec_result = -1 * msec_result;
	return msec_result;
}

//---------------------------------------------------------------------
// 変数を外部から設定
// 出力：
//   返り値  ：true=正常終了  false=失敗
//---------------------------------------------------------------------
bool JlsScript::setInputReg(const char *name, const char *val, bool overwrite){
	return setJlsRegVar(name, val, overwrite);
}

//---------------------------------------------------------------------
// オプションフラグを設定
// 出力：
//   返り値  ：true=正常終了  false=失敗
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
			//--- 各フラグの値を設定 ---
			int nloc = (int) strFlag.find(":");
			if (nloc >= 0){
				strName = strFlag.substr(0, nloc);
				strVal  = strFlag.substr(nloc+1);
			}
			else{
				strName = strFlag;
				strVal  = "1";
			}
			//--- 変数格納 ---
			bool flagtmp = setJlsRegVar(strName, strVal, overwrite);
			if (flagtmp == false) ret = false;
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// 内部設定の異常確認
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
// コマンド実行開始処理
//=====================================================================

//---------------------------------------------------------------------
// コマンド開始
// 出力：
//   返り値  ：0=正常終了 2=ファイル異常
//---------------------------------------------------------------------
int JlsScript::startCmd(const string &fname){
	//--- Call命令用のPath設定 ---
	pdata->cnv.getStrFilePath(m_pathNameJL, fname);

	//--- システム変数の初期値を設定 ---
	setSystemRegInit();

	//--- JLスクリプト実行 ---
	int errnum = startCmdLoop(fname, 0);

	//--- デバッグ用の表示 ---
	if (pdata->extOpt.verbose > 0 && errnum == 0){
		pdata->displayLogo();
		pdata->displayScp();
	}

	return errnum;
}


//---------------------------------------------------------------------
// コマンド読み込み・実行開始
// 入力：
//   fname   : スクリプトファイル名
//   loop    : 0=初回実行 1-:Callコマンドで呼ばれた場合のネスト数
// 出力：
//   返り値  ：0=正常終了 2=ファイル異常
//---------------------------------------------------------------------
int JlsScript::startCmdLoop(const string &fname, int loop){
	bool exe_command = false;			// 前回コマンドの実行状態

	//--- 初回実行 ---
	if (loop == 0){
		m_exe1st = true;
	}
	//--- 制御信号 ---
	JlsScriptState state;
	//--- ファイル読み込み ---
	ifstream ifs(fname.c_str());
	if (ifs.fail()){
		cerr << "error: failed to open " << fname << "\n";
		return 2;
	}
	string strBufOrg;
	while( startCmdGetLine(ifs, strBufOrg, state) ){
		//--- 前コマンドの実行有無を代入 ---
		setSystemRegLastexe(exe_command);
		//--- 変数を置換 ---
		string strBuf;
		replaceBufVar(strBuf, strBufOrg, state.isSkipCmd());
//printf("%s\n",strBuf.c_str());
		//--- デコード処理 ---
		JlsCmdSet cmdset;								// コマンド格納
		int errval = decodeCmd(cmdset.arg, strBuf);			// コマンド解析
		//--- デコード結果 ---
		JlcmdSelType      cmdsel   = cmdset.arg.cmdsel;
		JlcmdCategoryType category = cmdset.arg.category;
		bool enable_exe = true;
		//--- 実行マスク処理 ---
		{
			//--- 制御による有効行判断 ---
			bool category_if     = (category == JLCMD_CAT_COND)? true : false;
			bool category_repeat = (category == JLCMD_CAT_REP )? true : false;
			if (state.isValidCmdLine(category_if, category_repeat) == false){
				//--- 実行しない行はエラーも出さない ---
				enable_exe = false;
				errval = JLCMD_ERR_None;
			}
			//--- エラー時は実行しない ---
			if (errval != JLCMD_ERR_None){
				enable_exe = false;
			}
		}
		//--- コマンド処理 ---
		if (enable_exe){
			switch(category){
				case JLCMD_CAT_NONE:					// コマンドなし
					break;
				case JLCMD_CAT_COND:					// 条件分岐
					setCmdCondIf(cmdsel, cmdset.arg.cond.flagCond, state);
					break;
				case JLCMD_CAT_CALL:					// Call文
					errval = setCmdCall(cmdsel, strBuf, cmdset.arg.cond.posStr, loop);
					break;
				case JLCMD_CAT_REP:						// 繰り返し文
					errval = setCmdRepeat(cmdsel, strBuf, cmdset.arg.cond.posStr, state);
					break;
				case JLCMD_CAT_SYS:						// システムコマンド
					errval = setCmdSys(cmdsel, strBuf, cmdset.arg.cond.posStr);
					break;
				case JLCMD_CAT_REG:						// 変数設定
					errval = setCmdReg(cmdsel, strBuf, cmdset.arg.cond.posStr);
					break;
				default:								// 一般コマンド
					if (m_exe1st){						// 初回のみのチェック
						m_exe1st = false;
						if ( pdata->isSetupAdjInitial() ){
							pdata->setFlagSetupAdj( true );
							//--- 読み込みデータ微調整 ---
							JlsReformData func_reform(pdata);
							func_reform.adjustData();
							setSystemRegNologo(true);
						}
					}
					exe_command = exeCmd(cmdset);
					break;
			}
		}
		//--- エラーチェック ---
		if (errval != JLCMD_ERR_None){
			exe_command = false;
			startCmdDispErr(strBuf, errval);
		}
	}
	{						// ネストエラー確認
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
// 次の文字列取得
// 入出力：
//   ifs          : ファイル情報
//   state        : 制御状態
// 出力：
//   返り値    : 文字列取得結果（0=取得なし  1=取得あり）
//   strBufOrg : 取得文字列
//---------------------------------------------------------------------
bool JlsScript::startCmdGetLine(ifstream &ifs, string &strBufOrg, JlsScriptState &state){
	bool ret = false;

	//--- キャッシュ読み込み ---
	if ( state.readCmdCache(strBufOrg) ){
		ret = true;
	}
	//--- 通常の読み込み ---
	else{
		if ( getline(ifs, strBufOrg) ){
			ret = true;
			//--- キャッシュに保存 ---
			state.addCmdCache(strBufOrg);
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// エラー表示
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
// 変数部分を置換した文字列出力
// 入力：
//   strBuf : 文字列
// 出力：
//   返り値  ：置換結果（0=成功  -1=失敗）
//   dstBuf  : 出力文字列
//---------------------------------------------------------------------
int JlsScript::replaceBufVar(string &dstBuf, const string &srcBuf, int nowarn){
	string strName, strVal;
	int len_var, pos_var;

	dstBuf.clear();
	int ret = 0;
	int pos_cmt = (int) srcBuf.find("#");
	int pos_base = 0;
	while(pos_base >= 0){
		//--- 変数部分の置換 ---
		pos_var = (int) srcBuf.find("$", pos_base);
		if (pos_var >= 0){
			//--- $手前までの文字列を確定 ---
			if (pos_var > pos_base){
				dstBuf += srcBuf.substr(pos_base, pos_var-pos_base);
				pos_base = pos_var;
			}
			//--- 変数を検索して置換 ---
			len_var = replaceRegVarInBuf(strVal, srcBuf, pos_var);
			if (len_var > 0){
				dstBuf += strVal;
				pos_base += len_var;
			}
			else{
				if (pos_var < pos_cmt || pos_cmt < 0){		// コメントでなければ置換失敗
					ret = -1;
					if (nowarn == 0){
						cerr << "error: not defined variable in " << srcBuf << endl;
					}
				}
				pos_var = -1;
			}
		}
		//--- 変数がなければ残りすべてコピー ---
		if (pos_var < 0){
			dstBuf += srcBuf.substr(pos_base);
			pos_base = -1;
		}
	}
	return ret;
}

//---------------------------------------------------------------------
// 対象位置の変数を読み出し
// 入力：
//   strBuf : 文字列
//   pos    : 認識開始位置
// 出力：
//   返り値  ：変数部分の文字数
//   strVal  : 変数値
//---------------------------------------------------------------------
int JlsScript::replaceRegVarInBuf(string &strVal, const string &strBuf, int pos){
	int var_st, var_ed;
	bool exact;

	int ret = 0;
	if (strBuf[pos] == '$'){
		//--- 変数部分を取得 ---
		pos ++;
		if (strBuf[pos] == '{'){		// ${変数名}フォーマット時の処理
			exact = true;
			pos ++;
			var_st = pos;
			while(strBuf[pos] != '}' && strBuf[pos] != '\0'){
				pos ++;
			}
		}
		else{							// $変数名フォーマット時の処理
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
		//--- 変数読み出し実行 ---
		if (var_st <= var_ed){
			string strCandName = strBuf.substr(var_st, var_ed-var_st+1);
			int nmatch = getJlsRegVar(strVal, strCandName, exact);
			if (nmatch > 0){
				ret = nmatch + 1 + (exact*2);	// 変数名数 + $ + {}
			}
		}
	}
	return ret;
}



//=====================================================================
// デコード処理
//=====================================================================

//---------------------------------------------------------------------
// コマンド内容を文字列１行から解析
// 出力：
//   返り値：エラー状態
//   cmdarg: コマンド解析結果
//---------------------------------------------------------------------
int JlsScript::decodeCmd(JlsCmdArg &cmdarg, const string &strBuf){
	int retval = JLCMD_ERR_None;

	//--- コマンド内容初期化 ---
	cmdarg.clear();
	//--- コマンド受付(cmdsel) ---
	string strCmd;
	int csel = 0;
	int pos = pdata->cnv.getStrItem(strCmd, strBuf, 0);
	if (pos >= 0){
		csel = decodeCmdId(strCmd);
	}
	//--- コマンド異常時の終了 ---
	if (csel < 0){
		retval = JLCMD_ERR_ErrCmd;
		return retval;
	}

	//--- コマンド情報設定 ---
	JlcmdSelType      cmdsel   = CmdDefine[csel].cmdsel;
	JlcmdCategoryType category = CmdDefine[csel].category;
	int mustchar  = CmdDefine[csel].mustchar;
	int mustrange = CmdDefine[csel].mustrange;
	int needopt   = CmdDefine[csel].needopt;
	cmdarg.cmdsel = cmdsel;
	cmdarg.category = category;

	//--- 分類別動作 ---
	switch(category){
		case JLCMD_CAT_NONE:
			break;
		case JLCMD_CAT_COND:					// 条件分岐
			if (cmdsel == JLCMD_SEL_If   ||
				cmdsel == JLCMD_SEL_ElsIf){
				cmdarg.cond.flagCond = getCondFlag(strBuf, pos);
			}
			else{
				cmdarg.cond.flagCond = 1;
			}
			break;
		case JLCMD_CAT_CALL:					// Call文
		case JLCMD_CAT_REP:						// リピートコマンド
		case JLCMD_CAT_SYS:						// システムコマンド
		case JLCMD_CAT_REG:						// レジスタ設定
			cmdarg.cond.posStr = pos;
			break;
		default:
			break;
	}

	//--- コマンド解析 ---
	if (mustchar > 0 || mustrange > 0){
		pos = decodeCmdArgMust(cmdarg, retval, strBuf, pos, mustchar, mustrange);
	}

	//--- オプション受付 ---
	if (needopt > 0 && pos >= 0){
		while(pos >= 0){
			pos = decodeCmdArgOpt(cmdarg, retval, strBuf, pos);
		}
		decodeCmdTackOpt(cmdarg);
	}

	return retval;
}

//---------------------------------------------------------------------
// コマンド名を取得
// 出力：
//   返り値  ：取得コマンド番号（失敗時は-1）
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
		//--- 見つからなければ別名を検索 ---
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
// 必須引数の取得
// 入力：
//   strBuf : 文字列
//   pos    : 認識開始位置
//   tpc: 種類設定（0=設定なし  1=S/E/B  2=TR/SP/EC 3=省略可能なS/E/B）
//   tpw: 期間設定（0=設定なし  1=center  3=center+left+right）
// 出力：
//   返り値  : 読み込み位置（-1=オプション異常）
//   errval  : エラー番号
//   cmdarg  : コマンド解析結果
//---------------------------------------------------------------------
int JlsScript::decodeCmdArgMust(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos, int tpc, int tpw){
	if (tpc > 0 && pos >= 0){
		string strTmp;
		int posbak = pos;
		pos = pdata->cnv.getStrItem(strTmp, strBuf, pos);
		if (pos >= 0){
			//--- 項目１（文字指定） ---
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
	//--- 項目２（範囲指定） ---
	if (tpw > 0 && pos >= 0){
		if (tpw == 1 || tpw == 3){
			WideMsec wmsec = {};
			pos = pdata->cnv.getStrValMsec(wmsec.just, strBuf, pos);
			if (tpw == 3){
				pos = pdata->cnv.getStrValMsec(wmsec.early,  strBuf, pos);
				pos = pdata->cnv.getStrValMsec(wmsec.late, strBuf, pos);
				// 開始と終了が逆の場合は反転
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
// 引数オプションの取得
// バッファ残り部分から１設定を検索
// 出力：
//   返り値  : 読み込み位置（-1=オプション異常）
//   errval  : エラー番号
//   cmdarg  : コマンド解析結果
//---------------------------------------------------------------------
int JlsScript::decodeCmdArgOpt(JlsCmdArg &cmdarg, int &errval, const string &strBuf, int pos){
	string strWord;
	pos = pdata->cnv.getStrItem(strWord, strBuf, pos);
	if (pos >= 0){
		//--- コメント除去 ---
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
		//--- オプション識別 ---
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
		//--- オプションに対応した設定取得 ---
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
		//--- 読み込み ---
		int val1, val2, val3;
		switch(category){
			case JLOPT_CAT_NUMLOGO:					// ロゴ番号の限定
				if (cmdarg.isSetOpt(JLOPT_DATA_TypeNLogo) == false ||
					cmdarg.getOpt(JLOPT_DATA_TypeNLogo) == numdata){
					// 種類を設定
					cmdarg.setOpt(JLOPT_DATA_TypeNLogo, numdata);
					// 番号を設定
					string strSub;
					pos = pdata->cnv.getStrItem(strSub, strBuf, pos);
					int possub = 0;
					string strTmp;
					while(possub >= 0){		// comma区切りで複数値読み込み
						possub = pdata->cnv.getStrWord(strTmp, strSub, possub);
						if (possub >= 0){
							if (pdata->cnv.getStrValNum(val1, strTmp, 0) >= 0){
								cmdarg.addLgOpt(val1);
							}
						}
					}
				}
				break;
			case JLOPT_CAT_FRAME:					// フレーム位置による限定
				pos = pdata->cnv.getStrValMsecM1(val1, strBuf, pos);
				pos = pdata->cnv.getStrValMsecM1(val2, strBuf, pos);
				if (dsort == 12){
					sortTwoValM1(val1, val2);	// 並び替え
				}
				cmdarg.setOpt(dselect1, val1);
				cmdarg.setOpt(dselect2, val2);
				cmdarg.setOpt(JLOPT_DATA_TypeFrame, numdata);
				break;
			case JLOPT_CAT_SC:						// 無音SCによる限定
				pos = pdata->cnv.getStrValMsecM1(val1, strBuf, pos);
				pos = pdata->cnv.getStrValMsecM1(val2, strBuf, pos);
				if (dsort == 12){
					sortTwoValM1(val1, val2);	// 並び替え
				}
				cmdarg.addScOpt(numdata, val1, val2);
				break;
			case JLOPT_CAT_MSECM1:					// ミリ秒数（-1はそのまま）
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
							sortTwoValM1(val1, val2);	// 並び替え
							cmdarg.setOpt(dselect1, val1);
						}
						cmdarg.setOpt(dselect2, val2);
					}
				}
				if (numdata > 2){
					pos = pdata->cnv.getStrValMsecM1(val3,  strBuf, pos);
					if (pos >= 0){
						if (dsort == 23){
							sortTwoValM1(val2, val3);	// 並び替え
							cmdarg.setOpt(dselect2, val2);
						}
						cmdarg.setOpt(dselect3, val3);
					}
				}
				break;
			case JLOPT_CAT_NUM:						// 数値
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
			case JLOPT_CAT_SEC:						// 秒数
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
			case JLOPT_CAT_FLAG:					// フラグ
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
		//--- 引数不足時のエラー ---
		if (pos < 0){
			errval = JLCMD_ERR_ErrOpt;
		}
	}

	return pos;
}

//---------------------------------------------------------------------
// コマンドオプション内容から実行オプションの設定
// 出力：
//   cmdarg.tack  : コマンド解析結果
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
	//--- 比較位置を対象位置に変更 ---
	if (cmdarg.isSetOpt(JLOPT_DATA_MsecSftC) ||			// -shift
		cmdarg.getOpt(JLOPT_DATA_FlagRelative)){		// -relative
		floatbase = true;
	}
	//--- ロゴを推測位置に変更 ---
	if (category == JLCMD_CAT_AUTO ||
		category == JLCMD_CAT_AUTOEACH){				// Auto系
		vtlogo = true;
	}
	if (category == JLCMD_CAT_AUTOLOGO &&				// ロゴも見るAuto系
		cmdarg.getOpt(JLOPT_DATA_TypeNLogo) != CMDARG_LG_NLOGO){	// -Nlogo以外
		vtlogo = true;
	}
	//--- ロゴ確定状態でも実行するコマンド ---
	if (cmdsel == JLCMD_SEL_MkLogo  ||
		cmdsel == JLCMD_SEL_DivLogo ||
		cmdsel == JLCMD_SEL_GetPos){
		igncomp = true;
	}
	//--- 前後のロゴ位置以内に範囲限定する場合（絶対値位置指定とDivLogoコマンド） ---
	if (cmdsel == JLCMD_SEL_DivLogo){
		limbylogo = true;
	}
	if ( cmdarg.isSetOpt(JLOPT_DATA_MsecFromAbs)  ||	// -fromabs
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromHead) ||	// -fromhead
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromTail) ||	// -fromtail
		 cmdarg.getOpt(JLOPT_DATA_FlagAutoChg) ){		// -autochg
		limbylogo = true;
	}
	//--- 絶対位置指定時のロゴ検索は１箇所のみにする ---
	if ( cmdarg.isSetOpt(JLOPT_DATA_MsecFromAbs)  ||	// -fromabs
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromHead) ||	// -fromhead
		 cmdarg.isSetOpt(JLOPT_DATA_MsecFromTail) ){	// -fromtail
		onepoint = true;
	}
	if (cmdsel == JLCMD_SEL_GetPos){	// １箇所のみ検索
		onepoint = true;
	}
	//--- Auto構成を必要とするコマンド ---
	int numlist = cmdarg.sizeScOpt();
	if (numlist > 0){
		for(int i=0; i<numlist; i++){
			JlOptionArgScType sctype = cmdarg.getScOptType(i);
			if (sctype == CMDARG_SC_AC || sctype == CMDARG_SC_NOAC){
				needauto = true;
			}
		}
	}
	//--- 各ロゴ個別オプションのAutoコマンド ---
	if (cmdsel == JLCMD_SEL_AutoCut ||
		cmdsel == JLCMD_SEL_AutoAdd){
		if (cmdarg.getOpt(JLOPT_DATA_FlagAutoEach) > 0){
			category = JLCMD_CAT_AUTOEACH;
		}
	}
	//--- 設定格納 ---
	cmdarg.tack.floatBase   = floatbase;
	cmdarg.tack.virtualLogo = vtlogo;
	cmdarg.tack.ignoreComp  = igncomp;
	cmdarg.tack.limitByLogo = limbylogo;
	cmdarg.tack.onePoint    = onepoint;
	cmdarg.tack.needAuto    = needauto;
	cmdarg.category         = category;		// オプション(-autoeach)によるコマンド体系変更
}



//=====================================================================
// 変数展開を含めた文字列処理
//=====================================================================

//---------------------------------------------------------------------
// 文字列対象位置以降のフラグを判定
// 入力：
//   strBuf : 文字列
//   pos    : 認識開始位置
// 出力：
//   返り値  ：フラグ判定（0=false  1=true）
//---------------------------------------------------------------------
int JlsScript::getCondFlag(const string &strBuf, int pos){
	string strItem;
	string strCalc = "";
	string strBufRev = strBuf;
	//--- コメントカット ---
	int ntmp = (int) strBuf.find("#");
	if (ntmp >= 0){
		strBufRev = strBuf.substr(0, ntmp);
	}
	//--- １単語ずつ確認 ---
	while(pos >= 0){
		pos = pdata->cnv.getStrItem(strItem, strBufRev, pos);
		if (pos >= 0){
			getCondFlagConnectWord(strCalc, strItem);
		}
	}
	int pos_calc = 0;
	int val;
	if ((int)strCalc.find(":") >= 0 || (int)strCalc.find(".") >= 0){		// 時間表記だった場合
		pos_calc = pdata->cnv.getStrValMsec(val, strCalc, 0);	// 時間単位で比較
	}
	else{
		pos_calc = pdata->cnv.getStrValNum(val, strCalc, 0);	// strCalcの先頭から取得
	}
	if (pos_calc < 0){
		val = 0;
		cerr << "error: can not evaluate(" << strCalc << ") in " << strBuf << endl;
	}
	int flag = (val != 0)? 1 : 0;
	return flag;
}

//---------------------------------------------------------------------
// フラグ用に文字列を連結
// 入出力：
//   strCalc : 連結先文字列
// 入力：
//   strItem : 追加文字列
//---------------------------------------------------------------------
void JlsScript::getCondFlagConnectWord(string &strCalc, const string &strItem){

	//--- 連結文字の追加（比較演算子が２項間になければOR(||)を追加する） ---
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
	//--- 反転演算の判定 ---
	string strRemain;
	if (chNextFront == '!'){
		strCalc += "!";
		strRemain = strItem.substr(1);
	}
	else{
		strRemain = strItem;
	}
	//--- フラグ変数の判定 ---
	char chFront = strRemain.front();
	if ((chFront >= 'A' && chFront <= 'Z') || (chFront >= 'a' && chFront <= 'z')){
		string strVal;
		//--- 変数からフラグの値を取得 ---
		int nmatch = getJlsRegVar(strVal, strRemain, true);
		if (nmatch > 0 && strVal != "0"){	// 変数が存在して0以外の場合
			strVal = "1";
		}
		else{
			strVal = "0";
		}
		strCalc += strVal;				// フラグの値（0または1）を追加
	}
	else{
		strCalc += strRemain;			// 追加文字列をそのまま追加
	}
//printf("(rstr:%s)",strCalc.c_str());
}



//=====================================================================
// システム変数設定
//=====================================================================

//---------------------------------------------------------------------
// 初期設定変数
//---------------------------------------------------------------------
void JlsScript::setSystemRegInit(){
	setSystemRegHeadtail(-1, -1);
	setSystemRegUpdate();
}

//---------------------------------------------------------------------
// 初期設定変数の現在値による変更
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
// HEADFRAME/TAILFRAMEを設定
//---------------------------------------------------------------------
void JlsScript::setSystemRegHeadtail(int headframe, int tailframe){
	string str_head = pdata->cnv.getStringTimeMsecM1(headframe);
	string str_tail = pdata->cnv.getStringTimeMsecM1(tailframe);
	setJlsRegVar("HEADTIME", str_head, true);
	setJlsRegVar("TAILTIME", str_tail, true);
}

//---------------------------------------------------------------------
// システム変数の現在値による変更
//---------------------------------------------------------------------
void JlsScript::setSystemRegPoshold(int msec_pos){
	string str_val = pdata->cnv.getStringTimeMsecM1(msec_pos);
	setJlsRegVar("POSHOLD", str_val, true);
}

//---------------------------------------------------------------------
// システム変数の現在値による変更
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
// 無効なロゴの確認
//---------------------------------------------------------------------
void JlsScript::setSystemRegNologo(bool need_check){
	bool flag_nologo = false;
	//--- ロゴ期間が極端に少ない場合にロゴ無効化する場合の処理 ---
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
	else{		// チェックなしでロゴ無効の場合
			flag_nologo = true;
	}
	if (flag_nologo == true){
		// ロゴ読み込みなしに変更
		pdata->extOpt.flagNoLogo = 1;
		// システム変数を更新
		setJlsRegVar("NOLOGO", "1", true);	// 上書き許可で"1"設定
	}
}

//---------------------------------------------------------------------
// 前回の実行状態を設定
//---------------------------------------------------------------------
void JlsScript::setSystemRegLastexe(bool exe_command){
	//--- 前回の実行状態を変数に設定 ---
	setJlsRegVar("LASTEXE", to_string(exe_command), true);
	//--- 内部変数に設定 ---
	m_lastexe = exe_command;
}

//---------------------------------------------------------------------
// スクリプト内で記載する起動オプション
// 入力：
//   strBuf     ：オプションを含む文字列
//   pos        ：読み込み開始位置
//   overwrite  ：書き込み済みのオプション設定（false=しない true=する）
// 出力：
//   返り値  ：0=正常終了 -1=設定エラー
//---------------------------------------------------------------------
int JlsScript::setSystemRegOptions(const string &strBuf, int pos, bool overwrite){
	//--- 文字列区切り認識 ---
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
	//--- スクリプト内で設定可能なオプション ---
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
		else{		// 実行可能コマンドでなければ次に移行
			i ++;
		}
	}
	return 0;
}



//=====================================================================
// 設定コマンド処理
//=====================================================================

//---------------------------------------------------------------------
// If文処理
//---------------------------------------------------------------------
void JlsScript::setCmdCondIf(JlcmdSelType cmdsel, bool flag_cond, JlsScriptState &state){
	int errno;
	switch(cmdsel){
		case JLCMD_SEL_If:						// If文
			state.ifBegin(flag_cond);
			break;
		case JLCMD_SEL_EndIf:					// EndIf文
			errno = state.ifEnd();
			if (errno > 0){
				fprintf(stderr, "error: too many EndIf.\n");
			}
			break;
		case JLCMD_SEL_Else:					// Else文
		case JLCMD_SEL_ElsIf:					// ElsIf文
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
// Call処理
//---------------------------------------------------------------------
int JlsScript::setCmdCall(JlcmdSelType cmdsel, const string &strBuf, int pos, int loop){
	int errval = JLCMD_ERR_None;

	if (cmdsel == JLCMD_SEL_Call){
		string strTmp;
		pos = pdata->cnv.getStrItem(strTmp, strBuf, pos);
		if (pos >= 0){
			string strFileName = m_pathNameJL + strTmp;
			loop ++;
			if (loop < SIZE_CALL_LOOP){				// 再帰呼び出しは回数制限
				startCmdLoop(strFileName, loop);
			}
			else{
				// 無限呼び出しによるバッファオーバーフロー防止のため
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
// リピートコマンド処理
//---------------------------------------------------------------------
int JlsScript::setCmdRepeat(JlcmdSelType cmdsel, const string &strBuf, int pos, JlsScriptState &state){
	int errval = JLCMD_ERR_None;
	switch(cmdsel){
		case JLCMD_SEL_Repeat:				// Repeat文
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
		case JLCMD_SEL_EndRepeat:			// EndRepeat文
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
// システム関連コマンド処理
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
// レジスタ設定関連処理
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
		case JLCMD_SEL_UnitSec:					// 特定レジスタ設定
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
// SetParamコマンドによるパラメータ設定
//---------------------------------------------------------------------
int JlsScript::setCmdRegParam(const string &strName, const string &strVal){
	int errval = JLCMD_ERR_None;
	const char *varname = strName.c_str();

	//--- 文字列からパラメータを識別 ---
	int csel = -1;
	for(int i=0; i<SIZE_CONFIG_VAR; i++){
		if (_stricmp(varname, ConfigDefine[i].namestr) == 0){
			csel = i;
			break;
		}
	}
	//--- 対応パラメータ設定 ---
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
// コマンド実行処理
//=====================================================================

//---------------------------------------------------------------------
// スクリプト各行のコマンド実行
//---------------------------------------------------------------------
bool JlsScript::exeCmd(JlsCmdSet &cmdset){
	//--- コマンド実行の確認フラグ ---
	bool valid_exe = true;				// 今回の実行
	bool exe_command = false;			// 実行状態
	//--- 前コマンド実行済みか確認 (-else option) ---
	if (cmdset.arg.getOpt(JLOPT_DATA_FlagElse) > 0){
		if (m_lastexe){					// 直前コマンドを実行した場合
			valid_exe = false;			// 今回コマンドは実行しないが
			exe_command = true;			// 実行済み扱い
		}
	}
	//--- 前コマンド実行済みか確認 (-cont option) ---
	if (cmdset.arg.getOpt(JLOPT_DATA_FlagCont) > 0){
		if (m_lastexe == false){		// 直前コマンドを実行していない場合
			valid_exe = false;			// 今回コマンドも実行しない
		}
	}
	//--- コマンド実行 ---
	if (valid_exe){
		//--- 共通設定 ---
		limitHeadTail(cmdset);
		limitWindow(cmdset);						// -F系オプション設定
		limitListForTarget(cmdset);					// -TLholdによる範囲設定

		//--- オプションにAuto系構成が必要な場合(-AC -NoAC)の構成作成 ---
		if ( cmdset.arg.tack.needAuto ){
			exeCmdCallAutoSetup(cmdset);
		}
		//--- 分類別にコマンド実行 ---
		JlcmdCategoryType category = cmdset.arg.category;
		switch(category){
			case JLCMD_CAT_AUTO:
				exe_command = exeCmdCallAutoScript(cmdset);		// Auto処理クラス呼び出し
				break;
			case JLCMD_CAT_AUTOEACH:
				exe_command = exeCmdAutoEach(cmdset);			// 各ロゴ期間でAuto系処理
				break;
			case JLCMD_CAT_LOGO:
			case JLCMD_CAT_AUTOLOGO:
				exe_command = exeCmdLogo(cmdset);				// ロゴ別に実行
				break;
			case JLCMD_CAT_NEXT:
				exe_command = exeCmdNextTail(cmdset);			// 次の位置取得処理
				break;
			default:
				break;
		}
	}

	return exe_command;
}

//---------------------------------------------------------------------
// AutoScript拡張を実行
//---------------------------------------------------------------------
//--- コマンド解析後の実行 ---
bool JlsScript::exeCmdCallAutoScript(JlsCmdSet &cmdset){
	bool setup_only = false;
	return exeCmdCallAutoMain(cmdset, setup_only);
}
//--- コマンド解析のみ ---
bool JlsScript::exeCmdCallAutoSetup(JlsCmdSet &cmdset){
	bool setup_only = true;
	return exeCmdCallAutoMain(cmdset, setup_only);
}

//--- 実行メイン処理 ---
bool JlsScript::exeCmdCallAutoMain(JlsCmdSet &cmdset, bool setup_only){
	//--- 初回のみ実行 ---
	if ( pdata->isAutoModeInitial() ){
		//--- ロゴ使用レベルを設定 ---
		if (pdata->isExistLogo() == false){		// ロゴがない場合はロゴなしに設定
			pdata->setLevelUseLogo(CONFIG_LOGO_LEVEL_UNUSE_ALL);
		}
		else{
			int level = pdata->getConfig(CONFIG_VAR_LogoLevel);
			if (level <= CONFIG_LOGO_LEVEL_DEFAULT){		// 未設定時は値を設定
				level = CONFIG_LOGO_LEVEL_USE_HIGH;
			}
			pdata->setLevelUseLogo(level);
		}
		if (pdata->isUnuseLogo()){				// ロゴ使用しない場合
			pdata->extOpt.flagNoLogo = 1;		// ロゴなしに設定
			setSystemRegUpdate();				// NOLOGO更新
		}
	}
	//--- Autoコマンド実行 ---
	return m_funcAutoScript->startCmd(cmdset, setup_only);		// AutoScriptクラス呼び出し
}

//---------------------------------------------------------------------
// 各ロゴ期間を範囲として実行するAutoコマンド (-autoeachオプション)
//---------------------------------------------------------------------
bool JlsScript::exeCmdAutoEach(JlsCmdSet &cmdset){
	bool exeflag_total = false;
	NrfCurrent logopt = {};
	while( pdata->getNrfptNext(logopt, LOGO_SELECT_VALID) ){
		RangeMsec rmsec_logo;
		LogoResultType rtype_rise;
		LogoResultType rtype_fall;
		//--- 確定状態を確認 ---
		pdata->getResultLogoAtNrf(rmsec_logo.st, rtype_rise, logopt.nrfRise);
		pdata->getResultLogoAtNrf(rmsec_logo.ed, rtype_fall, logopt.nrfFall);
		//--- 確定時以外は候補場所にする ---
		if (rtype_rise != LOGO_RESULT_DECIDE){
			rmsec_logo.st = logopt.msecRise;
		}
		if (rtype_fall != LOGO_RESULT_DECIDE){
			rmsec_logo.ed = logopt.msecFall;
		}
		//--- 各ロゴ期間を範囲として位置を設定 ---
		limitHeadTailImm(cmdset, rmsec_logo);
		limitWindow(cmdset);						// 範囲を再度検索
		//--- Autoコマンド実行 ---
		int exeflag = exeCmdCallAutoScript(cmdset);	// Auto処理クラス呼び出し
		//--- 実行していたら実行フラグ設定 ---
		if (exeflag){
			exeflag_total = true;
		}
	};
	return exeflag_total;
}

//---------------------------------------------------------------------
// 全ロゴの中で選択ロゴを実行
//---------------------------------------------------------------------
bool JlsScript::exeCmdLogo(JlsCmdSet &cmdset){
	//--- ロゴ番号オプションから有効なロゴ番号位置をすべて取得 ---
	int nmax_list = limitLogoList(cmdset);
	//--- １箇所だけのコマンドか ---
	bool flag_onepoint = cmdset.arg.tack.onePoint;
	//--- １箇所だけコマンドの場合は一番近い候補だけを有効にする ---
	int nlist_base = -1;
	if (flag_onepoint){
		int difmsec_base = 0;
		for(int i=0; i<nmax_list; i++){
			//--- 制約条件を満たしているロゴか確認 ---
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
	//--- リスト作成時は開始前に内容消去 ---
	if (cmdset.arg.cmdsel == JLCMD_SEL_GetList){
		setSystemRegListhold(-1, true);				// 変数($LISTHOLD)クリア
	}
	//--- 各有効ロゴを実行 ---
	bool exeflag_total = false;
	for(int i=0; i<nmax_list; i++){
		if (flag_onepoint == false || nlist_base == i){
			//--- 制約条件を満たしているロゴか確認 ---
			bool exeflag = exeCmdLogoCheckTarget(cmdset, i);
			//--- 実行分岐 ---
			if (exeflag){
				switch(cmdset.arg.category){
					case JLCMD_CAT_LOGO :
						//--- ロゴ位置を直接設定するコマンド ---
						exeflag = exeCmdLogoTarget(cmdset);
						break;
					case JLCMD_CAT_AUTOLOGO :
						//--- 推測構成から生成するコマンド ---
						exeflag = exeCmdCallAutoScript(cmdset);
						break;
					default:
						break;
				}
			}
			//--- 実行していたら実行フラグ設定 ---
			if (exeflag){
				exeflag_total = true;
			}
		}
	}

	return exeflag_total;
}

//---------------------------------------------------------------------
// 対象ロゴについて制約条件を加味して対象位置取得
//---------------------------------------------------------------------
bool JlsScript::exeCmdLogoCheckTarget(JlsCmdSet &cmdset, int nlist){
	//--- 基準ロゴを選択 ---
	bool exeflag = limitTargetLogo(cmdset, nlist);
	//--- 検索対象範囲を設定（基準ロゴ位置をベース） ---
	if (exeflag){
		exeflag = limitTargetRangeByLogo(cmdset);
	}
	//--- ターゲットに一番近いシーンチェンジ位置を取得 ---
	if (exeflag){
		getTargetPoint(cmdset);
	}
	return exeflag;
}

//---------------------------------------------------------------------
// ロゴ位置別の実行コマンド
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
				if (cmdset.arg.getOpt(JLOPT_DATA_FlagAutoChg) > 0){	// 推測構成に反映
					exeCmdCallAutoScript(cmdset);					// Auto処理クラス呼び出し
				}
				else{								// 従来構成に反映
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
				//--- 対象位置取得 ---
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
				// 従来の確定位置を解除
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
				// 先頭区切り位置の保持
				if (nrf == 0){
					pdata->recHold.msecSelect1st = msec_nrf;
				}
				// 新しい確定位置を設定
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
					if (cmdset.arg.getOpt(JLOPT_DATA_FlagAutoChg) > 0){		// 推測構成に反映
						exeCmdCallAutoScript(cmdset);						// Auto処理クラス呼び出し
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
				setSystemRegPoshold(msec_tmp);					// 変数に設定
				exe_command = true;
			}
			else if (msec_force >= 0){
				// 無音シーンチェンジなく-forceオプションで強制作成時
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
				setSystemRegPoshold(msec_st);					// 変数に設定
				exe_command = true;
			}
			break;
		case JLCMD_SEL_GetList:
			if (nsc_scpos_sel >= 0){
				Msec msec_tmp = pdata->getMsecScp(nsc_scpos_sel);
				setSystemRegListhold(msec_tmp, false);			// 変数に設定
				exe_command = true;
			}
			else if (msec_force >= 0){
				// 無音シーンチェンジなく-forceオプションで強制作成時
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
				setSystemRegListhold(msec_st, false);			// 変数に設定
				exe_command = true;
			}
		default:
			break;
	}
	return exe_command;
}

//---------------------------------------------------------------------
// 次のHEADTIME/TAILTIMEを取得
//---------------------------------------------------------------------
bool JlsScript::exeCmdNextTail(JlsCmdSet &cmdset){
	//--- TAILFRAMEを次のHEADFRAMEに ---
	string cstr;
	Msec msec_headframe = -1;
	if (getJlsRegVar(cstr, "TAILTIME", true) > 0){
		pdata->cnv.getStrValMsecM1(msec_headframe, cstr, 0);
	}
	//--- 範囲を取得 ---
	WideMsec wmsec_target;
	wmsec_target.just  = msec_headframe + cmdset.arg.wmsecDst.just;
	wmsec_target.early = msec_headframe + cmdset.arg.wmsecDst.early;
	wmsec_target.late  = msec_headframe + cmdset.arg.wmsecDst.late;
	bool force = false;
	limitTargetRangeByImm(cmdset, wmsec_target, force);

	//--- 一番近いシーンチェンジ位置を取得 ---
	getTargetPoint(cmdset);

	//--- 結果を格納 --
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
// HEADTIME/TAILTIME定義によるフレーム位置限定
// 出力：
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

//--- 直接数値設定 ---
void JlsScript::limitHeadTailImm(JlsCmdSet &cmdset, RangeMsec rmsec){
	cmdset.limit.setHeadTail(rmsec);
}

//---------------------------------------------------------------------
// -F系オプションによるフレーム位置限定
// 出力：
//    cmdset.limit.setFrameRange()
//---------------------------------------------------------------------
void JlsScript::limitWindow(JlsCmdSet &cmdset){
	//--- フレーム制限値を設定 ---
	Msec msec_opt_left  = cmdset.arg.getOpt(JLOPT_DATA_MsecFrameLeft);
	Msec msec_opt_right = cmdset.arg.getOpt(JLOPT_DATA_MsecFrameRight);
	Msec msec_limit_left  = msec_opt_left;
	Msec msec_limit_right = msec_opt_right;
	//--- -FRオプションのフレームを検索し、フレーム制限値を取得 ---
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
	//--- 中間値制限情報の取得 ---
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
	// -Fhead,-Ftail,-Fmidでフレーム指定時のフレーム計算
	if (type_frame == CMDARG_FR_FHEAD ||
		type_frame == CMDARG_FR_FTAIL ||
		type_frame == CMDARG_FR_FMID){
		//--- head/tail取得 ---
		RangeMsec wmsec_headtail = cmdset.limit.getHeadTail();
		Msec msec_head = wmsec_headtail.st;
		Msec msec_tail = wmsec_headtail.ed;
		//--- 中間地点の取得 ---
		// 最初のロゴ開始から最後のロゴ終了の中間地点を取得
		Nrf nrf_1st_rise = pdata->getNrfNextLogo(-1, LOGO_EDGE_RISE, LOGO_SELECT_VALID);
		Nrf nrf_end_fall = pdata->getNrfPrevLogo(pdata->sizeDataLogo()*2, LOGO_EDGE_FALL, LOGO_SELECT_VALID);
		//--- 開始地点検索 ---
		Msec msec_window_start = 0;
		Msec msec_window_midst = 0;
		if (nrf_1st_rise >= 0)  msec_window_midst = pdata->getMsecLogoNrf(nrf_1st_rise);
		if (msec_window_midst < msec_head)  msec_window_midst = msec_head;
		if (msec_window_start < msec_head)  msec_window_start = msec_head;
		//--- 終了地点検索 ---
		Msec msec_window_mided = pdata->getMsecTotalMax();
		Msec msec_window_end   = pdata->getMsecTotalMax();
		if (nrf_end_fall >= 0) msec_window_mided = pdata->getMsecLogoNrf(nrf_end_fall);
		if (msec_window_mided > msec_tail) msec_window_mided = msec_tail;
		if (msec_window_end > msec_tail) msec_window_end = msec_tail;
		//--- 中間地点検索 ---
		Msec msec_window_md = (msec_window_midst +msec_window_mided) / 2;
		//--- フレーム制限範囲を設定 ---
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
	//--- 結果格納 ---
	RangeMsec rmsecLimit = {msec_limit_left, msec_limit_right};
	cmdset.limit.setFrameRange(rmsecLimit);
}


//---------------------------------------------------------------------
// -TLholdオプションで対象位置を限定する場合の位置リスト作成
// 出力：
//    cmdset.limit.addTargetList
//---------------------------------------------------------------------
void JlsScript::limitListForTarget(JlsCmdSet &cmdset){
	//--- オプションなければ何もしない ---
	if (cmdset.arg.isSetOpt(JLOPT_DATA_MsecTLHoldL) == false) return;
	string cstr;
	if (getJlsRegVar(cstr, "LISTHOLD", true) > 0){
		cmdset.limit.clearTargetList();		// リスト初期化
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
// -N -NR -LGオプションに対応する有効ロゴリストを作成
// 出力：
//   返り値： リスト数
//    cmdset.limit.addLogoList()
//---------------------------------------------------------------------
int JlsScript::limitLogoList(JlsCmdSet &cmdset){
	Msec msec_spc = pdata->msecValSpc;
	//--- コマンド設定情報 ---
	LogoEdgeType edgesel = cmdset.arg.selectEdge;
	bool virtual_logo    = cmdset.arg.tack.virtualLogo;
	RangeMsec rmsec_headtail = cmdset.limit.getHeadTail();
	Msec msec_head = rmsec_headtail.st;
	Msec msec_tail = rmsec_headtail.ed;

	//--- カウント最大値 ---
	LogoMarkCount max_nrise = {};
	LogoMarkCount max_nfall = {};
	//--- １回目は全体数確認用で２回目にマイナス設定も可能にする ---
	for(int k=0; k<2; k++){
		//--- 各状態カウント ---
		LogoMarkCount nrise = {};
		LogoMarkCount nfall = {};
		//--- 最初の位置設定 ---
		NrfCurrent logopt = {};
		ElgCurrent elg = {};
		bool flag_cont = true;
		while(flag_cont){
			Msec msec_rise, msec_fall;
			LogoMarkExist exist_rise = {};
			LogoMarkExist exist_fall = {};
			//--- 実際のロゴ位置をロゴ番号とする場合 ---
			if (virtual_logo == false){
				flag_cont = pdata->getNrfptNext(logopt, LOGO_SELECT_ALL);
				if (flag_cont){
					msec_rise = logopt.msecRise;
					msec_fall = logopt.msecFall;
					if ( isLogoEdgeRise(edgesel) ){					// 立ち上がりエッジ
						exist_rise.abs = true;						// -N
						exist_rise.rel = pdata->isValidLogoNrf(logopt.nrfRise);	// -NR
					}
					if ( isLogoEdgeFall(edgesel) ){					// 立ち上がりエッジ
						exist_fall.abs = true;						// -N
						exist_fall.rel = pdata->isValidLogoNrf(logopt.nrfFall);	// -NR
					}
				}
			}
			//--- 推測構成変化点をロゴ番号とする場合 ---
			else{
				flag_cont = pdata->getElgptNext(elg);
				if (flag_cont){
					msec_rise = elg.msecRise;						// 位置
					msec_fall = elg.msecFall;						// 位置
					if ( isLogoEdgeRise(edgesel) ){					// 立ち上がりエッジ
						exist_rise.abs = true;						// -N
						exist_rise.rel = true;						// -NR
					}
					if ( isLogoEdgeFall(edgesel) ){					// 立ち下がりエッジ
						exist_fall.abs = true;						// -N
						exist_fall.rel = true;						// -NR
					}
				}
			}
			//--- 有効位置カウント ---
			if (exist_rise.abs){										// 立ち上がりエッジ
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
			if (exist_fall.abs){										// 立ち下がりエッジ
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
			//--- 2回目に実行ロゴを設定 ---
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
		//--- １回目の検索で最大値設定 ---
		if (k==0){
			max_nrise = nrise;
			max_nfall = nfall;
		}
	}
	return cmdset.limit.sizeLogoList();
}

// 現在ロゴ番号がオプション指定ロゴ番号かチェック
bool JlsScript::limitLogoListSub(JlsCmdArg &cmdarg, LogoMarkExist exist, LogoMarkCount ncur, LogoMarkCount nmax){
	bool result = false;
	if (exist.abs){
		int numlist = (int) cmdarg.sizeLgOpt();
		if (numlist == 0){				// 指定なければ条件判断は全部有効扱い
			result = true;
		}
		for(int m=0; m<numlist; m++){
			int val = cmdarg.getLgOpt(m);
			switch( cmdarg.getOpt(JLOPT_DATA_TypeNLogo) ){
				case CMDARG_LG_N:						// -Nオプション
					if ((val == ncur.abs) || (nmax.abs + val + 1 == ncur.abs)){
						result = true;
					}
					break;
				case CMDARG_LG_NR:						// -NRオプション
					if (exist.rel){
						if ((val == 0) ||
							(val == ncur.rel) || (nmax.rel + val + 1 == ncur.rel)){
						result = true;
						}
					}
					break;
				case CMDARG_LG_NLOGO:					// -Nlogoオプション
				case CMDARG_LG_NAUTO:					// -Nautoオプション
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
// 基準ロゴを選択
// 入力：
//    nlist: 有効ロゴリストから選択する番号
// 出力：
//   返り値： 制約満たすロゴ情報判定（false=制約満たさない true=制約満たしロゴ情報取得）
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

// 基準ロゴ位置を取得
bool JlsScript::limitTargetLogoGet(JlsCmdSet &cmdset, int nlist){
	//--- コマンド設定情報取得 ---
	Msec msec_target  = cmdset.limit.getLogoListMsec(nlist);
	LogoEdgeType edge = cmdset.limit.getLogoListEdge(nlist);
	//--- リストがなければ終了 ---
	if (msec_target < 0) return false;
	//--- リスト格納位置に対応するロゴ番号を取得 ---
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

// 基準ロゴ位置に対応する条件設定を確認
bool JlsScript::limitTargetLogoCheck(JlsCmdSet &cmdset){
	//--- コマンド設定情報取得 ---
	Nrf nrf_base = cmdset.limit.getLogoBaseNrf();
	Nsc nsc_base = cmdset.limit.getLogoBaseNsc();
	LogoEdgeType edge_base = cmdset.limit.getLogoBaseEdge();
	bool exeflag = false;
	//--- ロゴ位置を直接設定するコマンドに必要なチェック ---
	if (nrf_base >= 0){
		exeflag = true;
		//--- 確定検出済みロゴか確認 ---
		Msec msec_tmp;
		LogoResultType outtype_rf;
		pdata->getResultLogoAtNrf(msec_tmp, outtype_rf, nrf_base);
		//--- 確定ロゴ位置も検出するコマンドか ---
		bool igncomp = cmdset.arg.tack.ignoreComp;
		if (outtype_rf == LOGO_RESULT_NONE || (outtype_rf == LOGO_RESULT_DECIDE && igncomp)){
		}
		else{
			exeflag = false;
		}
		//--- select用確定候補存在時は除く ---
		if (cmdset.arg.cmdsel == JLCMD_SEL_Select &&
			cmdset.arg.getOpt(JLOPT_DATA_FlagReset) == 0 &&
			pdata->getPriorLogo(nrf_base) >= LOGO_PRIOR_DECIDE){
			exeflag = false;
		}
	}
	else if (nsc_base >= 0){
		exeflag = true;
	}
	//--- 前後のロゴ位置を取得 ---
	WideMsec wmsec_lg;
	if (exeflag){
		getPrevNextLogo(wmsec_lg, nrf_base, nsc_base, edge_base);
		//--- フレーム範囲チェック ---
		{
			RangeMsec rmsec_frame = cmdset.limit.getFrameRange();
			//--- ロゴが範囲内か確認 ---
			if ((rmsec_frame.st > wmsec_lg.just && rmsec_frame.st >= 0) ||
				(rmsec_frame.ed < wmsec_lg.just && rmsec_frame.ed >= 0)){
				exeflag = false;
			}
		}
		//--- オプションと比較(-LenP, -LenN) ---
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
	//--- ロゴ位置から-SC系オプションを見る場合の確認 ---
	if (cmdset.arg.tack.floatBase == false && exeflag){
		bool chk_base = true;
		bool chk_rel  = false;
		exeflag = checkOptScpFromMsec(cmdset.arg, wmsec_lg.just, edge_base, chk_base, chk_rel);
	}
	return exeflag;
}

//---------------------------------------------------------------------
// 検索対象範囲を設定（基準ロゴ位置をベース）
// 出力：
//   返り値：制約満たす範囲確認（0:該当なし  1:対象範囲取得）
//   cmdset.limit.setTargetRange()
//---------------------------------------------------------------------
bool JlsScript::limitTargetRangeByLogo(JlsCmdSet &cmdset){
	//--- コマンド設定情報取得 ---
	Nrf nrf_base = cmdset.limit.getLogoBaseNrf();
	Nsc nsc_base = cmdset.limit.getLogoBaseNsc();
	LogoEdgeType edge_base = cmdset.limit.getLogoBaseEdge();
	bool exeflag = true;
	//--- 基準とするロゴデータの位置範囲を読み込み ---
	WideMsec wmsec_lg_org;
	{
		if (nrf_base >= 0){
			Nrf nrf_rev = nrf_base;
			if (cmdset.arg.getOpt(JLOPT_DATA_FlagFromLast) > 0){	// １つ前のロゴを取る時は逆エッジ
				nrf_rev = pdata->getNrfPrevLogo(nrf_base, jlsd::edgeInvert(edge_base), LOGO_SELECT_VALID);
			}
			wmsec_lg_org = {0, 0, 0};
			pdata->getWideMsecLogoNrf(wmsec_lg_org, nrf_rev);	// 対象ロゴデータ位置
		}
		else{
			Msec msec_tmp = pdata->getMsecScpEdge(nsc_base, edge_base);
			wmsec_lg_org = {msec_tmp, msec_tmp, msec_tmp};
		}
	}
	//--- オプションによる位置選択 ---
	WideMsec wmsec_base;
	{
		if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromAbs) ){
			Msec msec_tmp = cmdset.arg.getOpt(JLOPT_DATA_MsecFromAbs);
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_tmp < 0) exeflag = false;				// 負の値の時はコマンドを無効とする
		}
		else if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromHead) ){
			Msec msec_opt = cmdset.arg.getOpt(JLOPT_DATA_MsecFromHead);
			Msec msec_tmp = cmdset.limit.getHead() + msec_opt;
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_opt < 0) exeflag = false;				// 負の値の時はコマンドを無効とする
		}
		else if ( cmdset.arg.isSetOpt(JLOPT_DATA_MsecFromTail) ){
			Msec msec_opt = cmdset.arg.getOpt(JLOPT_DATA_MsecFromTail);
			Msec msec_tmp = cmdset.limit.getTail() - msec_opt;
			wmsec_base = {msec_tmp, msec_tmp, msec_tmp};
			if (msec_opt < 0) exeflag = false;				// 負の値の時はコマンドを無効とする
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
	//--- コマンド指定の範囲をフレーム範囲に追加 ---
	WideMsec wmsec_find;
	{
		wmsec_find.just  = wmsec_base.just  + cmdset.arg.wmsecDst.just;	// set point to find
		wmsec_find.early = wmsec_base.early + cmdset.arg.wmsecDst.early;
		wmsec_find.late  = wmsec_base.late  + cmdset.arg.wmsecDst.late;
		//--- shiftコマンド位置反映 ---
		if (cmdset.arg.isSetOpt(JLOPT_DATA_MsecSftC) != 0){
			wmsec_find.just  += cmdset.arg.getOpt(JLOPT_DATA_MsecSftC);
			wmsec_find.early += cmdset.arg.getOpt(JLOPT_DATA_MsecSftL);
			wmsec_find.late  += cmdset.arg.getOpt(JLOPT_DATA_MsecSftR);
		}
		//--- ロゴ候補内に限定するSelectコマンド用の範囲 ---
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
		//--- 前後のロゴ位置以内に範囲限定する場合 ---
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
	//--- フレーム指定範囲内に限定 ---
	if (exeflag){
		RangeMsec rmsec_window = cmdset.limit.getFrameRange();
		exeflag = pdata->limitWideMsecFromRange(wmsec_find, rmsec_window);
	}
	//--- 範囲が存在しなければ無効化 ---
	if (wmsec_find.early > wmsec_find.late){
		exeflag = false;
	}
	//--- 結果を格納 ---
	Msec msec_force = -1;
	if ((cmdset.arg.getOpt(JLOPT_DATA_FlagForce) > 0) ||
		(cmdset.arg.getOpt(JLOPT_DATA_FlagNoForce) > 0)){
		msec_force = wmsec_find.just;
	}
	bool from_logo = true;
	cmdset.limit.setTargetRange(wmsec_find, msec_force, from_logo);

	return exeflag;
}

//--- 直接数値設定 ---
void JlsScript::limitTargetRangeByImm(JlsCmdSet &cmdset, WideMsec wmsec, bool force){
	Msec msec_force = (force)? wmsec.just : -1;
	bool from_logo = false;
	cmdset.limit.setTargetRange(wmsec, msec_force, from_logo);
}

//---------------------------------------------------------------------
// ターゲットに一番近いシーンチェンジ位置を取得
// 出力：
//   cmdset.list.setResultTarget() : 選択シーンチェンジ位置
//---------------------------------------------------------------------
void JlsScript::getTargetPoint(JlsCmdSet &cmdset){
	//--- 範囲を取得 ---
	WideMsec wmsec_target = cmdset.limit.getTargetRangeWide();
	LogoEdgeType edge_sel = cmdset.limit.getLogoBaseEdge();

	Nsc nsc_scpos_tag = -1;
	Nsc nsc_scpos_end = -1;
	int flag_noedge = cmdset.arg.getOpt(JLOPT_DATA_FlagNoEdge);

	//--- -SC, -NoSC等オプションに対応するシーンチェンジ有無判定 ---
	getTargetPointSetScpEnable(cmdset);

	//--- NextTailコマンド用 ---
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
	//--- 一番近いシーンチェンジ位置を取得 ---
	int size_scp = pdata->sizeDataScp();
	int jfrom = 0 + flag_noedge;
	int jsize = size_scp - flag_noedge;

	int val_difmin = 0;
	ScpPriorType stat_scpos = SCP_PRIOR_DUPE;
	//--- ロゴからの情報取得用(Nextコマンド用） ---
	Nrf nrf_logo = 0;
	Msec msec_logo = 0;
	bool flag_logo = false;
	//--- 一番近い箇所の探索 ---
	for(int j=jfrom; j<jsize; j++){
		Msec         msec_now = pdata->getMsecScp(j);
		ScpPriorType stat_now = pdata->getPriorScp(j);
		Msec         msec_now_edge = pdata->getMsecScpEdge(j, edge_sel);
		Msec         val_dif = abs(msec_now_edge - wmsec_target.just);
		//--- 対象箇所のオプション制約確認 ---
		if ( (cmdset.limit.getScpEnable(j) && cmdset.limit.isTargetListed(msec_now)) || 
			 (j == size_scp-1 && flag_nexttail) ){
			//--- ロゴからの情報使用時(NextTailコマンド用） ---
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
						 pdata->isScpChapTypeDecideFromNsc(j) ){	// 確定区切り時のみ優先
						flag_now_logo = true;
					}
				}
				if (j == size_scp-1) flag_now_logo = true;			// 最終位置はロゴ扱い
				if (flag_now_logo == false && flag_logo == true){	// 候補と現位置の優先状態判断
					stat_now = SCP_PRIOR_DUPE;
				}
			}
			//--- 最小差分の位置を探索 ---
			if (val_difmin > val_dif || nsc_scpos_tag < 0){
				if (msec_now >= wmsec_target.early && msec_now <= wmsec_target.late){
					//--- 候補状態の確認 ---
					bool chk_stat = false;
					if (stat_now >= stat_scpos || cmdset.arg.getOpt(JLOPT_DATA_FlagFlat) > 0){
						chk_stat = true;
					}
					else if (cmdset.arg.cmdsel == JLCMD_SEL_Select){
						chk_stat = true;
					}
					else if (j == size_scp-1){						// 最終位置は確定扱い
						chk_stat = true;
					}
					if (chk_stat){
						//--- -EndLenオプション確認 ---
						int nend = -2;
						if (cmdset.arg.getOpt(JLOPT_DATA_MsecEndlenC) != 0){
							nend = getTargetPointEndlen(cmdset.arg, msec_now);
						}
						else{
							nend = getTargetPointEndArg(cmdset.arg, msec_now);
						}
						if (nend != -1){			// End位置を検索して未発見の場合のみ除く
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
	//--- 結果を格納 ---
	cmdset.limit.setResultTarget(nsc_scpos_tag, nsc_scpos_end);
}


//---------------------------------------------------------------------
// -EndLenオプションに対応するシーンチェンジ位置取得
// 入力：
//   msec_base  : 基準となるフレーム
// 返り値：
//   -1    : 該当なし
//   0以上 : 一致するシーンチェンジ番号
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
// End系オプションからの位置取得（-EndLen以外）
// 入力：
//   msec_base  : 基準となるフレーム
// 返り値：
//   -2    : コマンドなし
//   -1    : 指定に対応する位置は該当なし
//   0以上 : 一致するシーンチェンジ番号
//---------------------------------------------------------------------
Nsc JlsScript::getTargetPointEndArg(JlsCmdArg &cmdarg, int msec_base){
	int  errnum = -2;
	Msec msec_target = 0;
	Msec msec_th = pdata->msecValExact;
	string cstr;

	//--- オプション方法取得 ---
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
	//--- オプション設定 ---
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
// -SC, -NoSC等オプションに対応するシーンチェンジ有無判定（全無音シーンチェンジ位置で確認）
// 出力：
//   cmdset.limit.setScpEnable()
//---------------------------------------------------------------------
void JlsScript::getTargetPointSetScpEnable(JlsCmdSet &cmdset){
	//--- 更新判断 ---
	int size_scp = pdata->sizeDataScp();
	int size_enable = cmdset.limit.sizeScpEnable();
	if (size_scp == size_enable) return;	// 増減なければ前回から変更なし

	//--- 相対コマンドは常にチェック。通常コマンドは設定によりチェック ---
	bool chk_base = false;
	bool chk_rel  = true;
	if (cmdset.arg.tack.floatBase){
		chk_base = true;
	}
	//--- 全無音シーンチェンジ位置でチェック ---
	vector <bool> list_enable;
	for(int m=0; m<size_scp; m++){
		int msec_base = pdata->getMsecScp(m);
		bool result = checkOptScpFromMsec(cmdset.arg, msec_base, LOGO_EDGE_RISE, chk_base, chk_rel);
		list_enable.push_back(result);
	}
	cmdset.limit.setScpEnable(list_enable);
}


//---------------------------------------------------------------------
// 前後のロゴ位置を取得
// 入力
//   nrf     : 通算ロゴ番号
//   nsc     : 推測構成ロゴ番号
//   edge    : 立ち上がり／立ち下がり
// 出力
//   wmsec_lg
//      justr  : 現在ロゴ位置（ミリ秒）
//      early  : １つ前の有効ロゴ位置（ミリ秒）
//      late   : １つ後の有効ロゴ位置（ミリ秒）
//---------------------------------------------------------------------
void JlsScript::getPrevNextLogo(WideMsec &wmsec_lg, int nrf, int nsc, LogoEdgeType edge){

	wmsec_lg = {-1, -1, -1};
	if (nrf >= 0){								// 実際のロゴ
		//--- 前後ロゴ位置取得 ---
		LogoEdgeType edge_side = jlsd::edgeInvert(edge);
		int nrf_prev = pdata->getNrfPrevLogo(nrf, edge_side, LOGO_SELECT_VALID);
		int nrf_next = pdata->getNrfNextLogo(nrf, edge_side, LOGO_SELECT_VALID);
		//--- 位置（ミリ秒）取得 ---
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
	else{										// 推測ロゴ扱い構成
		//--- 前後ロゴ位置取得 ---
		LogoEdgeType edge_side = jlsd::edgeInvert(edge);
		int nsc_prev = pdata->getNscPrevElg(nsc, edge_side);
		int nsc_next = pdata->getNscNextElg(nsc, edge_side);
		//--- 独立ロゴで同じ位置になる場合の補正 ---
		if ( pdata->isSameLocPrevElg(nsc, edge_side) ){
			nsc_prev = nsc;
		}
		if ( pdata->isSameLocNextElg(nsc, edge_side) ){
			nsc_next = nsc;
		}
		//--- 位置（ミリ秒）取得 ---
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
// -SC, -NoSC系オプションに対応するシーンチェンジ有無判定
// 入力：
//   msec_base  : 基準となるフレーム
//   edge      : 0:start edge  1:end edge
//   chk_base  : 通常コマンドの判定実施(false=しない true=する)
//   chk_rel   : 相対位置コマンドの判定実施(false=しない true=する)
// 返り値：
//   false : 一致せず
//   true  : 一致確認
//---------------------------------------------------------------------
bool JlsScript::checkOptScpFromMsec(JlsCmdArg &cmdarg, int msec_base, LogoEdgeType edge, bool chk_base, bool chk_rel){
	int size_scp = pdata->sizeDataScp();
	int jfrom = 0 + cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);
	int jsize = size_scp - cmdarg.getOpt(JLOPT_DATA_FlagNoEdge);
	bool result = true;
	int numlist = cmdarg.sizeScOpt();
	for(int k=0; k<numlist; k++){
		//--- 相対位置コマンド判定処理 ---
		JlOptionArgScType sctype = cmdarg.getScOptType(k);
		if (cmdarg.isScOptRelative(k)){			// 相対位置明示コマンド
			if (chk_rel == false){				// 相対位置チェックしない時は中止
				sctype = CMDARG_SC_NONE;
			}
		}
		else{									// 通常設定
			if (chk_base == false){				// 通常設定のチェックでない時は中止
				sctype = CMDARG_SC_NONE;
			}
		}
		//--- 対象であればチェック ---
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
				// 無音系
				int msec_smute_s = dt.msmute_s;
				int msec_smute_e = dt.msmute_e;
				if (msec_smute_s < 0 || msec_smute_e < 0){
					msec_smute_s = msec_now;
					msec_smute_e = msec_now;
				}
				// for -SMA option （無音情報がある場合のみ検出）
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
// 引数オプションの並び替え
// 両方-1以外の時、小さい値を先にする
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

