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
        void *_h_select_thread;
        // < end of vars

        static unsigned long __stdcall floral_flyff::_thread_select_target(void *t);
        static float get_hyp(flyff *f, flyff::targetInfo ti);
        void load(void *handle, unsigned long base_addr, unsigned long base_size);

		/*
        // variables
        struct vars {
            unsigned long _base_addr;
            unsigned long _select_addr;
            unsigned long _maxInView_addr;
            unsigned long _targetBase_addr;
            unsigned long _me_addr;
            
            unsigned char _saved_pos[12];
            double _killed_count;
            double _kill_to_home;
            
            unsigned long _target_lvl_begin;
            unsigned long _target_lvl_end;
            
            unsigned long _range_all_addr;
            unsigned long _range_addr;
            unsigned long _range_nr_addr;
            
            unsigned long _no_collision_addr;
            
			bool _use_perin_convert_spam;
			unsigned long _perin_convert_spam_write_addr;
			unsigned long _perin_convert_spam_ecx;

			int reselect_after;

			unsigned long hp_key_code;

            std::vector<flyff::key> _keys;
            void *_h_select_thread;
			void * h_perin_converter;
        }; vars _vars;
        
        // no vars vars
        bool bo_set_range;
        void *hwnd_noti;

        void load(void *handle, unsigned long base_addr, unsigned long base_size);
		*/
	public:
        // constructors
		floral_flyff(void);
        floral_flyff(void *handle, unsigned long base_addr, unsigned long base_size);
        floral_flyff(unsigned long pid);
        
		// filling nested struct virtuals
		class ci_localPlayer: public c_localPlayer {
            // get
			void get_name(char *name);
			unsigned int get_money();
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
        
        void init_range();
        void init_perin_convert_spam();


        // miscs
        // set
        void set_perin_convert_spam(bool state);

        // get
        bool get_perin_convert_spam();

        // do
        void enable_perin_convert_spam(bool state);

		/*
		// varrs
        char *error_string;
		void *hwnd_tab;
        bool thread_uing;

		floral_flyff(void);
		floral_flyff(void *handle, unsigned long base_addr, unsigned long base_size);
		floral_flyff(unsigned long pid);
        
        bool run(bool run = true);
        void stop();

        void set_hwnd(void *hwnd);
        void *get_hwnd();
        
		
        
        void get_local_name(char *name);
		unsigned int get_local_money();
		

		void select(unsigned long target);
		unsigned long getSelect();
		unsigned long getMe();
		float getHyp(flyff::bot::targetInfo ti);
		flyff::bot::targetInfo getClosestTargetInView();
        void addUpdateAttackKey(unsigned char key, float priority, bool remove = false);
        bool getKey(flyff::key *k);
        void attack();

		bool is_target_pet(unsigned long target);

		int get_target_hp(unsigned long target);
		bool is_target_dead(unsigned long target);

        void set_target_lvls(int begin = -1, int end = -1);
        void get_target_lvls(int *begin, int *end);
        
		void teleport_to_target(unsigned long target);
		void save_location(unsigned char *loc = nullptr);
        void get_location(unsigned char *loc); // 12 bytes
		void teleport_to_saved_pos();
        double get_killed_count();
        void set_killed_count(double val);
        double get_kill_to_home();
        void set_kill_to_home(double val);
        
        void set_range(float f);
        
        void set_no_collision(bool state);
        bool get_no_collision();
        
		void set_perin_convert_spam(bool state);
		bool get_perin_convert_spam();
		void init_perin_convert_spam();
		void enable_perin_convert_spam(bool state);

		void set_reselect_after(int seconds);
		int get_reselect_after();

		void set_hp_key_code(unsigned long code);
		unsigned long get_hp_key_code();

        void set_hwnd_noti(void *hwnd);
        void *get_hwnd_noti();
		*/
};

#endif
