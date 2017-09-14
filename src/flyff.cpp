#include "h/flyff.h"
#include "h/losu.h"
#include "h/summoner.h"
#include "h/texts.h"

#include <windows.h>
#include <stdio.h>
#include <vector>
#include <cmath>
#include <ctime>

// courius adds
//Neuz.exe + 5E5490, makes most of items very big, 
//    so to bot make this float val to 0and just run autoclicker on screen because targets are mega big

unsigned long OFFSET_SELECT = 0x20;             // type = 4 bytes
unsigned long OFFSET_X = 0x188;                 // type = float
unsigned long OFFSET_LVL = 0x79C + 8;           // type = 4 bytes
unsigned long OFFSET_IS_DEAD = 0x900 + 8;       // 255 = alive, 6 = dead, type = 1 byte
unsigned long OFFSET_TYPE_PET = 0x7EC + 8;      // type = 1 byte, 19 = pet, 0 = npc, 3 = aibatt
unsigned long OFFSET_NAME = 0x1890 + 8;         // char array
unsigned long OFFSET_ID = 0x3F4;				// 4 byte int
unsigned long OFFSET_MONEY = 0x1884 + 8;		// 5 byte array int

// no class functinos
unsigned long __stdcall _thread_select_target(void *t) {
    bool killed;
    flyff f;
    flyff::key k;
    flyff::targetInfo ti;
	time_t time_selected;
	unsigned long bad_target;

    f = *((flyff *)t); // main class for every client
    killed = true;
    
    f.select(0);
    
    for (;; Sleep(100)) {
		// deal with killing
        if (f.getSelect() == 0) {
            if (killed == false) {
                if (f.get_kill_to_home() > 0 && f.get_killed_count() >= f.get_kill_to_home()) {
                    f.teleport_to_saved_pos();
                    f.set_killed_count(0);
                    Sleep(1000);
                }
                
                f.set_killed_count(f.get_killed_count() + 1);
                
                // setting bot statis text to searching
                SetWindowText((HWND)f.get_hwnd_noti(), (char *)texts::noti_bot_searching_target);
            }
            
            // select target if any
            ti = f.getClosestTargetInView();
            if (ti.base != 0 && ti.base != bad_target) {
                printf("selecting closest target: %08X\n", ti.base);
                f.select(ti.base);

				// get time we select target
				time_selected = time(0);
                
                if (f.get_kill_to_home() > 0)
                    f.teleport_to_target(ti.base);
                
                killed = false;
                
                // setting bot status text to attacking
                SetWindowText((HWND)f.get_hwnd_noti(), (char *)texts::noti_bot_attacking_target);
            }
            
            // rotate cam
			f.thread_uing = true;
            PostMessage((HWND)f.get_hwnd(), WM_KEYDOWN, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
            Sleep(50);
            PostMessage((HWND)f.get_hwnd(), WM_KEYUP, VK_RIGHT, MapVirtualKey(VK_RIGHT, MAPVK_VK_TO_VSC));
			f.thread_uing = false;
        } else {
            // get key to use
            if (f.getKey(&k)) {
                // send key to window
				f.thread_uing = true;
                Sleep(20);
                PostMessage((HWND)f.get_hwnd(), WM_KEYDOWN, k.code, MapVirtualKey(k.code, MAPVK_VK_TO_VSC));
                Sleep(50);
                PostMessage((HWND)f.get_hwnd(), WM_KEYUP, k.code, MapVirtualKey(k.code, MAPVK_VK_TO_VSC));
				f.thread_uing = false;
            }

			// check time we have time to kill target
			if (f.get_reselect_after() > 0) {
				time_t now = time(0);

				if (time_selected + f.get_reselect_after() < now) {
					// if we passed time we had to kill target then select 0
					bad_target = f.getSelect();
					f.select(0);

					printf("couldn't killd in %d seconds, reselcting target\n", f.get_reselect_after());
				}
			}
        }
    }
    
    return 0;
}

unsigned long __stdcall _thread_perin_converter(void *t) {
    flyff f;

    f = *((flyff *)t); // main class for every client

	for (;; Sleep(1000)) {
		// check does we need to convert to perin
		//if (f.get_perin_convert_spam()) { // no need to check since 9.2.2017
			if (f.get_local_money() >= 100000000)
				f.enable_perin_convert_spam(true);
			else f.enable_perin_convert_spam(false);
		//}
	}
}

// end of no class functinos

flyff::flyff(void) {}

flyff::flyff(void *handle, unsigned long base_addr, unsigned long base_size) {
    load(handle, base_addr, base_size);	
}

flyff::flyff(unsigned long pid) {
    void *handle;
    unsigned long base, base_size;
    
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
    } else error_string = (char *)texts::error_open_process;
}

void flyff::load(void *handle, unsigned long base_addr, unsigned long base_size) {
    unsigned long addr;
	char buf[256];
    
    // null some
    _vars._h_select_thread = nullptr;
    
	printf("Searcing for flyff ... ");
    if (search(handle, base_addr, base_size, "Floral Flyff", 12, 1) != 0) {
		printf("Floral Flyff | Done\n");
        _vars._base_addr = base_addr;
        _vars._handle = handle;
        
        // updated 8.26.2017
        _vars._select_addr = base_addr + 0x66EE04; // old 0x66EDE4(dif: +20)
		// updated 8.26.2017
        _vars._maxInView_addr = base_addr + 0x668D88 + 0x20;
		// updated 8.26.2017
        _vars._targetBase_addr = base_addr + 0x65E5F0 + 0x20;
		// updated 8.26.2017
        _vars._me_addr = base_addr + 0x659A48 + 0x20;
        
        // updated 8.27.2017
        _vars._range_nr_addr = base_addr + 0x00FC7AFC - 0x00980000; // old 0x66FDA0
		// updated 8.26.2017
        _vars._range_addr = base_addr + 0x2A62B1; //old 0x2A6161(dif: +150)
		// updated 8.26.2017
        _vars._range_all_addr = base_addr + 0x2A654A + 0x150;
        
		// update 8.27.2017
        _vars._no_collision_addr = base_addr + 0xFC00BC - 0x00980000; // old 0x6400BC

		// updated 9.2.2017
		_vars._perin_convert_spam_write_addr = base_addr + 0x249096; // old 0x249016(dif: +80)
		// updated 9.2.2017
		_vars._perin_convert_spam_ecx = base_addr + 0x66B628; // old 0x66B608(dif: +20)
        
		init_perin_convert_spam();
		

        // { - waiting _select_addr to point
        printf("waiting when _select_addr points ... ");
        for (addr = 0; !addr; Sleep(20))
            ZwReadVirtualMemory(_vars._handle, (void *)(_vars._select_addr), &addr, 4, 0);
        printf("%08X | Done\n", addr + OFFSET_SELECT);
        // end of waiting _select_add to point - }

        
        // { - waiting _me_addr to point
        printf("waiting when _me_addr points ... ");
		for (addr = 0; !addr; Sleep(20))
			addr = getMe();
        printf("%08X | Done\n", addr);
        // end of waiting _me_addr to point - }

		// printing some local vars
		printf("local money: %d\n", get_local_money());
		get_local_name(buf);
		printf("local name: %s\n", buf);

        // nulling some vars
        set_kill_to_home(0);
        set_killed_count(0);
        memset(_vars._saved_pos, '\x00', 12);
		set_reselect_after(0);
    } else {
        error_string = (char *)texts::error_wrong_flyff;
    }
}

bool flyff::run(bool run) {
    if (_vars._h_select_thread == nullptr) {
        if (run == true) {
            // if position not saved yet then doing so
            if (memcmp(_vars._saved_pos, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0)
                save_location();
            
            // running target selecting, killing thread
            _vars._h_select_thread = CreateThread(0, 0, _thread_select_target, this, 0, 0);
            // set bot status text to created
            SendMessage((HWND)get_hwnd_noti(), WM_SETTEXT, 0, (LPARAM)texts::noti_bot_created);
        }
        
        return false;
    } return true;
}
void flyff::stop() {
	// waiting for thread to finish all keypresses
	for (; thread_uing; Sleep(50));
    // terminating target selecting and killing thread and nulling vars
    TerminateThread(_vars._h_select_thread, 0);
    _vars._h_select_thread = nullptr;
    memset(_vars._saved_pos, '\x00', 12);
    // set bot status text to idle
    SetWindowText((HWND)get_hwnd_noti(), (char *)texts::noti_bot_idle);
}

void flyff::set_hwnd(void *hwnd) { _vars._hwnd = hwnd; }
void *flyff::get_hwnd() { return _vars._hwnd; }

void flyff::get_local_name(char *name) {
    ZwReadVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_NAME), &*name, 255, 0);
}
unsigned int flyff::get_local_money() {
	unsigned int money;
	ZwReadVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_MONEY), &money, 4, 0);
	return money;
}

void flyff::select(unsigned long target) {
	unsigned long pointed = 0;
	ZwReadVirtualMemory(_vars._handle, (void *)(_vars._select_addr), &pointed, 4, 0);
	ZwWriteVirtualMemory(_vars._handle, (void *)(pointed + OFFSET_SELECT), &target, 4, 0);
}

unsigned long flyff::getSelect() {
	unsigned long pointed = 0;
	ZwReadVirtualMemory(_vars._handle, (void *)(_vars._select_addr), &pointed, 4, 0);
	ZwReadVirtualMemory(_vars._handle, (void *)(pointed + OFFSET_SELECT), &pointed, 4, 0);
	return pointed;
}

unsigned long flyff::getMe() {
	unsigned long value = 0;
	ZwReadVirtualMemory(_vars._handle, (void *)(_vars._me_addr), &value, 4, 0);
	return value;
}

float flyff::getHyp(targetInfo ti) {
	float x, z;
	ZwReadVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_X), &x, 4, 0);
	ZwReadVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_X +8), &z, 4, 0);
	return sqrt((x - ti.x) * (x - ti.x) + (z - ti.z) * (z - ti.z));
}

flyff::targetInfo flyff::getClosestTargetInView() {
	unsigned long maxInView;
    unsigned long target;
    unsigned long type;
    unsigned long lvl;
    unsigned char is_dead;
    unsigned long type_pet;
    
	targetInfo closest_ti;
    
    maxInView = 0;
    closest_ti = targetInfo();
	closest_ti.hyp = 99999999.f;

	ZwReadVirtualMemory(_vars._handle, (void *)(_vars._maxInView_addr), &maxInView, 4, 0);
    
    //printf("maxInView: %d\n", maxInView);
    
	for (unsigned long i = 1; i < maxInView; i++) {
		target = 0;
        type = 0;
        lvl = 0;
        is_dead = 0;
        type_pet = 0;

		ZwReadVirtualMemory(_vars._handle, (void *)(i * 4 + _vars._targetBase_addr), &target, 4, 0);
		ZwReadVirtualMemory(_vars._handle, (void *)(target + 8), &type, 4, 0);
		ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_LVL), &lvl, 4, 0);
		ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_IS_DEAD), &is_dead, 1, 0);
		ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_TYPE_PET), &type_pet, 1, 0);

        //printf("base: %08X\ntarget: %08X\ntype: %d\nlvl: %d\nis_dead: %d\n", i * 4 + _vars._targetBase_addr, target, type, lvl, is_dead);
        
		if (type == 18 && lvl >= _vars._target_lvl_begin && lvl <= _vars._target_lvl_end && is_dead == 255 && type_pet != 19 && type_pet != 0) {
			targetInfo ti;
			ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_X), &ti.x, 4, 0);
			ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_X +4), &ti.y, 4, 0);
			ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_X +8), &ti.z, 4, 0);
			ti.hyp = getHyp(ti);

			if (ti.hyp < closest_ti.hyp) {
				ti.base = target;
				ti.lvl = lvl;
				closest_ti = ti;
			}
		}
	}
	
	return closest_ti;
}


void flyff::addUpdateAttackKey(unsigned char key, float priority, bool remove) {
    flyff::key k;
    k.code = key;
    k.priority = priority;
    
    _vars._keys.clear();
    _vars._keys.push_back(k);
}

bool flyff::getKey(flyff::key *k) {
    if (_vars._keys.size() > 0) {
        *k = _vars._keys[0];
        return true;
    }
    return false;
}

void flyff::attack() {
    
}

void flyff::set_target_lvls(int begin, int end) {
    if (begin != -1) _vars._target_lvl_begin = begin;
    if (end != -1) _vars._target_lvl_end = end;
}

void flyff::get_target_lvls(int *begin, int *end) {
    *begin = _vars._target_lvl_begin;
    *end = _vars._target_lvl_end;
}


void flyff::teleport_to_target(unsigned long target) {
	unsigned char pos[12];
	ZwReadVirtualMemory(_vars._handle, (void *)(target + OFFSET_X), &pos, 12, 0);
	*(float *)(pos +4) += 2.f;
	ZwWriteVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_X), &pos, 12, 0);
}

void flyff::save_location(unsigned char *loc) {
    // if loc is null then getting local player pos, else given loc
    if (loc == nullptr)
        ZwReadVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_X), &_vars._saved_pos, 12, 0);
    else memcpy(_vars._saved_pos, loc, 12);
    
	*(float *)(_vars._saved_pos +4) += 2.f;
}

void flyff::get_location(unsigned char *loc) {
    // returning saved loc
    memcpy(loc,  _vars._saved_pos, 12);
}

void flyff::teleport_to_saved_pos() {
	ZwWriteVirtualMemory(_vars._handle, (void *)(getMe() + OFFSET_X), &_vars._saved_pos, 12, 0);
}

double flyff::get_killed_count() { return _vars._killed_count; }
void flyff::set_killed_count(double val) { _vars._killed_count = val; }
double flyff::get_kill_to_home() { return _vars._kill_to_home; }
void flyff::set_kill_to_home(double val) { _vars._kill_to_home = val; }

void flyff::set_range(float f) {
    if (bo_set_range == false) {
        // enabling range for everyone
        ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._range_all_addr), (void *)"\x90\x90", 2, 0, true);
            
        // force to use set range
        ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._range_addr), (void *)"\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90", 12, 0, true);
        ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._range_addr +12 +2), &_vars._range_nr_addr, 4, 0, true);
        
        bo_set_range = true;
    }
    
    // set range number
    ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._range_nr_addr), &f, 4, 0);
}

void flyff::set_no_collision(bool state) {
    if (state == true)
        ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._no_collision_addr), "\x00", 1, 0, true);
    else
        ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._no_collision_addr), "\x01", 1, 0, true);
}
bool flyff::get_no_collision() {
    bool collision;
    ZwReadVirtualMemory(_vars._handle, (void *)(_vars._no_collision_addr), &collision, 1, 0);
    return !collision;
}

void flyff::set_perin_convert_spam(bool state) {
	_vars._use_perin_convert_spam = state;

	if (state)
		// make thread to check penya nad convert if needed
        _vars.h_perin_converter = CreateThread(0, 0, _thread_perin_converter, this, 0, 0);
	else
		// kill thread if checkbox is unchecked
		TerminateThread(_vars._h_select_thread, 0);
}
bool flyff::get_perin_convert_spam() {
	return _vars._use_perin_convert_spam;
}
void flyff::init_perin_convert_spam() {
	ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._perin_convert_spam_write_addr),
		// old "\xEB\x35\x68\xC8\xCC\x00\x00\xB9\x08\xB6\x9A\x00\xE8\x59\x19\x23\x00\xEB\x24\x90", 20, 0, true);
		// updated 9.2.2017
		"\xEB\x35\x68\xC8\xCC\x00\x00\xB9\x08\xB6\x9A\x00\xE8\x29\x1A\x23\x00\xEB\x24\x90", 20, 0, true);
	ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._perin_convert_spam_write_addr + 8), &_vars._perin_convert_spam_ecx, 4, 0, true);
}
void flyff::enable_perin_convert_spam(bool state) {
	unsigned char bytes[2];

	ZwReadVirtualMemory(_vars._handle, (void *)(_vars._perin_convert_spam_write_addr), &bytes, 2, 0);

	if (state == true) {
		if (memcpy(bytes, "\x90\x90", 2) != 0)
			ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._perin_convert_spam_write_addr), "\x90\x90", 2, 0, true);
	} else if (memcpy(bytes, "\xEB\x35", 2) != 0)
		ZwWriteVirtualMemory(_vars._handle, (void *)(_vars._perin_convert_spam_write_addr), "\xEB\x35", 2, 0, true);
}

void flyff::set_reselect_after(int seconds) {
	_vars.reselect_after = seconds;
}
int flyff::get_reselect_after() {
	return _vars.reselect_after;
}


void flyff::set_hwnd_noti(void *hwnd) {
    hwnd_noti = hwnd;
}
void *flyff::get_hwnd_noti() {
    return hwnd_noti;
}
