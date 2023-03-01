#ifndef __GEENIE_FUNCTIONS_H__
#define __GEENIE_FUNCTIONS_H__
#include <ads1299.h>


class GEENIE{
    public:
        void initialize_ads();
        void set_buttons();
        byte read_ads();
    private:
        ADS1299 Ads1299;
        boolean verbose;		// turn on/off Serial feedback
        void reset_ads();
};

#endif