#pragma once

#include <string>
#include "messaging.h"

struct withdraw {
	std::string account;
	unsigned amount;
	mutable messaging::sender atm_queue;
	withdraw(std::string const& account_, unsigned amount_, messaging::sender atm_queue_)
		:account{ account_ }, amount{ amount_ }, atm_queue{ atm_queue_ }
	{
	}
};

struct withdraw_ok {

};

struct withdraw_denied {

};
struct cancel_withdraw {
	std::string account;
	unsigned amount;
	cancel_withdraw(std::string const& account_, unsigned amount_)
		: account{ account_ }, amount{ amount_ }
	{
	}
};

struct withdraw_processed {
	std::string account;
	unsigned amount;
	withdraw_processed(std::string const& account_, unsigned amount_)
		:account{ account_ }, amount{ amount_ }
	{
	}
};
struct card_inserted {
	std::string account;
	explicit card_inserted(std::string const& account_)
		:account{ account_ }
	{
	}
};
struct digit_presses {
	char digit;
	explicit digit_presses(char digit_)
		:digit{ digit_ }
	{
	}
};
struct clear_last_pressed {

};
struct eject_card {

};
struct withdraw_pressed {
	unsigned amount;
	explicit withdraw_pressed(unsigned amount_)
		:amount{ amount_ }
	{
	}
};
struct cancel_pressed {

};
struct issue_money {
	unsigned amount;
	issue_money(unsigned amount_)
		:amount{ amount_ }
	{
	}
};

struct verify_pin {
	std::string account;
	std::string pin;
	mutable messaging::sender atm_queue;
	verify_pin(std::string const& account_, std::string const& pin_,
		messaging::sender atm_queue_)
		: account{ account_ }, pin{ pin_ }, atm_queue{ atm_queue_ }
	{
	}
};
struct pin_verified {

};
struct pin_incorrect {

};
struct display_pin_incorrect {

};
struct display_enter_pin {

};
struct display_enter_card {

};
struct display_insufficient_funds {

};
struct display_withdraw_cancelled {

};
struct display_withdraw_options {

};
struct get_balance {
	std::string account;
	mutable messaging::sender atm_queue;
	get_balance(std::string const& account_, messaging::sender atm_queue_)
		:account{ account_ }, atm_queue{ atm_queue_ }
	{
	}
};

struct balance {
	unsigned amount;
	explicit balance(unsigned amount_)
		:amount{ amount_ }
	{
	}
};

struct display_balance {
	unsigned amount;
	explicit display_balance(unsigned amount_)
		:amount{ amount_ }
	{
	}
};

struct balance_pressed {

};