#pragma once


/****************************ISO 14443A*****************************/
#define MIFARE_UL 0x01
#define MIFARE_DF 0x02
#define MIFARE_CL 0x03
#define MAX_APPS  0x1C
#define MAX_FILES 0x10
#define MAX_KEYS 0x0F
#define USN_SIZE 0x07
#define PBN_SIZE 0x05
#define AID_SIZE 0x03
#define KEY_SIZE 0x10

//14443A structures
const struct DF_hardware_info{
	unsigned char vendor_ID;
	unsigned char type;
	unsigned char sub_type;
	unsigned char major_version;
	unsigned char minor_version;
	unsigned char storage_size;
	unsigned char comm_protocol;
};

const struct DF_software_info{
	unsigned char vendor_ID;
	unsigned char type;
	unsigned char sub_type;
	unsigned char major_version;
	unsigned char minor_version;
	unsigned char storage_size;
	unsigned char comm_protocol;
};

const struct DF_creation_info{
	unsigned char unique_serial_number[USN_SIZE];
	unsigned char production_batch_number[PBN_SIZE];
	unsigned char calendar_week;
	unsigned char year;
};

const struct DESFire_tag{
    DF_hardware_info hardware_info;
    DF_software_info software_info;
    DF_creation_info creation_info;
	unsigned char last_error;
};

const struct ULite_tag{
	unsigned char security_buff[16];
};

const struct tag_14443A{
	unsigned char type;
	unsigned char tag_id[10];
	unsigned char read_buff[4000];
	DESFire_tag DESFire;
	ULite_tag ULite;
};

#ifdef C1LIB_EXPORTS
	#define C1LIB_API extern "C" __declspec(dllexport)
#elif STATICBUILD
	#define C1LIB_API
#else
	#define C1LIB_API extern "C" __declspec(dllimport)
#endif

//General Select Functions
C1LIB_API tag_14443A new_14443A(void);
C1LIB_API int get_14443A(tag_14443A *tag);
C1LIB_API int rats_14443A();
C1LIB_API int pps_14443A();
//ULite Functions
C1LIB_API int UL_read(tag_14443A *tag, int page);
C1LIB_API int UL_read_multi(tag_14443A *tag, int page, int page_no);
C1LIB_API int UL_halt();
C1LIB_API int UL_write(tag_14443A *tag, int page, unsigned char *data);
C1LIB_API int UL_write_multi(tag_14443A *tag, int page, int page_no, unsigned char *data);
C1LIB_API int UL_write_otp(tag_14443A *tag, unsigned char *data);
C1LIB_API int UL_lock(tag_14443A *tag, int page);
C1LIB_API int UL_lock_otp(tag_14443A *tag);
C1LIB_API int UL_block_locking_otp(tag_14443A *tag);
C1LIB_API int UL_block_locking_4_9(tag_14443A *tag);
C1LIB_API int UL_block_locking_10_15(tag_14443A *tag);
C1LIB_API int UL_read_security(tag_14443A *tag);
//DESFire Functions
C1LIB_API int DF_authenticate(tag_14443A *my_tag, unsigned char key_no, unsigned char *key);
C1LIB_API int DF_create_app(tag_14443A *my_tag, unsigned char key_settings, unsigned char num_keys, unsigned char *_AID);
C1LIB_API int DF_get_key_settings(tag_14443A *my_tag);
C1LIB_API int DF_select_application(tag_14443A *my_tag, unsigned char *_AID);
C1LIB_API int DF_change_key(tag_14443A *my_tag, unsigned char key_number, unsigned char *_current_key, unsigned char *_new_key);
C1LIB_API int DF_get_key_version(tag_14443A *my_tag, unsigned char key_number);
C1LIB_API int DF_format_PICC(tag_14443A *my_tag);
C1LIB_API int DF_get_file_IDs(tag_14443A *my_tag, int *num_of_files);
C1LIB_API int DF_delete_application(tag_14443A *my_tag, unsigned char *_AID);
C1LIB_API int DF_get_AIDs(tag_14443A *my_tag, int *AID_count);
C1LIB_API int DF_change_key_settings(tag_14443A *my_tag, unsigned char key_settings);
C1LIB_API int DF_create_std_data_file(tag_14443A *my_tag, unsigned char file_number,unsigned char communication_settings, unsigned char *_access_rights, unsigned long _file_size);
C1LIB_API int DF_create_backup_data_file(tag_14443A *my_tag, unsigned char file_number,unsigned char communication_settings, unsigned char *_access_rights, unsigned long _file_size);
C1LIB_API int DF_create_value_file(tag_14443A *my_tag, unsigned char file_number, unsigned char communication_settings, unsigned char *_access_rights, signed long _lower_limit, signed long _upper_limit, signed long _value, unsigned char lim_credit_enabled);
C1LIB_API int DF_create_linear_record_file(tag_14443A *my_tag, unsigned char file_number, unsigned char communication_settings, unsigned char *_access_rights, unsigned long _record_size, unsigned long _max_records);
C1LIB_API int DF_create_cyclic_record_file(tag_14443A *my_tag, unsigned char file_number, unsigned char communication_settings, unsigned char *_access_rights, unsigned long _record_size, unsigned long _max_records);
C1LIB_API int DF_delete_file(tag_14443A *my_tag, unsigned char file_number);
C1LIB_API int DF_write_data(tag_14443A *my_tag, unsigned char file_number,unsigned long _offset,unsigned long _length,unsigned char *_data);
C1LIB_API int DF_write_record(tag_14443A *my_tag, unsigned char file_number,unsigned long _offset,unsigned long _length,unsigned char *_data);
C1LIB_API int DF_get_value(tag_14443A *my_tag, unsigned char file_number);
C1LIB_API int DF_clear_record_file(tag_14443A *my_tag, unsigned char file_number);
C1LIB_API int DF_credit(tag_14443A *my_tag, unsigned char file_number, unsigned long _value);
C1LIB_API int DF_debit(tag_14443A *my_tag, unsigned char file_number, unsigned long _value);
C1LIB_API int DF_limited_credit(tag_14443A *my_tag, unsigned char file_number, unsigned long _value);
C1LIB_API int DF_commit_transaction(tag_14443A *my_tag);
C1LIB_API int DF_abort_transaction(tag_14443A *my_tag);
C1LIB_API int DF_read_data(tag_14443A *my_tag, unsigned char file_number,unsigned long _offset,unsigned long _length);
C1LIB_API int DF_read_records(tag_14443A *my_tag, unsigned char file_number,unsigned long _offset,unsigned long _length);
C1LIB_API int DF_change_file_settings(tag_14443A *my_tag, unsigned char file_number, unsigned char communication_settings, unsigned char *access_rights);
C1LIB_API int DF_get_file_settings(tag_14443A *my_tag, unsigned char file_number);
C1LIB_API int DF_get_version(tag_14443A *my_tag);
/*****************************End ISO 14443A******************************/


/*********************************ISO 15693******************************/
#define TEMPSENS	1
#define VARIOSENS	2
#define	HEI			3
#define TAG_IT_HF	4

const struct tag_15693{
	bool program_option;
	bool read_security;
	bool selected;          
	bool addressed;
	unsigned char sys_info_support;
	unsigned int command_support;
	unsigned char DSFID;
	unsigned char AFI;
	unsigned char bytes_per_block;
	unsigned char blocks;
	unsigned char ic_ref;
	unsigned char tag_id[10];
	int id_length;
	unsigned char read_buff[256];
	unsigned char security_buff[256];
	unsigned char tag_type;
};

C1LIB_API tag_15693 new_15693(void);
C1LIB_API int get_15693(tag_15693 *tag, unsigned char AFI);
C1LIB_API int stay_quite_15693(tag_15693 *tag);
C1LIB_API int select_15693(tag_15693 *tag);
C1LIB_API int read_single_15693(tag_15693 *tag, int block);
C1LIB_API int read_multi_15693(tag_15693 *tag, int block, int block_no);
C1LIB_API int write_single_15693(tag_15693 *tag, int block, unsigned char *data);
C1LIB_API int write_multi_15693(tag_15693 *tag, int block, int block_no, unsigned char *data);
C1LIB_API int lock_15693(tag_15693 *tag, int block);
C1LIB_API int reset_15693(tag_15693 *tag);
C1LIB_API int write_afi_15693(tag_15693 *tag, unsigned char afi);
C1LIB_API int lock_afi_15693(tag_15693 *tag);
C1LIB_API int write_dsfid_15693(tag_15693 *tag, unsigned char afi);
C1LIB_API int lock_dsfid_15693(tag_15693 *tag);
C1LIB_API int read_sys_info_15693(tag_15693 *tag);
C1LIB_API int read_security_15693(tag_15693 *tag, int block, int block_no);

//Tempsens tags
#define LARGE			0x00
#define MEDIUM			0x01
#define MEDIUM_SMALL	0x02
#define SMALL			0x03

const struct tempsens_log{
	//log settings
	bool time_only_mode;
	bool no_temp_measurement;
	bool cyclical_storage;
	unsigned char storage_size;
	bool battery_test;
	bool start_log;
	bool overflow;     //feedback
	bool battery_fail; //feedback

	//status settings
	unsigned long period;      //seconds ..  up to 57,000
	float low_temp_limit;  //degree C
	float high_temp_limit; //degree C
	unsigned int ticks;	      //feedback
	unsigned int time_since_last_measurement; //feedback
	unsigned char next_measurement_block;     //set & feedback
	unsigned int cal_val2;                    //feedback
	unsigned int cal_val1;                    //feedback
	unsigned char password[6];
	unsigned long start_time;
	bool use_start_time;
	unsigned long current_time;
	unsigned long time[64];
	float temperature[64]; //degree C
};


C1LIB_API tempsens_log new_tempsens_log();
C1LIB_API int TS_set_passive(tag_15693 *tag);
C1LIB_API int TS_clear(tag_15693 *tag);
C1LIB_API int TS_lock(tag_15693 *tag, tempsens_log *log);
C1LIB_API int TS_unlock(tag_15693 *tag, tempsens_log *log);
C1LIB_API int TS_set_log(tag_15693 *tag, tempsens_log *log);
C1LIB_API int TS_get_log_info(tag_15693 *tag, tempsens_log *log);
C1LIB_API int TS_get_log_data(tag_15693 *tag, tempsens_log *log, int block, int block_no);
C1LIB_API int TS_get_password(tag_15693 *tag, tempsens_log *log);
C1LIB_API int TS_set_password(tag_15693 *tag, tempsens_log *log);


// Variosens tags
struct tempViolationData
{
	unsigned char logMode;

	// Log Mode 0
	//	temperature[0] - internal temperature
	//  temperature[1] - external temperature
	//  logCounter - Log Counter

	// Log Mode 1
	//	temperature[0] - Measurement 1
	//  temperature[1] - Measurement 2
	//  temperature[2] - Measurement 3

	// Log Mode 2
	//	temperature[0] - Measurement outside limits
	//  logCounter - Log Counter

	// Log Mode 3
	//	temperature[0] - First time measurement is outside or inside limits or extremum value
	//  logCounter - Log Counter

	float temperature[3];

	// the time that the violation occurred
	unsigned int vltionTime;
};


#define VIOLATIONDATALNTH	0xEF
const struct variosens_log
{
	//log settings
	// memory block 0x05
	int calTemp1;
	int calTemp2;
	int calTicks1;

	// memory block 0x06
	unsigned int calVoltageLo;
	unsigned int voltageTh;
	unsigned int calVoltageHi;
	int calTicks2;

	// memory block 0x08
	unsigned int UTCStartTime;

	// memory block 0x09
	unsigned short stndByTime;
	unsigned short logIntval;

	// memory block 0x0A
	unsigned char logMode;
	unsigned char batCheck;
	float upperTemp;
	float lowerTemp;
	unsigned char storageMode;
	unsigned char sensorFlag;

	// memory block 0x0B
	unsigned int	nextBlockAdd;
	unsigned int	numMemRpl;
	bool			cardStopped;
	bool			cardExternal;
	unsigned int	numMeasure;

	unsigned int	numDownloadMeas;

	unsigned char	PWLevel;
	unsigned int	password;

	tempViolationData	vltionData[VIOLATIONDATALNTH];
};


C1LIB_API variosens_log new_variosens_log();
C1LIB_API int VS_init(tag_15693 *tag);
C1LIB_API int VS_setLogMode(tag_15693 *tag, variosens_log *pVLog);
C1LIB_API int VS_setLogTimer(tag_15693 *tag, variosens_log *pVLog);
C1LIB_API int VS_startLog(tag_15693 *tag, unsigned int UTCTime);
C1LIB_API int VS_getLogState(tag_15693 *tag, variosens_log *pVLog);
C1LIB_API int VS_set_passive(tag_15693 *tag);
C1LIB_API int VS_getTimerState(tag_15693 *tag, unsigned int &elapsedTime, unsigned int &batVoltageLvl, bool &timerRun);
C1LIB_API int VS_verifyPW(tag_15693 *tag, variosens_log *pVLog);
C1LIB_API int VS_setPW(tag_15693 *tag, variosens_log *pVLog);
C1LIB_API int VS_getLogData(tag_15693 *pTag, variosens_log *pVLog);

/*********************************End ISO 15693******************************/


/*********************************ISO 18000******************************/
const struct tag_18000{
	unsigned char data[256];
	int data_length;
};

C1LIB_API tag_18000 new_18000(void);
C1LIB_API int get_18000(tag_18000 *tagstruct);
/*********************************End ISO 18000******************************/


/*********************************ISO 14443B******************************/
const struct tag_14443B{
	unsigned char tag_id[20];
	int id_length;
	unsigned char data[256];
};

C1LIB_API tag_14443B new_14443B(void);
C1LIB_API int get_14443B_SRI(tag_14443B *tagstruct);
C1LIB_API int get_14443B_SR176(tag_14443B *tagstruct);
C1LIB_API int get_14443B(tag_14443B *tagstruct);
/*********************************End ISO 14443B******************************/

/*********************************FeliCa******************************/
const struct tag_FeliCa{
	unsigned char tag_id[20];
	int id_length;
	unsigned char data[256];
};

C1LIB_API tag_FeliCa new_FeliCa(void);
C1LIB_API int get_FeliCa(tag_FeliCa *tagstruct);
/*********************************End FeliCa******************************/


/********************************PICO READ*************************/
#define AUTHENTICATE	0x01
#define	NO_AUTH			0x00
#define DEBIT			0x01
#define CREDIT			0x00

C1LIB_API int PR_select_tag(unsigned char *tagID, bool authenticate, bool key_type);
C1LIB_API int PR_select_page(unsigned char *page_conf, bool select_pg, bool authenticate, unsigned char reader_key, bool book, unsigned char page_number);
C1LIB_API int PR_select_current_key(unsigned char key_no);
C1LIB_API int PR_diversify_key(unsigned char key_no, unsigned char *tagID);
C1LIB_API int PR_load_key(int key_no, unsigned char *new_pico_key, unsigned char *exchange_key);
C1LIB_API int PR_delete_key(int key_no);
C1LIB_API int PR_deactivate_key(int key_no);
C1LIB_API int PR_send_recieve(unsigned char *cmd_in, unsigned char *data_in, unsigned char *data_out, bool in, unsigned char *sbyte1, unsigned char *sbyte2);
/********************************End PICO READ*************************/


/********************************INSIDE Sirit Tags*************************/
#define IC_15693		0x01
#define IC_14443B		0x02
#define ISO_14443B3		0x03
#define PROTO_BOTH		0x01
#define PROTO_15693		0x02
#define	PROTO_14443B	0x00

const struct secure_obj{
	unsigned char app_limit;
	bool auth15693;
	bool key_access;
	bool locked_keys;
	bool read_secure;
	unsigned int epurse;
};

const struct locked_obj{
	bool all;
	bool block_12;
	bool block_11;
	bool block_10;
	bool block_9;
	bool block_8;
	bool block_7;
	bool block_6;
};

const struct tag_IC{
	unsigned char tag_id[10];
	unsigned char read_buff[2000];
	bool secure;
	bool auth;
	bool key_type;
	unsigned char key_num;
	unsigned char pages;
	unsigned char page_size;
	unsigned char books;
	unsigned char proto;
	bool isoB3;
	unsigned int otp;
	bool eas;
	bool app_mode;
	unsigned char AID[8];
	locked_obj locked;
	secure_obj secure_area;
};

C1LIB_API tag_IC new_IC(void);
C1LIB_API int get_IC(tag_IC *tagstruct, unsigned char proto, bool auth, bool key_type);
C1LIB_API int IC_select_page(tag_IC *tag, bool auth, unsigned char reader_key, bool book, unsigned char page_number);
C1LIB_API int IC_authenticate(tag_IC *tag, unsigned char reader_key);
C1LIB_API int IC_read_block(tag_IC *tag, int block_start, int block_count);
C1LIB_API int IC_write_block(tag_IC *tag, int block_start, int block_count, unsigned char *_data_in);
C1LIB_API int IC_decrease_counter(tag_IC *tag, unsigned int decrease_amt);
C1LIB_API int IC_increase_counter(tag_IC *tag, unsigned int new_value);
C1LIB_API int IC_write_credit_key(tag_IC *tag, unsigned char *key);
C1LIB_API int IC_write_debit_key(tag_IC *tag, unsigned char *key);
C1LIB_API int IC_get_page_info(tag_IC *tag);
C1LIB_API int IC_lock_all(tag_IC *tag);
C1LIB_API int IC_lock_block(tag_IC *tag, unsigned char block);
C1LIB_API int IC_write_otp(tag_IC *tag, unsigned int otp);
C1LIB_API int IC_set_app_limit(tag_IC *tag, unsigned char block);
C1LIB_API int IC_set_eas(tag_IC *tag);
C1LIB_API int IC_mem_config_2K_pages(tag_IC *tag);
C1LIB_API int IC_mem_config_1_book(tag_IC *tag);
C1LIB_API int IC_mem_config_non_secure(tag_IC *tag);
C1LIB_API int IC_set_isoB3(tag_IC *tag);
C1LIB_API int IC_set_no_auth_15693(tag_IC *tag);
C1LIB_API int IC_set_no_key_access(tag_IC *tag);
C1LIB_API int IC_set_iso_coding(tag_IC *tag, unsigned char proto);
C1LIB_API int IC_set_lock_keys(tag_IC *tag);
C1LIB_API int IC_set_app_mode(tag_IC *tag);
/********************************End INSIDE Sirit Tags*************************/


/********************************Sirit Reader*************************/
#define LONG_WAIT	1
#define SHORT_WAIT	0

#define IC_SEND_SIG 1

C1LIB_API int C1_open_comm();
C1LIB_API int C1_close_comm();
C1LIB_API int C1_set_comm();
C1LIB_API int C1_disable();
C1LIB_API int C1_enable();
C1LIB_API int C1_config_14443A();
C1LIB_API int C1_config_14443A_4();
C1LIB_API int C1_config_15693();
C1LIB_API int C1_config_18000();
C1LIB_API int C1_config_IC_14443B();
C1LIB_API int C1_config_IC_15693();
C1LIB_API int C1_config_14443B3();
C1LIB_API int C1_config_FeliCa();
C1LIB_API int C1_config_FeliCa_old();
C1LIB_API unsigned int C1_transmit(unsigned char *in, int in_length, unsigned char *out, int out_length, bool wait, bool send_sig);
/********************************End Sirit Reader*************************/
