//
// join_logo_scp データ用定義
//
#pragma once

//---------------------------------------------------------------------
// データ格納用
//---------------------------------------------------------------------
namespace jlsd
{
	using Msec = int;
	using Sec = int;
	using Nsc = int;
	using Nrf = int;
	using Nlg = int;

	//--- Auto構成区切り(scp.chap) ---
	enum ScpChapType {
		SCP_CHAP_DUPE = -1,		// 重複箇所
		SCP_CHAP_NONE,			// 初期状態
		SCP_CHAP_CPOSIT,		// 5秒単位可能性保持
		SCP_CHAP_CPOSQ,			// 15秒単位可能性保持
		SCP_CHAP_CDET,			// 15秒単位保持
		SCP_CHAP_DINT,			// 構成内部整数秒単位区切り
		SCP_CHAP_DBORDER,		// 無音シーンチェンジなし区切り
		SCP_CHAP_DFIX,			// 構成区切り
		SCP_CHAP_DFORCE,		// 強制区切り設定箇所
		SCP_CHAP_DUNIT			// Trimも強制区切り
	};
	static const ScpChapType SCP_CHAP_DECIDE = SCP_CHAP_DINT;		// 確定箇所閾値
	inline bool isScpChapTypeDecide(ScpChapType type){
		return (type >= SCP_CHAP_DECIDE)? true : false;
	}

	//--- Auto構成推測基本内容(scp.arstat) ---
	enum ScpArType {
		SCP_AR_UNKNOWN,			// 不明
		SCP_AR_L_UNIT,			// ロゴ有 １５秒単位
		SCP_AR_L_OTHER,			// ロゴ有 その他
		SCP_AR_L_MIXED,			// ロゴ有 ロゴ無も混合
		SCP_AR_N_UNIT,			// ロゴ無 １５秒単位
		SCP_AR_N_OTHER,			// ロゴ無 その他
		SCP_AR_N_AUNIT,			// ロゴ無 合併で１５秒の中間地点
		SCP_AR_N_BUNIT,			// ロゴ無 合併で１５秒の端
		SCP_AR_B_UNIT,			// ロゴ境界 １５秒単位
		SCP_AR_B_OTHER			// ロゴ境界 その他
	};
	static const int SCP_ARR_L_LOW  = SCP_AR_L_UNIT;		// ロゴ有の下限値
	static const int SCP_ARR_L_HIGH = SCP_AR_L_MIXED;		// ロゴ有の上限値
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
	//--- Auto構成推測拡張内容(scp.arext) ---
	enum ScpArExtType {
		SCP_AREXT_NONE,			// 追加構成なし
		SCP_AREXT_L_TRKEEP,		// ロゴ有 残す予告
		SCP_AREXT_L_TRCUT,		// ロゴ有 カット番宣
		SCP_AREXT_L_TRRAW,		// ロゴ有 エンドカード判断前
		SCP_AREXT_L_ECCUT,		// ロゴ有 カット番宣
		SCP_AREXT_L_EC,			// ロゴ有 エンドカード
		SCP_AREXT_L_SP,			// ロゴ有 番組提供
		SCP_AREXT_L_LGCUT,		// ロゴ有 ロゴ端部分カット
		SCP_AREXT_L_LGADD,		// ロゴ有 ロゴ端部分残す
		SCP_AREXT_N_TRCUT,		// ロゴ無 カット番宣
		SCP_AREXT_N_LGCUT,		// ロゴ無 ロゴ端部分カット
		SCP_AREXT_N_LGADD		// ロゴ無 ロゴ端部分残す
	};
	

	//--- 設定値保持 ---
	enum ConfigVarType {
		CONFIG_VAR_msecWLogoTRMax,			// AutoCutコマンドでカット対象とするロゴ期間最大フレーム期間
		CONFIG_VAR_msecWCompTRMax,			// AutoCutコマンドTRで予告と認識する構成最大フレーム期間
		CONFIG_VAR_msecWLogoSftMrg,			// Autoコマンド前調整でロゴ切り替わりのずれを許すフレーム期間
		CONFIG_VAR_msecWCompFirst,			// 先頭構成カット扱いにする構成最大フレーム期間
		CONFIG_VAR_msecWCompLast,			// 最後構成カット扱いにする構成最大フレーム期間
		CONFIG_VAR_msecWLogoSumMin,			// ロゴ合計期間が指定フレーム未満の時はロゴなしとして扱う
		CONFIG_VAR_msecWLogoLgMin,			// CM推測時にロゴ有情報確定と認識する最小ロゴ期間
		CONFIG_VAR_msecWLogoCmMin,			// CM推測時にロゴ無情報確定と認識する最小ロゴ期間
		CONFIG_VAR_msecWLogoRevMin,			// ロゴ情報補正する時に本編と認識する最小期間
		CONFIG_VAR_msecMgnCmDetect,			// CM構成で15秒単位ではない可能性と認識する誤差フレーム期間
		CONFIG_VAR_msecMgnCmDivide,			// CM構成内分割を許す１秒単位からの誤差フレーム期間
		CONFIG_VAR_secWCompSPMin,			// Autoコマンド番組提供で標準最小秒数
		CONFIG_VAR_secWCompSPMax,			// Autoコマンド番組提供で標準最大秒数
		CONFIG_VAR_flagCutTR,				// 15秒以上番宣をカットする場合は1をセット
		CONFIG_VAR_flagCutSP,				// 番組提供をカットする場合は1をセット
		CONFIG_VAR_flagAddLogo,				// ロゴあり通常構成を残す場合は1をセット（現在は未使用）
		CONFIG_VAR_flagAddUC,				// ロゴなし不明構成を残す場合は1をセット
		CONFIG_VAR_typeNoSc,				// シーンチェンジなし無音位置のCM判断（0:自動 1:なし 2:あり）
		CONFIG_VAR_cancelCntSc,				// 無音が多い構成を分離しない処理を1の時は使用しない
		CONFIG_VAR_LogoLevel,				// ロゴ使用レベル
		CONFIG_VAR_LogoRevise,				// ロゴからの補正
		CONFIG_VAR_AutoCmSub,				// ロゴなし時の補助設定
		CONFIG_VAR_msecPosFirst,			// ロゴ開始位置検出設定期間
		CONFIG_VAR_msecLgCutFirst,			// ロゴが最初からある時にカット扱いにする構成最大フレーム期間
		CONFIG_VAR_msecZoneFirst,			// ロゴ無効とする開始位置検出設定期間
		CONFIG_VAR_msecZoneLast,			// ロゴ無効とする終了位置検出設定期間
		CONFIG_VAR_priorityPosFirst,		// 最初の位置設定優先度（0:制御なし 1:ロゴあり 2:位置優先 3:Select優先）
		SIZE_CONFIG_VAR
	};
	enum ConfigActType {					// 設定値を動作別に取得用
		CONFIG_ACT_LogoDelEdge,				// ロゴ端のCM判断
		CONFIG_ACT_LogoDelMid,				// ロゴ内の15秒単位CM化
		CONFIG_ACT_LogoDelWide,				// 広域ロゴなし削除
		CONFIG_ACT_LogoUCRemain,			// ロゴなし不明部分を残す
		CONFIG_ACT_LogoUCGapCm,				// CM単位から誤差が大きい構成を残す
		CONFIG_ACT_MuteNoSc					// シーンチェンジなし無音位置のCM判断（1:使用しない 2:使用する）
	};

	//--- 構成候補優先順位(scp.stat) ---
	enum ScpPriorType {
		SCP_PRIOR_DUPE = -1,	// 間引き
		SCP_PRIOR_NONE,			// 初期状態
		SCP_PRIOR_LV1,			// 候補
		SCP_PRIOR_DECIDE		// 決定
	};

	//--- 構成候補優先順位(logo.stat_*) ---
	enum LogoPriorType {
		LOGO_PRIOR_DUPE = -1,	// 間引き
		LOGO_PRIOR_NONE,		// 初期状態
		LOGO_PRIOR_LV1,			// 候補
		LOGO_PRIOR_DECIDE		// 決定
	};
	//--- ロゴ分離構成状態(logo.unit_*) ---
	enum LogoUnitType {
		LOGO_UNIT_NORMAL,		// 通常
		LOGO_UNIT_DIVIDE		// ロゴ分離
	};
	//--- ロゴ結果確定状態(logo.flag_*) ---
	enum LogoResultType {
		LOGO_RESULT_NONE,		// 初期状態
		LOGO_RESULT_DECIDE,		// 確定
		LOGO_RESULT_ABORT		// abort破棄確定
	};

	//--- 選択方向 ---
	enum SearchDirType {
		SEARCH_DIR_PREV,
		SEARCH_DIR_NEXT
	};

	//--- ロゴの選択エッジ ---
	enum LogoEdgeType {
		LOGO_EDGE_RISE,			// ロゴの立ち上がりエッジ
		LOGO_EDGE_FALL,			// ロゴの立ち下がりエッジ
		LOGO_EDGE_BOTH			// ロゴの両エッジ
	};
	//--- ロゴの選択 ---
	enum LogoSelectType {
		LOGO_SELECT_ALL,		// 全選択
		LOGO_SELECT_VALID		// 有効のみ選択
	};
	//--- シーンチェンジで全体の先頭最後を除く選択 ---
	enum ScpEndType {
		SCP_END_EDGEIN,			// シーンチェンジ番号の先頭最後含む
		SCP_END_NOEDGE			// シーンチェンジ番号の先頭最後除く
	};

	//--- ロゴのエッジ方向認識 ---
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
	//--- ロゴ番号変換（nrf - nlg） ---
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
	//--- 優先順位の変換 ---
	inline LogoPriorType priorLogoFromScp(ScpPriorType n){
		return (LogoPriorType) n;
	}

	//--- 保持設定値のフラグ値定義 ---
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

	//--- 構造体 ---
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
		bool fixSt;				// true=確定開始地点
		bool fixEd;				// true=確定終了地点
	};
	struct RangeWideMsec {
		WideMsec st;
		WideMsec ed;
		bool fixSt;				// true=確定開始地点
		bool fixEd;				// true=確定終了地点
		bool logomode;			// false=CM期間  true=ロゴ期間
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
		bool valid;				// 0=データ格納なし  1=データ格納あり
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
		bool valid;				// 0=データ格納なし  1=データ格納あり
		bool border;			// 0=border含めない  1=border含む
		bool outflag;			// 0=内部動作 1=最終出力動作
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
// JLスクリプトの引数関連保持
//---------------------------------------------------------------------
namespace jlscmd
{
	//--- JLスクリプト命令 ---
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
	//--- JLスクリプト命令種類 ---
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
	//--- JLスクリプトオプション種類 ---
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
	//--- JLスクリプトオプション格納変数 ---
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

	//--- JLスクリプト命令サブ選択 ---
	enum JlcmdSubType {
		JLCMD_SUB_TR,
		JLCMD_SUB_SP,
		JLCMD_SUB_EC
	};
	//--- JLスクリプトデコード結果エラー ---
	enum JlcmdErrType {
		JLCMD_ERR_None,
		JLCMD_ERR_ErrOpt,				// コマンド異常（オプション）
		JLCMD_ERR_ErrRange,				// コマンド異常（範囲）
		JLCMD_ERR_ErrSEB,				// コマンド異常（S/E/B選択）
		JLCMD_ERR_ErrVar,				// コマンド異常（変数関連）
		JLCMD_ERR_ErrTR,				// コマンド異常（TR/SP/ED選択）
		JLCMD_ERR_ErrCmd				// コマンド異常（コマンド）
	};
	//--- JLスクリプトAuto系コマンド ---
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
	//--- Autoコマンドパラメータ ---
	enum JlParamAuto {
		// codeパラメータ
		PARAM_AUTO_c_exe,			// 0:コマンド実行なし 1:コマンド実行
		PARAM_AUTO_c_search,		// 検索する範囲を選択
		PARAM_AUTO_c_wmin,			// 構成期間の最小値秒数
		PARAM_AUTO_c_wmax,			// 構成期間の最大値秒数
		PARAM_AUTO_c_w15,			// 1:番組構成で15秒を検索
		PARAM_AUTO_c_lgprev,		// 0:ロゴ・予告の前側を対象外
		PARAM_AUTO_c_lgpost,		// 0:ロゴ・予告の後側を対象外
		PARAM_AUTO_c_lgintr,		// 1:予告と番組提供の間のみ対象とする
		PARAM_AUTO_c_lgsp,			// 1:番組提供が直後にある場合のみ対象
		PARAM_AUTO_c_cutskip,		// 1:予告カット以降も対象とする
		PARAM_AUTO_c_in1,			// 1:予告位置に番組提供を入れる
		PARAM_AUTO_c_chklast,		// 1:本体構成が後にあれば対象外とする
		PARAM_AUTO_c_lgy,			// 1:ロゴ内を対象とする
		PARAM_AUTO_c_lgn,			// 1:ロゴ外を対象とする
		PARAM_AUTO_c_lgbn,			// 1:両隣を含めロゴ外の場合を対象とする
		PARAM_AUTO_c_limloc,		// 1:標準期間の候補位置のみに限定
		PARAM_AUTO_c_limtrsum,		// 1:予告期間により無効化する
		PARAM_AUTO_c_unitcmoff,		// 1:CM分割した構成の検出を強制無効
		PARAM_AUTO_c_unitcmon,		// 1:CM分割した構成の検出を強制設定
		PARAM_AUTO_c_wdefmin,		// 標準の構成期間の最小値秒数
		PARAM_AUTO_c_wdefmax,		// 標準の構成期間の最大値秒数
		// autocut用
		PARAM_AUTO_c_from,			// cuttr
		PARAM_AUTO_c_cutst,			// cuttr
		PARAM_AUTO_c_lgpre,			// cuttr
		PARAM_AUTO_c_sel,			// cutec
		PARAM_AUTO_c_cutla,			// cutec
		PARAM_AUTO_c_cutlp,			// cutec
		PARAM_AUTO_c_cut30,			// cutec
		PARAM_AUTO_c_cutsp,			// cutec
		// edge用
		PARAM_AUTO_c_cmpart,
		PARAM_AUTO_c_add,
		PARAM_AUTO_c_allcom,
		PARAM_AUTO_c_noedge,
		// autoins,autodel用
		PARAM_AUTO_c_restruct,
		// 数値パラメータ
		PARAM_AUTO_v_limit,
		PARAM_AUTO_v_scope,
		PARAM_AUTO_v_scopen,
		PARAM_AUTO_v_period,
		PARAM_AUTO_v_maxprd,
		PARAM_AUTO_v_trsumprd,
		PARAM_AUTO_v_secprev,
		PARAM_AUTO_v_secnext,
		// autocut用
		PARAM_AUTO_v_trscope,
		PARAM_AUTO_v_tr1stprd,
		// 合計数
		SIZE_PARAM_AUTO
	};
}


