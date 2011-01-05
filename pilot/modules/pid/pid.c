#include "pid.h"
#include "../util/convert.h"

vector_t _kp;
vector_t _ki;
vector_t _kd;

typedef struct err {
    double i;
    double d;
} pid_err_t;

pid_err_t _err_x;
pid_err_t _err_y;
pid_err_t _err_z;

void pid_init() {
    // TODO get tuning values K from EEPROM
	_kp.x = 0.7;
	_kp.y = 0.7;
	_kp.z = 0.7;
	_ki.x = 0.001;
	_ki.y = 0.001;
	_ki.z = 0.001;
	_kd.x = 0.0;
	_kd.y = 0.0;
	_kd.z = 0.0;
}

void pid_send_tuning() {
	uint8_t length = 36;
	uint8_t buf[length];
	
	convert_double_to_bytes(_kp.x, buf, 0);
	convert_double_to_bytes(_kp.y, buf, 4);
	convert_double_to_bytes(_kp.z, buf, 8);
	convert_double_to_bytes(_ki.x, buf, 12);
	convert_double_to_bytes(_ki.y, buf, 16);
	convert_double_to_bytes(_ki.z, buf, 20);
	convert_double_to_bytes(_kd.x, buf, 24);
	convert_double_to_bytes(_kd.y, buf, 28);
	convert_double_to_bytes(_kd.z, buf, 32);

	protocol_send_message('p', buf, length);
}

void pid_receive_tuning(uint8_t *buf) {
	_kp.x = convert_bytes_to_double(buf, 0);
	_kp.y = convert_bytes_to_double(buf, 4);
	_kp.z = convert_bytes_to_double(buf, 8);
	_ki.x = convert_bytes_to_double(buf, 12);
	_ki.y = convert_bytes_to_double(buf, 16);
	_ki.z = convert_bytes_to_double(buf, 20);
	_kd.x = convert_bytes_to_double(buf, 24);
	_kd.y = convert_bytes_to_double(buf, 28);
	_kd.z = convert_bytes_to_double(buf, 32);
}

double _pid_mv(double sp, double pv, double kp, double ki, double kd, pid_err_t *err){
    double e = sp - pv;
    double mv = (kp * e) + (ki * err->i) + (kd * (pv - err->d));

    err->i += e;
    err->d = e;
    
    return mv;
}

vector_t pid_mv(vector_t sp, vector_t pv) {
    vector_t mv;

	mv.x = _pid_mv(sp.x, pv.x, _kp.x, _ki.x, _kd.x, &_err_x);
	mv.y = _pid_mv(sp.y, pv.y, _kp.y, _ki.y, _kd.y, &_err_y);
	mv.z = _pid_mv(sp.z, pv.z, _kp.z, _ki.z, _kd.z, &_err_z);

    return mv;
}

