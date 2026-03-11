#pragma once

#include "messaging.h"
#include "atm_message.h"

class bank_machine {
public:
	bank_machine() :balance{199}
	{ }
	void done() {
		get_sender().send(messaging::close_queue{});
	}
	messaging::sender get_sender() {
		return incoming;
	}

	void run() {
		try
		{
			for (;;) {
				incoming.wait(
				).handle<verify_pin>(
					[&](verify_pin const& msg) {						
						if ("1937" == msg.pin)
							msg.atm_queue.send(pin_verified{});
						else
							msg.atm_queue.send(pin_incorrect{});
					}
				).handle<withdraw>(
					[&](withdraw const& msg) {
						if (balance >= msg.amount) {
							msg.atm_queue.send(withdraw_ok{});
							balance -= msg.amount;
						}
						else
						{
							msg.atm_queue.send(withdraw_denied{});
						}
					}
				).handle<get_balance>(
					[&](get_balance const& msg) {
						msg.atm_queue.send(::balance{ balance });
					}
				).handle<withdraw_processed>(
					[&](withdraw_processed const& msg) {

					}
				).handle<cancel_withdraw>(
					[&](cancel_withdraw const& msg) {

					}
				);
			}
		}
		catch (messaging::close_queue const&)
		{
		}
	}
private:
	messaging::receiver incoming;
	unsigned balance{};
};