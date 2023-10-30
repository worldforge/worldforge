#ifndef AVATAR_TEST
#define AVATAR_TEST

class Controller;

void testWield(Controller& ctl);
void testDeleteWielded(Controller& ctl);

void testHear(Controller& ctl);

void testLogoutRequest(Controller& ctl);
void testTransferRequest(Controller& ctl);
void testTransferRequestWithInvalidOp(Controller& ctl);


#endif
