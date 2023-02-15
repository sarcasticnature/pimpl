#include <memory>
#include <string>
#include <iostream>

#include "cli/cli.h"
#include "cli/clilocalsession.h"
#include "cli/loopscheduler.h"

int main()
{
    auto root_menu = std::make_unique<cli::Menu>("root");
    root_menu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, world\n"; },
            "Print hello world" );
    root_menu -> Insert(
            "hello_everysession",
            [](std::ostream&){ cli::Cli::cout() << "Hello, everybody" << std::endl; },
            "Print hello everybody on all open sessions" );
    root_menu -> Insert(
            "answer",
            [](std::ostream& out, int x){ out << "The answer is: " << x << "\n"; },
            "Print the answer to Life, the Universe and Everything " );
    root_menu -> Insert(
            "color",
            [](std::ostream& out){ out << "Colors ON\n"; cli::SetColor(); },
            "Enable colors in the cli" );
    root_menu -> Insert(
            "nocolor",
            [](std::ostream& out){ out << "Colors OFF\n"; cli::SetNoColor(); },
            "Disable colors in the cli" );

    auto subMenu = std::make_unique< cli::Menu >( "sub" );
    subMenu -> Insert(
            "hello",
            [](std::ostream& out){ out << "Hello, submenu world\n"; },
            "Print hello world in the submenu" );
    subMenu -> Insert(
            "demo",
            [](std::ostream& out){ out << "This is a sample!\n"; },
            "Print a demo string" );

    root_menu -> Insert( std::move(subMenu) );


    cli::Cli cli( std::move(root_menu) );

    cli::LoopScheduler scheduler;
    cli::CliLocalTerminalSession local_session(cli, scheduler, std::cout);
    scheduler.Run();
    return 0;
}
