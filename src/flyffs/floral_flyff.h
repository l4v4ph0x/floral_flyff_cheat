#ifndef FLORAL_FLYFF_H
#define FLORAL_FLYFF_H

#include "../a/flyff.h"

#include <vector>

class floral_flyff: public flyff {
	private:
        // offsets >
        static const unsigned long OFFSET_SELECT;
        static const unsigned long OFFSET_X;
        static const unsigned long OFFSET_LVL;
        static const unsigned long OFFSET_IS_DEAD;
        static const unsigned long OFFSET_HP;
        static const unsigned long OFFSET_TYPE_PET;
        static const unsigned long OFFSET_NAME;
        static const unsigned long OFFSET_ID;
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

        unsigned long _range_all_addr;
        unsigned long _range_addr;
        unsigned long _range_nr_addr;

        unsigned long _no_collision_addr;

        bool _use_perin_convert_spam;
        unsigned long _perin_convert_spam_write_addr;
        unsigned long _perin_convert_spam_ecx;
        unsigned long _perin_convert_spam_call;

        unsigned long _init_hp_offset_addr;

        void *_h_select_thread;
        // < end of vars

        // threads
        static unsigned long __stdcall floral_flyff::_thread_select_target(void *t);
        static unsigned long __stdcall floral_flyff::_thread_hper(void *t);
        static unsigned long __stdcall floral_flyff::_thread_perin_converter(void *t);

        static float get_hyp(flyff *f, flyff::targetInfo ti);
        void load(void *handle, unsigned long base_addr, unsigned long base_size, bool light_loading);

	public:
        // constructors
		floral_flyff(void);
        floral_flyff(unsigned long pid, bool light_loading);
        floral_flyff(void *handle, unsigned long base_addr, unsigned long base_size, bool light_loading);
        
		// filling nested struct virtuals
		class ci_localPlayer: public c_localPlayer {
            // props >
            char *name;
            // < end of props


            // get
            char *get_name();
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
        void init_hp_offset();


        // miscs
        // set
        void set_perin_convert_spam(bool state);

        // get
        bool get_perin_convert_spam();

        // do
        void enable_perin_convert_spam(bool state);
};

#endif
