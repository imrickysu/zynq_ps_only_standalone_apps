/*
 * awdt.h
 *
 *  Created on: Jun 15, 2016
 *      Author: rickys
 */

#ifndef AWDT_H_
#define AWDT_H_

int awdt_init(unsigned int seconds);
int awdt_kick_seconds(unsigned int seconds);
void awdt_status(void);
void awdt_reset_status_check(void);

#endif /* AWDT_H_ */
