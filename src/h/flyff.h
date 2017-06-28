#ifndef FLYFF_H
#define FLYFF_H

#include <vector>

class flyff {
    public:
        struct key {
            unsigned char code;
            float priority;
        };
    
	private:
        // variables
        struct vars {
            void *_hwnd;
            void *_handle;
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
            
            std::vector<key> _keys;
            void *_h_select_thread;
        }; vars _vars;
        
        // no vars vars
        bool bo_set_range = false;
        
        void load(void *handle, unsigned long base_addr, unsigned long base_size);

	public:
        char *error_string;
        
		flyff(void);
		flyff(void *handle, unsigned long base_addr, unsigned long base_size);
        flyff(unsigned long pid);
        
        bool run(bool run = true);
        void stop();

        void set_hwnd(void *hwnd);
        void *get_hwnd();
        
		struct targetInfo {
			unsigned long base;	
			float x, y, z;
			float hyp;
			unsigned long lvl;
			unsigned long hp;
			char name[255];
		};
        
        void get_local_name(char *name);

		void select(unsigned long target);
		unsigned long getSelect();
		unsigned long getMe();
		float getHyp(targetInfo ti);
		targetInfo getClosestTargetInView();
        void addUpdateAttackKey(unsigned char key, float priority, bool remove = false);
        bool getKey(key *k);
        void attack();

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
};

#endif
