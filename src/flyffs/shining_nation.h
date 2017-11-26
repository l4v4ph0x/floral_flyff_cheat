#ifndef SHINING_NATION_H
#define SHINING_NATION_H

#include "../a/flyff.h"

class shining_nation : public flyff {
    private:
        // offsets >
        static const unsigned long OFFSET_SELECT;
        static const unsigned long OFFSET_X;
        static const unsigned long OFFSET_LVL;
        static const unsigned long OFFSET_HP;
        static const unsigned long OFFSET_PET_TYPE;
        static const unsigned long OFFSET_MONEY;
        // < end of offsets

        // vars >
        void *_hwnd;
        void *_handle;

        unsigned long _base_addr;
        unsigned long _select_addr;
        unsigned long _maxInView_addr;
        unsigned long _targetBase_addr;
        unsigned long _me_addr;

        unsigned long _anti_mem_select_addr;
        unsigned long _anti_mem_select_call;
        unsigned long _anti_mem_select_ecx;
        unsigned long _anti_mem_select_detour_start;

        unsigned long _range_all_addr;
        unsigned long _range_addr;
        unsigned long _range_nr_addr;

        unsigned long _no_collision_addr;
        unsigned long _anti_no_collision_addr;

        bool _use_perin_convert_spam;
        unsigned long _perin_convert_spam_write_addr;
        unsigned long _perin_convert_spam_ecx;
        void *_h_select_thread;
		void *_h_hper_thread;
        // < end of vars

		// threads
		static unsigned long __stdcall _thread_select_target(void *t);
		static unsigned long __stdcall _thread_hper(void *t);

		// were no class funcs
        static float get_hyp(flyff *f, flyff::targetInfo ti);
        void load(void *handle, unsigned long base_addr, unsigned long base_size);

    public:
        // constructors
        shining_nation(void);
        shining_nation(void *handle, unsigned long base_addr, unsigned long base_size);
        shining_nation(unsigned long pid);

        // filling nested struct virtuals
        class ci_localPlayer : public c_localPlayer {
            // get
            void get_name(char *name);
            unsigned int get_money();
			unsigned int get_hp();
            unsigned long c_localPlayer::get_me();
            unsigned long get_select();
            void get_location(unsigned char *loc);
            bool get_no_collision();

            // set
            void save_location(unsigned char *loc = nullptr);
            void set_no_collision(bool state);
            float set_range(float f);

            // do
            void teleport_to_saved_pos();
            void teleport_to_target(targetInfo target);
            void select(unsigned long target);
            void attack();
        };

        class ci_bot : public c_bot {
            // get
            targetInfo get_closest_target_in_view();
            bool get_key(key *k);

            // set
            void add_update_attack_key(key k, bool remove = false);

            // do
            bool run();
            void stop();
        };

		class ci_buff : public c_buff {
			// get
			bool get_run();

			// do
			void run(bool state);
		};

        void init_range();
        void init_perin_convert_spam();
        void init_select(unsigned char *detourReplaceBytes);
        void init_no_collision();

        // miscs
        // set
        void set_perin_convert_spam(bool state);

        // get
        bool get_perin_convert_spam();

        // do
        void enable_perin_convert_spam(bool state);
};

#endif
