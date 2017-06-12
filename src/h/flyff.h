#ifndef FLYFF_H
#define FLYFF_H

class flyff {
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
            
            unsigned long _target_lvl_begin;
            unsigned long _target_lvl_end;
            
            unsigned long _range_all_addr;
            unsigned long _range_addr;
            unsigned long _range_nr_addr;
        }; vars _vars;
        
        

	public:
		flyff(void);
		flyff(void *handle, unsigned long base_addr, unsigned long base_size);

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

		void select(unsigned long target);
		unsigned long getSelect();
		unsigned long getMe();
		float getHyp(targetInfo ti);
		targetInfo getClosestTargetInView();

        void set_target_lvls(int begin = -1, int end = -1);
        void get_target_lvls(int *begin, int *end);
        
		void teleport_to_target(unsigned long target);
		void save_location();
		void teleport_to_saved_pos();
        
        void set_range(float f);
};

#endif
