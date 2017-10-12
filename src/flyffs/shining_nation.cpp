#include "shining_nation.h"
#include "../h/losu.h"
#include "../h/summoner.h"
#include "../h/texts.h"

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <ctime>

const unsigned long shining_nation::OFFSET_SELECT = 0x20;         // type = 4 bytes
const unsigned long shining_nation::OFFSET_X = 0x160;             // type = float
const unsigned long shining_nation::OFFSET_LVL = 0x6EC;           // type = 4 bytes
const unsigned long shining_nation::OFFSET_HP = 0x710;            // type 4 bytes
const unsigned long shining_nation::OFFSET_PET_TYPE = 0x720;
const unsigned long shining_nation::OFFSET_MONEY = 0x1638;        // 4 bytes int

//////////////////// threads \\\\\\\\\\\\\\\\\\\\

unsigned long __stdcall shining_nation::_thread_select_target(void *t) {
    bool killed;
    flyff *f;
    flyff::key k;
    flyff::targetInfo ti;
    time_t time_selected;
    unsigned long bad_target;

    f = ((flyff *)t); // main class for every client
    killed = true;

    f->localPlayer->select(0);

    for (;; Sleep(100)) {
        // deal with killing
        if (f->localPlayer->get_select() == 0) {
            if (killed == false) {
                if (f->bot->get_kill_to_home() > 0 && f->bot->killed_count >= f->bot->get_kill_to_home()) {
                    f->localPlayer->teleport_to_saved_pos();
                    f->bot->killed_count = 0;
                    Sleep(1000);
                }

                f->bot->killed_count++;

                // setting bot statis text to searching
                SetWindowText((HWND)f->ui->get_hwnd_noti(), (char *)texts::noti_bot_searching_target);
            }

            // select target if any
            ti = f->bot->get_closest_target_in_view();
            if (ti.base != 0 && ti.base != bad_target) {
                printf("selecting closest target: %08X\n", ti.base);
                f->localPlayer->select(ti.base);

                // get time we select target
                time_selected = time(0);

                if (f->bot->get_kill_to_home() > 0)
                    f->localPlayer->teleport_to_target(ti);

                killed = false;

                // setting bot status text to attacking
                SetWindowText((HWND)f->ui->get_hwnd_noti(), (char *)texts::noti_bot_attacking_target);
            }

            // rotate cam
            f->bot->thread_uing = true;
            PostMessage((HWND)f->ui->get_hwnd(), WM_KEYDOWN, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
            Sleep(50);
            PostMessage((HWND)f->ui->get_hwnd(), WM_KEYUP, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
            Sleep(50);
            f->bot->thread_uing = false;
        } else {
            // get key to use, attack function choice itself key
            //if (f->bot->get_key(&k)) {
                // send key to window
                f->bot->thread_uing = true;
                f->localPlayer->attack();
                Sleep(50);
                f->bot->thread_uing = false;
            //}

            // check time we have time to kill target
            if (f->bot->get_reselect_after() > 0) {
                time_t now = time(0);

                if (time_selected + f->bot->get_reselect_after() < now) {
                    // if we passed time we had to kill target then select 0
                    bad_target = f->localPlayer->get_select();
                    f->localPlayer->select(0);

                    printf("couldn't killd in %d seconds, reselcting target\n", f->bot->get_reselect_after());
                }
            }
        }
    }

    return 0;
}

//////////////////// no class functions \\\\\\\\\\\\\\\\\\\\

float shining_nation::get_hyp(flyff *f, flyff::targetInfo ti) {
    float x = 0,
        z = 0;

    if (OFFSET_X != 0) {
        ZwReadVirtualMemory(f->localPlayer->handle, (void *)(f->localPlayer->get_me() + OFFSET_X), &x, 4, 0);
        ZwReadVirtualMemory(f->localPlayer->handle, (void *)(f->localPlayer->get_me() + OFFSET_X + 8), &z, 4, 0);
    }

    return sqrt((x - ti.x) * (x - ti.x) + (z - ti.z) * (z - ti.z));
}

//////////////////// class contructors \\\\\\\\\\\\\\\\\\\\

shining_nation::shining_nation(void) {}

shining_nation::shining_nation(void *handle, unsigned long base_addr, unsigned long base_size) {
    load(handle, base_addr, base_size);
}

shining_nation::shining_nation(unsigned long pid) {
    void *handle;
    unsigned long base, base_size;

    printf("Shining Nation --- opening proces\n");
    handle = VZwOpenProcess(pid);

    if (handle) {
        // get base and size if available
        base = get_module(pid, "Neuz.exe", &base_size);

        if (base == 0) {
            // set variables21
            base = 0x00AB0000; //get_module(pid, "Neuz.exe", &base_size); // get_module does not work on wine staging 2.9
            base_size = 0x00917000; // this and base from immunity debugger
        }

        error_string = nullptr;
        load(handle, base, base_size);
    }
    else error_string = (char *)texts::error_open_process;
}

void shining_nation::load(void *handle, unsigned long base_addr, unsigned long base_size) {
    unsigned long addr;
    char buf[256];

    printf("Searcing for Shining Nation ... ");

    if (search(handle, base_addr, base_size, "Shining Nation", 14, 1) != 0) {
        printf(" | Done\n");

        _base_addr = base_addr;
        _handle = handle;

        _select_addr = base_addr + 0x617280;
        _maxInView_addr = base_addr + 0x88D698;
        _targetBase_addr = base_addr + 0x887108;
        _me_addr = base_addr + 0x6131E0;

        _range_addr = base_addr + 0x006AB9FC - 0x00400000;
        _range_all_addr = base_addr + 0x006ABCFF - 0x00400000;
        _range_nr_addr = base_addr + 0x168E4C;
        
        _anti_mem_select_addr = base_addr + 0x008FBE18 - 0x00400000;
        _anti_mem_select_call = base_addr + 0x00424E80 - 0x00400000;
        _anti_mem_select_ecx = base_addr + 0x0A0A7D8 - 0x00400000;
        _anti_mem_select_detour_start = base_addr + 0x005731EE - 0x00400000;

        _no_collision_addr = base_addr + 0x9F7B74 - 0x00400000;
        _anti_no_collision_addr = base_addr + 0x00731BE9 - 0x00400000;

        // need implementation
        //init_range();
        // need implementation
        //init_perin_convert_spam();
        init_select();
        init_no_collision();

        // { - waiting _select_addr to point
        printf("waiting when _select_addr points ... ");
        for (addr = 0; !addr; Sleep(20))
            ZwReadVirtualMemory(_handle, (void *)(_select_addr), &addr, 4, 0);
        printf("%08X | Done\n", addr + OFFSET_SELECT);
        // end of waiting _select_add to point - }

        // fillin virtual vars
        localPlayer = new ci_localPlayer();
        bot = new ci_bot();

        // filling with few vars
        localPlayer->parent = this;
        localPlayer->handle = _handle;
        localPlayer->select_addr = _select_addr;
        localPlayer->me_addr = _me_addr;
        localPlayer->no_collision_addr = _no_collision_addr;
        localPlayer->range_nr_addr = _range_nr_addr;
        localPlayer->anti_mem_select_addr = _anti_mem_select_addr;

        localPlayer->max_range = 4.f;

        bot->parent = this;
        bot->handle = _handle;
        bot->maxInView_addr = _maxInView_addr;
        bot->targetBase_addr = _targetBase_addr;

        // { - waiting _me_addr to point
        printf("waiting when _me_addr points ... ");
        for (addr = 0; !addr; Sleep(20))
            addr = localPlayer->get_me();
        printf("%08X | Done\n", addr);
        // end of waiting _me_addr to point - }

        // printing some local vars
        printf("local money: %d\n", localPlayer->get_money());
        localPlayer->get_name(buf);
        printf("local name: %s\n", buf);

        // nulling some vars
        bot->h_select_thread = nullptr;
        bot->set_kill_to_home(0);
        bot->killed_count = 0;
        bot->set_reselect_after(0);
        memset(localPlayer->saved_pos, '\x00', 12);
    }
    // if we havent found flyff we give nothing
    else {
        printf(" | Failed\n");
        error_string = (char *)texts::error_flyff_not_found;

        // close function
        return;
    }
}


//////////////////// localPlayer \\\\\\\\\\\\\\\\\\\\
// ------------------------------------------------- gets
void shining_nation::ci_localPlayer::get_name(char *name) {
    memcpy(&*name, "Shining Nation: can't get name", 31);

    // ruskii is in cyrillic and its unicode, this code is in multibyte
    //if (OFFSET_NAME != 0)
    //    ZwReadVirtualMemory(handle, (void *)(get_me() + OFFSET_NAME), &*name, 255, 0);
}

unsigned int shining_nation::ci_localPlayer::get_money() {
    unsigned int money = 0;

    if (OFFSET_MONEY != 0)
        ZwReadVirtualMemory(handle, (void *)(get_me() + OFFSET_MONEY), &money, 4, 0);

    return money;
}


unsigned long shining_nation::ci_localPlayer::get_me() {
    unsigned long value = 0;
    ZwReadVirtualMemory(handle, (void *)(me_addr), &value, 4, 0);
    return value;
}

unsigned long shining_nation::ci_localPlayer::get_select() {
    unsigned long pointed = 0;

    if (OFFSET_SELECT != 0) {
        ZwReadVirtualMemory(handle, (void *)(select_addr), &pointed, 4, 0);
        ZwReadVirtualMemory(handle, (void *)(pointed + OFFSET_SELECT), &pointed, 4, 0);
    }

    return pointed;
}

void shining_nation::ci_localPlayer::get_location(unsigned char *loc) {
    memcpy(loc, saved_pos, 12);
}

bool shining_nation::ci_localPlayer::get_no_collision() {
    bool collision;
    ZwReadVirtualMemory(handle, (void *)(no_collision_addr), &collision, 1, 0);
    return !collision;
}


// ------------------------------------------------- sets
void shining_nation::ci_localPlayer::save_location(unsigned char *loc) {
    // if loc is null then getting local player pos, else given loc
    if (loc == nullptr)
        if (OFFSET_X != 0)
            ZwReadVirtualMemory(handle, (void *)(get_me() + OFFSET_X), &saved_pos, 12, 0);
        else memcpy(saved_pos, loc, 12);

        *(float *)(saved_pos + 4) += 2.f;
}

void shining_nation::ci_localPlayer::set_no_collision(bool state) {
    if (state == true)
        ZwWriteVirtualMemory(handle, (void *)(no_collision_addr), "\x00", 1, 0, true);
    else
        ZwWriteVirtualMemory(handle, (void *)(no_collision_addr), "\x01", 1, 0, true);
}

float shining_nation::ci_localPlayer::set_range(float f) {
    if (f > get_max_range()) {
        f = get_max_range();
        printf("Shining Nation flyff only can use max %f range\n", f);
    }
    // set range number
    ZwWriteVirtualMemory(handle, (void *)(range_nr_addr), &f, 4, 0);

    // returning value that we really wrote
    return f;
}

// ------------------------------------------------- something to do
void shining_nation::ci_localPlayer::teleport_to_saved_pos() {
    if (OFFSET_X != 0)
        ZwWriteVirtualMemory(handle, (void *)(get_me() + OFFSET_X), &saved_pos, 12, 0);
}

void shining_nation::ci_localPlayer::teleport_to_target(targetInfo target) {
    unsigned char pos[12];

    if (OFFSET_X != 0) {
        ZwReadVirtualMemory(handle, (void *)(target.base + OFFSET_X), &pos, 12, 0);
        *(float *)(pos + 4) += 2.f;
        ZwWriteVirtualMemory(handle, (void *)(get_me() + OFFSET_X), &pos, 12, 0);
    }
}

void shining_nation::ci_localPlayer::select(unsigned long target) {
    unsigned long pointed = 0;

    if (OFFSET_SELECT != 0) {
        ZwReadVirtualMemory(handle, (void *)(select_addr), &pointed, 4, 0);
        ZwWriteVirtualMemory(handle, (void *)(pointed + OFFSET_SELECT), &target, 4, 0);
        Sleep(100);

        if (target != 0)
            ZwWriteVirtualMemory(handle, (void *)(anti_mem_select_addr), "\x75", 1, 0);
    }
}

void shining_nation::ci_localPlayer::attack() {
    key k;

    if (parent->bot->get_key(&k)) {
        PostMessage((HWND)parent->ui->get_hwnd(), WM_KEYDOWN, k.code, MapVirtualKey(k.code, MAPVK_VK_TO_VSC));
        Sleep(50);
        PostMessage((HWND)parent->ui->get_hwnd(), WM_KEYUP, k.code, MapVirtualKey(k.code, MAPVK_VK_TO_VSC));
    }
}

//////////////////// bot \\\\\\\\\\\\\\\\\\\\
// ------------------------------------------------- gets
flyff::targetInfo shining_nation::ci_bot::get_closest_target_in_view() {
    unsigned long maxInView;
    unsigned long target;
    unsigned long type;
    unsigned long lvl;
    unsigned long pet_type;
    unsigned long hp;

    targetInfo closest_ti;

    maxInView = 0;
    closest_ti = targetInfo();
    closest_ti.hyp = 99999999.f;

    ZwReadVirtualMemory(handle, (void *)(maxInView_addr), &maxInView, 4, 0);

    //printf("maxInView: %d\n", maxInView);

    for (unsigned long i = 1; i < maxInView; i++) {
        target = 0;
        type = 0;
        lvl = 0;
        hp = 0;
        pet_type = 0;

        ZwReadVirtualMemory(handle, (void *)(i * 4 + targetBase_addr), &target, 4, 0);
        ZwReadVirtualMemory(handle, (void *)(target + 4), &type, 4, 0);
        ZwReadVirtualMemory(handle, (void *)(target + OFFSET_LVL), &lvl, 4, 0);
        ZwReadVirtualMemory(handle, (void *)(target + OFFSET_HP), &hp, 4, 0);
        ZwReadVirtualMemory(handle, (void *)(target + OFFSET_PET_TYPE), &pet_type, 4, 0);

        //printf("base: %08X\ntarget: %08X\ntype: %d\nlvl: %d\n", 
        //s    i * 4 + targetBase_addr, target, type, lvl);

        if (type == 18 && lvl >= target_lvl_begin && lvl <= target_lvl_end && hp > 1 && pet_type != 19) {
            targetInfo ti;
            ZwReadVirtualMemory(handle, (void *)(target + OFFSET_X), &ti.x, 4, 0);
            ZwReadVirtualMemory(handle, (void *)(target + OFFSET_X + 4), &ti.y, 4, 0);
            ZwReadVirtualMemory(handle, (void *)(target + OFFSET_X + 8), &ti.z, 4, 0);
            ti.hyp = get_hyp(this->parent, ti);

            if (ti.hyp < closest_ti.hyp) {
                ti.base = target;
                ti.lvl = lvl;
                closest_ti = ti;
            }
        }
    }

    return closest_ti;
}

bool shining_nation::ci_bot::get_key(key *k) {
    if (keys.size() > 0) {
        *k = keys[0];
        return true;
    }
    return false;
}


// ------------------------------------------------- sets
void shining_nation::ci_bot::add_update_attack_key(key k, bool remove) {
    if (remove) {
        for (int i = 0; i < keys.size(); i++) {
            if (keys[i].code == k.code && keys[i].priority == k.priority) {
                keys.erase(keys.begin() + i - 1);
                break;
            }
        }
    }
    else keys.push_back(k);
}

// ------------------------------------------------- something to do
bool shining_nation::ci_bot::run() {
    // save new position when bot enables
    parent->localPlayer->save_location();
    // resets vars
    killed_count = 0;

    // running target selecting, killing thread
    h_select_thread = CreateThread(0, 0, _thread_select_target, this->parent, 0, 0);
    // set bot status text to created
    SendMessage((HWND)parent->ui->get_hwnd_noti(), WM_SETTEXT, 0, (LPARAM)texts::noti_bot_created);

    return get_run();
}

void shining_nation::ci_bot::stop() {
    // waiting for thread to finish all keypresses
    for (Sleep(50); thread_uing; Sleep(50));
    // terminating target selecting and killing thread and nulling vars
    TerminateThread(h_select_thread, 0);
    h_select_thread = nullptr;
    // set bot status text to idle
    SetWindowText((HWND)parent->ui->get_hwnd_noti(), (char *)texts::noti_bot_idle);
}


// initializings
void shining_nation::init_range() {
    ZwWriteVirtualMemory(_handle, (void *)_range_addr,
        "\xA1\x4C\x8E\x56\x00\x89\x44\x24\x0C\x90\x90\x90\x90\x90\x90\x90\x90",
        17, 0, true);
    ZwWriteVirtualMemory(_handle, (void *)(_range_addr +1), &_range_nr_addr, 4, 0, true);
}

void shining_nation::init_perin_convert_spam() {
    // need implementation
}

void shining_nation::init_select() {
    unsigned long pointed;
    unsigned long to_back;
    unsigned long to_detour;

    VirtualProtectEx(_handle, (void *)_anti_mem_select_addr, 38+8, PAGE_EXECUTE_READWRITE, (LPDWORD)&pointed);
    ZwWriteVirtualMemory(_handle, (void *)_anti_mem_select_addr,
        "\xEB\x1F\xA1\xE8\x2A\x8C\x0A\x8B\x80\xF0\x02\x00\x00\x6A\x02\x50\xB9\xD8\xA7\xA0\x00\xE8\x4E\x90\xB2\xFF\xC6\x05\x18\xBE\x8F\x00\xEB\x8B\x8C\x24\xDC\x00\x00\x00\xE9\xB0\x73\xC7\xFF\x90",
        38+8, 0);

    // getting pointed select addr
    ZwReadVirtualMemory(_handle, (void *)(_select_addr), &pointed, 4, 0);
    pointed += OFFSET_SELECT;
    // andr writin it
    ZwWriteVirtualMemory(_handle, (void *)(_anti_mem_select_addr +3), &pointed, 4, 0);
    ZwWriteVirtualMemory(_handle, (void *)(_anti_mem_select_addr + 0x11), &_anti_mem_select_ecx, 4, 0);
    // calculating call from detured func
    _anti_mem_select_call -= _anti_mem_select_addr + 0x16 + 4;
    ZwWriteVirtualMemory(_handle, (void *)(_anti_mem_select_addr + 0x16), &_anti_mem_select_call, 4, 0);
    // calculating back to original func
    to_back = _anti_mem_select_detour_start - (_anti_mem_select_addr + 0x28) + 7 - 5;
    ZwWriteVirtualMemory(_handle, (void *)(_anti_mem_select_addr + 0x28 +1), &to_back, 4, 0);

    ZwWriteVirtualMemory(_handle, (void *)_anti_mem_select_detour_start, "\xE9\x25\x8C\x38\x00\x90\x90", 7, 0);
    // calculating to detour function
    to_detour = _anti_mem_select_addr - _anti_mem_select_detour_start - 5;
    ZwWriteVirtualMemory(_handle, (void *)(_anti_mem_select_detour_start +1), &to_detour, 4, 0);

}

void shining_nation::init_no_collision() {
    ZwWriteVirtualMemory(_handle, (void *)_anti_no_collision_addr, "\xEB", 1, 0, true);
}


//////////////////// miscs \\\\\\\\\\\\\\\\\\\\
// ------------------------------------------------- sets
void shining_nation::set_perin_convert_spam(bool state) {
    
}

// ------------------------------------------------- gets
bool shining_nation::get_perin_convert_spam() {
    return _use_perin_convert_spam;
}

// ------------------------------------------------- something to do
void shining_nation::enable_perin_convert_spam(bool state) {
    
}
