/*
 * Job.h
 *
 *  Created on: Jan 20, 2017
 *      Author: viki
 */

#ifndef JOB_H_
#define JOB_H_

class Job {
private:
	void *arg;
	void * (*func)(void *arg);
public:
	Job(void * (*func)(void *arg), void* arg);
	void execute();
	virtual ~Job();
};

#endif /* JOB_H_ */
