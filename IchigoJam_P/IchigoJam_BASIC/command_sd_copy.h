/*
 * SD 開始番号,終了番号,コピー先開始番号
 * 内蔵FlashとSD間をコピーする。BASICの編集中プログラムは変更しない。
 * SDドライバーの既存4Kバッファを使い、コピー用RAMを追加しない。
 */
#ifndef GIVETAKE_COMMAND_SD_COPY_H
#define GIVETAKE_COMMAND_SD_COPY_H

// コピー元・コピー先の保存先が違い、両範囲が全て有効か最初に検査する。
static bool sd_copy_valid_range(int first, int last, int target) {
    if (first < 0 || last < first) return false;
    if (first < N_FLASH_STORAGE && last < N_FLASH_STORAGE)
        return target >= 200 && target <= 299 && last-first <= 299-target;
    if (first >= 200 && last <= 299)
        return target >= 0 && target < N_FLASH_STORAGE && last-first < N_FLASH_STORAGE-target;
    return false;
}

// エラー時も番号を明示する。完了済みのコピーは取り消さない。
static void sd_copy_report_error(int source, int target) {
    put_str("SD copy error "); put_num(source); put_str(" -> "); put_num(target);
    if (sd_write_error) { put_str(" write="); put_num(sd_write_error); }
    else if (sd_file_error) { put_str(" file="); put_num(sd_file_error); }
    put_chr('\n');
    if (sd_save_committed) put_str("File committed; cleanup failed\n");
    else if (sd_save_commit_started) put_str("Directory update uncertain\n");
    command_error(ERR_FILE_ERROR);
}

static void command_sd_copy(void) {
    int first=token_expression();
    IJB_ERR_CHK();
    if (token_getCode()!=TOKEN_COMMA) { command_error(ERR_SYNTAX_ERROR); return; }
    int last=token_expression();
    IJB_ERR_CHK();
    if (token_getCode()!=TOKEN_COMMA) { command_error(ERR_SYNTAX_ERROR); return; }
    int target=token_expression();
    IJB_ERR_CHK();
    token_end();
    IJB_ERR_CHK();

    // 全範囲の検査に合格するまで、SDもFlashも書き換えない。
    if (!sd_copy_valid_range(first,last,target)) {
        command_error(ERR_ILLEGAL_ARGUMENT); return;
    }
    if (!sd_initialized || !sd_fat_ready) {
        sd_file_error=36; sd_write_error=0;
        sd_save_committed=sd_save_commit_started=false;
        sd_copy_report_error(first,target); return;
    }
    int previous_file=_g.lastfile;
    for (int number=first; number<=last; number++,target++) {
        int size;
        sd_file_error=sd_write_error=0;
        sd_save_committed=sd_save_commit_started=false;
        if (first<200) {
            // IJB_loadのinit=0でFILE()を変更せずにFlashから読み込む。
            // 未使用の消去済みFlashは-1を返し、先頭2バイトを0にする既存仕様。
            memset(sd_file_data,0xa5,sizeof(sd_file_data));
            int result=IJB_load(number,(uint8 *)sd_file_data,4096,0);
            if (result<0 && sd_le16(sd_file_data)==0) size=0;
            else if (result!=4096) size=-1;
            else size=sd_validate_program(sd_file_data,4096);
        } else {
            // 通常LOADと同じIJB優先。BASしかなければ変換して読む。
            size=sd_prepare_program(number);
            if (size<0 && sd_file_error==30) { size=0; sd_file_error=0; }
        }
        if (size<0) {
            if (!sd_file_error) sd_file_error=37;
            sd_copy_report_error(number,target); return;
        }
        if (size==0) {
            // 空のコピー元でコピー先を消さない。番号の対応も詰めない。
            if (!noresmode) { put_str("Skipped "); put_num(number); put_chr('\n'); }
            continue;
        }
        // 内蔵Flashは常に4K書き込むため、使用量以降を0で埋めて終端を保証する。
        if (size<4096) memset(sd_file_data+size,0,4096-(unsigned)size);
        if (first<200) {
            // SDへのコピーはIJB形式。正常確定後に同番号のBASを削除する。
            if (!sd_save_program(target,sd_file_data,(unsigned)size,0)) {
                sd_copy_report_error(number,target); return;
            }
        } else {
            // 既存のFlash書込み経路（DVI停止/再開・排他処理を含む）を使う。
            int result=IJB_save(target,(uint8 *)sd_file_data,size);
            _g.lastfile=previous_file; // IJB_saveが変更するFILE()を失敗時にも復元する。
            if (result) { sd_copy_report_error(number,target); return; }
            // 既存のBAS用一時領域で4Kを読み戻し、書いた内容と全バイト比較する。
            int readback=IJB_load(target,(uint8 *)sd_text_data,4096,0);
            if (readback!=4096 || memcmp(sd_file_data,sd_text_data,4096)!=0) {
                sd_copy_report_error(number,target); return;
            }
        }
        if (!noresmode) {
            put_str("Copied "); put_num(number); put_str(" -> "); put_num(target); put_chr('\n');
        }
    }
}
#endif
