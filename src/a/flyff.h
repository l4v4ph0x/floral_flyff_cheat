#ifndef FLYFF_H
#define FLYFF_H

#include <vector>


class flyff {
	public:
		struct key {
			unsigned char code;
			float priority;
			bool isSet = false;

            // contructors
            key() {
            
            }

            key(unsigned char code, float priority) {
                this->code = code;
                this->priority = priority;
				isSet = true;
            }
		};

        struct targetInfo {
            unsigned long base;
            float x, y, z;
            float hyp;
            unsigned long lvl;
            unsigned long hp;
            char name[255];
        };

        // vars >
        char *error_string;

        // < end of vars

	protected:
		// local char class
        struct c_localPlayer {
            // vars it uses >
            flyff *parent;
			void *handle;

            unsigned long select_addr;
            unsigned long me_addr;
            unsigned long no_collision_addr;
            unsigned long range_nr_addr;
            unsigned long anti_mem_select_addr;

            unsigned char saved_pos[12];

            float max_range = 4;
            // < end of vars it uses

            // get
			virtual char *get_name() = 0;
			virtual unsigned int get_money() = 0;
			virtual unsigned int get_hp() = 0;
			virtual unsigned long get_me() = 0;
			virtual unsigned long get_select() = 0;
			virtual void get_location(unsigned char *loc) = 0; // 12 bytes
            virtual bool get_no_collision() = 0;
            float get_max_range() { return max_range; };

            // set
			virtual void save_location(unsigned char *loc = nullptr) = 0;
            virtual void set_no_collision(bool state) = 0;
            virtual float set_range(float f) = 0;

            // do
			virtual void teleport_to_saved_pos() = 0;
            virtual void teleport_to_target(targetInfo target) = 0;
            virtual void select(unsigned long target) = 0;
            virtual void attack() = 0;
		};

		// bot functions herro
        struct c_bot {
            // vars >
            flyff *parent;
            void *handle;

            unsigned long maxInView_addr;
            unsigned long targetBase_addr;

            std::vector<key> keys;

            int reselect_after;
            int target_lvl_begin, target_lvl_end;

            void *h_select_thread;
            bool thread_uing;
            double killed_count;
            double kill_to_home;

			unsigned long bad_target;
            // < vars

            // get
			virtual targetInfo get_closest_target_in_view() = 0;
			virtual bool get_key(key *k) = 0;
            int get_reselect_after() { return reselect_after; };
            double get_killed_count() { return killed_count; };
            double get_kill_to_home() { return kill_to_home; };
            /**
            * if one of arguments are set to -1 then not writing this one
            **/
            void set_target_lvls(int begin = -1, int end = -1) {
                if (begin != -1) target_lvl_begin = begin;
                if (end != -1) target_lvl_end = end;
            };
            bool get_run() {
                return !(h_select_thread == nullptr);
            };

            // set
			virtual void add_update_attack_key(key k, bool remove = false) = 0;
            virtual void set_reselect_after(int seconds) { reselect_after = seconds; };
            void set_kill_to_home(double val) { kill_to_home = val; };
            void get_target_lvls(int *begin, int *end) {
                *begin = target_lvl_begin;
                *end = target_lvl_end;
            };

            // do
            virtual bool run() = 0;
            virtual void stop() = 0;
		};

        struct c_ui {
            private:
                void *hwnd;
                void *noti_hwnd;

            public:
                // get
                void *get_hwnd_noti() { return noti_hwnd; };
                void *get_hwnd() { return hwnd; };

                // set
                void set_hwnd_noti(void *hwnd) { noti_hwnd = hwnd; };
                void set_hwnd(void *hwnd) { this->hwnd = hwnd; };
                
        };
        
		struct c_buff {
			flyff *parent;

			void *h_hper_thread;
			bool thread_uing;

			private:
				int hp_to_buff = 0;
				key hp_key;

			public:
				// get
				bool get_hp_key(key *k) {
					*k = hp_key;
					if (!hp_key.isSet) return false;
					return true;
				};
				int get_hp_to_buff() { return hp_to_buff; };
				virtual bool get_run() = 0;

				// set
				void set_hp_key(key k) {
					hp_key = k; 
				};
				void set_hp_to_buff(int hp) { hp_to_buff = hp; }

				// do
				virtual void run(bool state) = 0;
		};
	public:
        // miscs
        // set
        virtual void set_perin_convert_spam(bool state) = 0;

        // get
        virtual bool get_perin_convert_spam() = 0;

        // do
        virtual void enable_perin_convert_spam(bool state) = 0;

        

		c_localPlayer *localPlayer;
		c_bot *bot;
        c_ui *ui = new c_ui();
		c_buff *buff;
};


#endif