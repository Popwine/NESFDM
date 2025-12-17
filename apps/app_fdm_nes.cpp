#include "NESSolver.h"
#include "CLI11.hpp"
#include <optional>

struct Arguments{
    std::optional<double> initialAStar;
    // tao = f_n * t
    std::optional<double> totalTao;
    std::optional<double> resultCalcStartTao;
	std::optional<double> taoStepSize;

	std::optional<double> fNatural;
	std::optional<double> fDesign;

	std::optional<double> UStar;

	std::optional<std::string> outputFile;
	std::optional<std::string> config;		// single(default) 3m3u
	std::optional<std::string> objFunc;	// avg max avg_max(default)
	std::optional<std::string> sweepParamsFile;
	std::optional<bool> sweep;
	

	std::optional<int> nesNum;
	std::vector<std::optional<double>> mr = std::vector<std::optional<double>>(9);
    std::vector<std::optional<double>> kr = std::vector<std::optional<double>>(9);
    std::vector<std::optional<double>> cr = std::vector<std::optional<double>>(9);

};

void parseArguments(int argc, char* argv[], Arguments& arg){
	CLI::App app{"Force Decomposition Model with NES"};

    
    app.add_option("-a,--initial-a-star", arg.initialAStar, "Initial AStar");
    app.add_option("--rctao", arg.resultCalcStartTao, "Result Calculation Start Time");
	app.add_option("--ctao", arg.totalTao, "Total Calculation Tao");
	app.add_option("--dtao", arg.taoStepSize, "Tao Step Size");

	app.add_option("--fn", arg.fNatural, "Natural Frequency");
	app.add_option("--fd", arg.fDesign, "Design Frequency");

	app.add_option("--ustar", arg.UStar, "Reduced Wind Velocity");

	app.add_option("-n,--nes-number", arg.nesNum, "NES Number (0 ~ 9)");

	app.add_option("--out", arg.outputFile, "State Time History Output File Path");
	app.add_option("--config", arg.config, 
		"Config for Single Calculation: \n\
		single: use indicated params(Ustar, fn); \n\
		3m3u: calculate 3 modes and 3 UStars using built-in params(Ustar, fn)");
	app.add_option("-j,--objective-funtion", arg.objFunc, "\
		Objective Function. Only avaliable when config is 3m3u.\n\
		avg, average, max, max_avg\
		");
	app.add_option("--sweep-params", arg.sweepParamsFile, "Sweep Parameters File Path");

	app.add_flag("-s,--sweep", arg.sweep, "Sweep flag");
	
	
	for(int i = 1; i <= 9; i++){
		std::string cmd = "--mr" + std::to_string(i);
		std::string help = "Mass ratio for NES" + std::to_string(i);
		app.add_option(cmd, arg.mr[i-1], help);

		cmd = "--kr" + std::to_string(i);
		help = "Stiffness ratio for NES" + std::to_string(i);
		app.add_option(cmd, arg.kr[i-1], help);

		cmd = "--cr" + std::to_string(i);
		help = "Damping ratio for NES" + std::to_string(i);
		app.add_option(cmd, arg.cr[i-1], help);
	}
	
	try {
        app.parse(argc, argv);
    } catch (const CLI::ParseError &e) {
        // app.exit(e) 会处理帮助信息的打印，或者错误信息的打印
        // 并在 -h 时返回 0，在错误时返回非 0
        std::exit(app.exit(e));
    }
}
void checkArgValidation(Arguments& arg){
	if(!arg.nesNum.has_value()){
		throw std::runtime_error("NES number is required. Use \"-n\" to indecate.");
	}
	if(!arg.initialAStar.has_value()){arg.initialAStar = 0.06;}
	if(!arg.totalTao.has_value()){arg.totalTao = 500;}
	if(!arg.resultCalcStartTao.has_value()){arg.resultCalcStartTao = 250;}
	if(!arg.taoStepSize.has_value()){arg.taoStepSize = 0.001;}
	//if(!arg.fNatural.has_value()){arg.fNatural = 0.06;}在不同的config下需单独处理
	//if(!arg.UStar.has_value()){arg.UStar = 1.117;}在不同的config下需单独处理
	if(!arg.fDesign.has_value()){arg.fDesign = 1.117;}

	
	if((!arg.config.has_value())){arg.config = "single";}


	// 非扫描的情况：
	if((!arg.sweep.has_value()) || arg.sweep == false){
		
		// 检验nes参数是否全面
		std::string missingMessage;
		for(int i = 1; i <= arg.nesNum; i++){
			if(!arg.mr[i-1].has_value()){
				missingMessage += (
					"Mass ratio      of NES " + std::to_string(i) + " is required. Use \"--mr" + std::to_string(i) + "\" to specify.\n"
				);
			}
			if(!arg.kr[i-1].has_value()){
				missingMessage += (
					"Stiffness ratio of NES " + std::to_string(i) + " is required. Use \"--kr" + std::to_string(i) + "\" to specify.\n"
				);
			}
			if(!arg.cr[i-1].has_value()){
				missingMessage += (
					"Damping ratio   of NES " + std::to_string(i) + " is required. Use \"--cr" + std::to_string(i) + "\" to specify.\n"
				);
			}
			
		}
		if(missingMessage.empty() == false){
			throw std::runtime_error(missingMessage);
		}
		
		if(!arg.outputFile.has_value()){arg.outputFile = "";}
		
	}
	
}
void run(const Arguments& arg){
	if(arg.sweep){

	}
	else{
		NESSolver solver(arg.nesNum.value());
		solver.setInitialAStar(arg.initialAStar.value());
		solver.setTotalTao(arg.totalTao.value());
		solver.setResultCalcStartTao(arg.resultCalcStartTao.value());
		solver.setTaoStepSize(arg.taoStepSize.value());
		solver.setMainFN(arg.fNatural.value());
		solver.setFD(arg.fDesign.value());
		solver.setUStar(arg.UStar.value());
		solver.setOutput(arg.outputFile.value());
		for(int i = 1; i <= arg.nesNum; i++){
			solver.setNESMr(i, arg.mr[i-1].value());
			
		}
		solver.printAll();
	}
}
int main(int argc, char* argv[]){
	try {
		Arguments arg;
		parseArguments(argc, argv, arg);
		checkArgValidation(arg);
		run(arg);
	}
		catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
    return 0;
}