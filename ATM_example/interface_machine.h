#pragma once

#include "messaging.h"
#include "atm_message.h"

#include <mutex>
#include <iostream>

extern std::mutex iom;

class interface_machine {
public:
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
				incoming.wait().handle<issue_money>(
					[&](issue_money const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Issuing " << msg.amount << "\n";
						}
					}
				).handle<display_insufficient_funds>(
					[&](display_insufficient_funds const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Insufficient funds\n";
						}
					}
				).handle<display_enter_pin>(
					[&](display_enter_pin const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Please enter your PIN(0-9)\n";
						}
					}
				).handle<display_enter_card>(
					[&](display_enter_card const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Please enter your card(I)\n";
						}
					}
				).handle<display_balance>(
					[&](display_balance const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "The balance of your account is: " << msg.amount << "\n";
						}
					}
				).handle<display_withdraw_options>(
					[&](display_withdraw_options const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Withdraw 50?(w)\n";
							std::cout << "Display Balance(b)\n";
							std::cout << "Cancel?(c)\n";
						}
					}
				).handle<display_withdraw_cancelled>(
					[&](display_withdraw_cancelled const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Withdraw cancelled\n";
						}
					}
				).handle<display_pin_incorrect>(
					[&](display_pin_incorrect const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "PIN incorrect\n";
						}
					}
				).handle<eject_card>(
					[&](eject_card const& msg) {
						{
							std::lock_guard lk(iom);
							std::cout << "Ejecting card\n";
						}
					}
				);
			}
		}
		catch (messaging::close_queue const&)
		{

			std::lock_guard lk(iom);
			std::cout << "interface machine close_queue\n";
		}
	}
private:
	messaging::receiver incoming;
};