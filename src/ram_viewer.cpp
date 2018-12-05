#include "ram_viewer.h"
#include "mem.h"

RamViewer::RamViewer(TinerasNES* tineras_nes)
    : _tineras_nes(tineras_nes)
{
    _ui.setupUi(this);

    _update_timer.setInterval(250);
    connect(&_update_timer, SIGNAL(timeout()), this, SLOT(updateText()));
    _update_timer.start();
}

RamViewer::~RamViewer()
{
}

void RamViewer::updateText()
{
    if (_tineras_nes->mem() == nullptr)
        return;

    _ui.text_edit_ram_view->clear();

    // Save the current cursor position (DOESN'T CURRENTLY WORK)
    auto saved_cursor = _ui.text_edit_ram_view->textCursor().position();

    _ui.text_edit_ram_view->append("00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F ");
    _ui.text_edit_ram_view->append("-- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- ");

    char dest[1536 * 3];
    unsigned char* mem_cpu = _tineras_nes->mem()->mem();
    for (int i = 0; i < (1536-3); i++) // ??? Had to do (-3) here so it wouldn't crash... why?
    {
        sprintf(&dest[i*3], "%02x ", mem_cpu[i + 0x200]);
    }

    _ui.text_edit_ram_view->append(QString(dest).toUpper());

    // Restore the cursor to its original position (DOESN'T CURRENTLY WORK)
    QTextCursor restored_cursor;
    restored_cursor.setPosition(saved_cursor, QTextCursor::MoveAnchor);
    _ui.text_edit_ram_view->setTextCursor(restored_cursor);
}
