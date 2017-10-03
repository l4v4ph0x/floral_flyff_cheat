#ifndef FLYFF_H
#define FLYFF_H

#include <vector>

class flyff {
	public:
		struct key {
			unsigned char code;
			float priority;

            // contructors
            key() {
            
            }

            key(unsigned char code, float priority) {
                this->code = code;
                this->priority = priority;
            }
		};

        // vars >
        char *error_string;

        // < end of vars

	protected:
		// local char class
        struct c_localPlayer {
            // vars it uses >
			void *handle;

            unsigned long select_addr;
            unsigned long me_addr;
            unsigned long no_collision_addr;

            unsigned char saved_pos[12];
            // < end of vars it uses

            // get
			virtual void get_name(char *name) = 0;
			virtual unsigned int get_money() = 0;
			virtual unsigned long get_me() = 0;
			virtual unsigned long get_select() = 0;
			virtual void get_location(unsigned char *loc) = 0; // 12 bytes
            virtual bool get_no_collision() = 0;

            // set
			virtual void save_location(unsigned char *loc = nullptr) = 0;
            virtual void set_no_collision(bool state) = 0;

            // do
			virtual void teleport_to_saved_pos() = 0;
		};

		// bot functions herro
        struct c_bot {
			struct targetInfo {
				unsigned long base;
				float x, y, z;
				float hyp;
				unsigned long lvl;
				unsigned long hp;
				char name[255];
			};

            // vars >
            std::vector<key> keys;
            int reselect_after;
            double kill_to_home;
            int target_lvl_begin, target_lvl_end;
            // < vars

            // get
			virtual targetInfo get_closest_target_in_view() = 0;
			virtual bool get_key(flyff::key *k) = 0;
            virtual int get_reselect_after() = 0;
            double get_kill_to_home() { return kill_to_home; };
            void set_target_lvls(int begin, int end) {
                if (begin != -1) target_lvl_begin = begin;
                if (end != -1) target_lvl_end = end;
            };

            // set
			virtual void add_update_attack_key(key k, bool remove = false) = 0;
            virtual void set_reselect_after(int seconds) = 0;
            void set_kill_to_home(double val) { kill_to_home = val; };
            void get_target_lvls(int *begin, int *end) {
                *begin = target_lvl_begin;
                *end = target_lvl_end;
            };

            // do
			virtual void teleport_to_target(targetInfo target) = 0;
			virtual void select(unsigned long target) = 0;
			virtual void attack() = 0;            
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
        

	public:
        // miscs
        virtual void init_perin_convert_spam() = 0;

        // set
        virtual void set_perin_convert_spam(bool state) = 0;

        // get
        virtual bool get_perin_convert_spam() = 0;

        // do
        virtual void enable_perin_convert_spam(bool state) = 0;

        

		c_localPlayer *localPlayer;
		c_bot *bot;
        c_ui *ui = new c_ui();
};


#endif