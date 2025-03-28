#!/usr/bin/env python3

'''
Coverage gap analyzer for the GPU shader core verification project
'''

import sys
import os
import json
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_agg import FigureCanvasAgg as FigureCanvas
from collections import defaultdict

class CoverageGapAnalyzer:
    def __init__(self, coverage_data_file):
        '''
        Initialize the coverage gap analyzer with coverage data
        
        Args:
            coverage_data_file: Path to the coverage data file
        '''
        self.coverage_data_file = coverage_data_file
        self.test_history = []
        self.coverage_data = {}
        self.domain_expectations = {
            'instruction': 40,  # Expected number of instruction coverage points
            'memory': 30,       # Expected number of memory coverage points
            'tensor': 35,       # Expected number of tensor operation coverage points
            'edge_case': 20     # Expected number of edge case coverage points
        }
        self.load_coverage_data()
        
    def load_coverage_data(self):
        '''
        Load coverage data from a file
        '''
        try:
            with open(self.coverage_data_file, 'r') as f:
                self.coverage_data = json.load(f)
                
            # Load test history if available
            history_file = os.path.splitext(self.coverage_data_file)[0] + '_history.json'
            if os.path.exists(history_file):
                with open(history_file, 'r') as f:
                    self.test_history = json.load(f)
                    
            print(f"Loaded coverage data for {len(self.coverage_data)} coverage points")
            print(f"Loaded test history with {len(self.test_history)} entries")
            
        except Exception as e:
            print(f"Error loading coverage data: {e}")
            self.coverage_data = {}
    
    def identify_gaps(self):
        '''
        Identify gaps in coverage
        
        Returns:
            Dictionary mapping domains to lists of uncovered items
        '''
        gaps = {}
        
        # Check instruction coverage
        instruction_gaps = [point for point, value in self.coverage_data.items()
                           if point.startswith('instruction') and not value]
        if instruction_gaps:
            gaps['instruction'] = instruction_gaps
        
        # Check memory coverage
        memory_gaps = [point for point, value in self.coverage_data.items()
                      if point.startswith('memory') and not value]
        if memory_gaps:
            gaps['memory'] = memory_gaps
        
        # Check tensor coverage
        tensor_gaps = [point for point, value in self.coverage_data.items()
                      if point.startswith('tensor') and not value]
        if tensor_gaps:
            gaps['tensor'] = tensor_gaps
        
        # Check edge case coverage
        edge_gaps = [point for point, value in self.coverage_data.items()
                    if point.startswith('edge_case') and not value]
        if edge_gaps:
            gaps['edge_case'] = edge_gaps
        
        return gaps
    
    def get_domain_coverage(self, domain):
        '''
        Get coverage percentage for a specific domain
        
        Args:
            domain: Domain name
            
        Returns:
            Coverage percentage for the domain
        '''
        if not self.coverage_data:
            return 0.0
        
        domain_points = {point: value for point, value in self.coverage_data.items()
                        if point.startswith(domain)}
        
        if not domain_points:
            return 0.0
        
        covered_points = sum(1 for _, value in domain_points.items() if value)
        total_points = len(domain_points)
        
        return covered_points / total_points * 100.0
    
    def calculate_overall_coverage(self):
        '''
        Calculate overall coverage percentage
        
        Returns:
            Overall coverage percentage
        '''
        if not self.coverage_data:
            return 0.0
        
        total_points = len(self.coverage_data)
        covered_points = sum(1 for point, value in self.coverage_data.items() if value)
        
        if total_points == 0:
            return 0.0
        
        return covered_points / total_points * 100.0
    
    def get_coverage_summary(self):
        '''
        Get a summary of coverage by domain
        
        Returns:
            Dictionary with coverage percentages by domain
        '''
        summary = {
            'overall': self.calculate_overall_coverage(),
            'instruction': self.get_domain_coverage('instruction'),
            'memory': self.get_domain_coverage('memory'),
            'tensor': self.get_domain_coverage('tensor'),
            'edge_case': self.get_domain_coverage('edge_case')
        }
        
        return summary
    
    def analyze_coverage_trend(self):
        '''
        Analyze coverage trend over time (tests)
        
        Returns:
            Dictionary with coverage trend data
        '''
        if not self.test_history:
            return None
        
        # Initialize trend data
        trend = {
            'overall': [],
            'instruction': [],
            'memory': [],
            'tensor': [],
            'edge_case': []
        }
        
        # Calculate cumulative coverage
        cumulative_coverage = {}
        
        for entry_idx, entry in enumerate(self.test_history):
            # Update coverage with this test
            for point, increase in entry['coverage_increase'].items():
                if point not in cumulative_coverage:
                    cumulative_coverage[point] = 0
                cumulative_coverage[point] += increase
            
            # Calculate coverage after this test
            overall_covered = sum(1 for _, value in cumulative_coverage.items() if value > 0)
            overall_total = len(self.coverage_data) if self.coverage_data else 100
            overall_percentage = overall_covered / overall_total * 100.0
            trend['overall'].append(overall_percentage)
            
            # Calculate domain coverage
            for domain in ['instruction', 'memory', 'tensor', 'edge_case']:
                domain_points = {point: value for point, value in cumulative_coverage.items()
                                if point.startswith(domain)}
                domain_covered = sum(1 for _, value in domain_points.items() if value > 0)
                domain_total = sum(1 for point in self.coverage_data if point.startswith(domain))
                domain_total = domain_total if domain_total > 0 else self.domain_expectations[domain]
                domain_percentage = domain_covered / domain_total * 100.0
                trend[domain].append(domain_percentage)
        
        return trend
    
    def suggest_tests_for_gaps(self):
        '''
        Suggest tests to fill coverage gaps
        
        Returns:
            List of suggested tests
        '''
        gaps = self.identify_gaps()
        suggested_tests = []
        
        for domain, items in gaps.items():
            for item in items:
                test = self._create_test_for_gap(domain, item)
                if test:
                    suggested_tests.append(test)
        
        return suggested_tests
    
    def _create_test_for_gap(self, domain, item):
        '''
        Create a test to fill a specific gap
        
        Args:
            domain: Coverage domain
            item: Uncovered item
            
        Returns:
            Test specification for the gap
        '''
        # Extract specific details from the item name
        parts = item.split('.')
        specific_item = parts[-1]
        
        test = {
            'name': f"Fill gap in {domain}: {specific_item}",
            'domain': domain,
            'target': item
        }
        
        if domain == 'instruction':
            test['type'] = 'instruction_test'
            test['opcode'] = specific_item
            test['description'] = f"Test to cover instruction {specific_item}"
        
        elif domain == 'memory':
            test['type'] = 'memory_test'
            test['pattern'] = specific_item
            test['description'] = f"Test to cover memory access pattern {specific_item}"
        
        elif domain == 'tensor':
            test['type'] = 'tensor_test'
            test['operation'] = specific_item
            test['description'] = f"Test to cover tensor operation {specific_item}"
        
        elif domain == 'edge_case':
            test['type'] = 'edge_case_test'
            test['case'] = specific_item
            test['description'] = f"Test to cover edge case {specific_item}"
        
        return test
    
    def generate_coverage_plot(self, output_file):
        '''
        Generate a plot of coverage data
        
        Args:
            output_file: Path to save the plot
            
        Returns:
            True if successful, False otherwise
        '''
        try:
            # Create a figure with two subplots
            fig = Figure(figsize=(12, 10))
            canvas = FigureCanvas(fig)
            
            # Domain coverage subplot
            ax1 = fig.add_subplot(211)
            summary = self.get_coverage_summary()
            domains = [key for key in summary.keys() if key != 'overall']
            percentages = [summary[domain] for domain in domains]
            
            bars = ax1.bar(domains, percentages, color=['blue', 'green', 'orange', 'red'])
            ax1.set_ylim(0, 100)
            ax1.set_ylabel('Coverage (%)')
            ax1.set_title('Coverage by Domain')
            
            # Add coverage percentage labels
            for bar in bars:
                height = bar.get_height()
                ax1.text(bar.get_x() + bar.get_width() / 2., height + 2,
                        f'{height:.1f}%', ha='center', va='bottom')
            
            # Coverage trend subplot
            ax2 = fig.add_subplot(212)
            trend = self.analyze_coverage_trend()
            
            if trend:
                x = range(1, len(trend['overall']) + 1)
                ax2.plot(x, trend['overall'], 'k-', linewidth=2, label='Overall')
                ax2.plot(x, trend['instruction'], 'b-', label='Instruction')
                ax2.plot(x, trend['memory'], 'g-', label='Memory')
                ax2.plot(x, trend['tensor'], 'orange', label='Tensor')
                ax2.plot(x, trend['edge_case'], 'r-', label='Edge Case')
                
                ax2.set_xlim(1, len(trend['overall']))
                ax2.set_ylim(0, 100)
                ax2.set_xlabel('Test Number')
                ax2.set_ylabel('Cumulative Coverage (%)')
                ax2.set_title('Coverage Trend')
                ax2.legend(loc='lower right')
                ax2.grid(True, linestyle='--', alpha=0.7)
            else:
                ax2.text(0.5, 0.5, 'No trend data available', horizontalalignment='center',
                        verticalalignment='center', transform=ax2.transAxes)
            
            # Add overall coverage as text
            fig.text(0.5, 0.95, f"Overall Coverage: {summary['overall']:.1f}%",
                    horizontalalignment='center', fontsize=14, fontweight='bold')
            
            # Adjust layout and save
            fig.tight_layout(rect=[0, 0, 1, 0.95])
            canvas.print_figure(output_file, dpi=100)
            
            print(f"Coverage plot saved to {output_file}")
            return True
            
        except Exception as e:
            print(f"Error generating plot: {e}")
            return False
    
    def analyze_test_effectiveness(self):
        '''
        Analyze the effectiveness of tests
        
        Returns:
            Dictionary with test effectiveness data
        '''
        if not self.test_history:
            return None
        
        effectiveness = {}
        
        for idx, entry in enumerate(self.test_history):
            test_name = entry['test'].get('name', f"Test #{idx+1}")
            test_type = entry['test'].get('type', 'unknown')
            
            # Calculate total coverage increase
            total_increase = sum(entry['coverage_increase'].values())
            
            # Count number of new coverage points
            new_points = sum(1 for _, value in entry['coverage_increase'].items() if value > 0)
            
            effectiveness[test_name] = {
                'type': test_type,
                'total_increase': total_increase,
                'new_points': new_points,
                'domains_covered': defaultdict(int)
            }
            
            # Count coverage by domain
            for point, increase in entry['coverage_increase'].items():
                if increase > 0:
                    for domain in ['instruction', 'memory', 'tensor', 'edge_case']:
                        if point.startswith(domain):
                            effectiveness[test_name]['domains_covered'][domain] += 1
        
        return effectiveness

def main():
    # Parse command line arguments
    if len(sys.argv) < 2:
        print("Usage: python coverage_gap_analyzer.py <coverage_data_file>")
        return 1
    
    coverage_data_file = sys.argv[1]
    
    # Create coverage gap analyzer
    analyzer = CoverageGapAnalyzer(coverage_data_file)
    
    # Command line interface
    while True:
        print("\nCoverage Gap Analyzer Commands:")
        print("1. Identify coverage gaps")
        print("2. Get coverage summary")
        print("3. Analyze coverage trend")
        print("4. Suggest tests for gaps")
        print("5. Generate coverage plot")
        print("6. Analyze test effectiveness")
        print("7. Exit")
        
        choice = input("Enter choice (1-7): ")
        
        if choice == '1':
            gaps = analyzer.identify_gaps()
            print("\nCoverage Gaps:")
            for domain, items in gaps.items():
                print(f"\n{domain.upper()} ({len(items)} gaps):")
                for item in items[:10]:  # Show first 10 gaps
                    print(f"  - {item}")
                if len(items) > 10:
                    print(f"  ... and {len(items) - 10} more")
        
        elif choice == '2':
            summary = analyzer.get_coverage_summary()
            print("\nCoverage Summary:")
            for domain, percentage in summary.items():
                print(f"{domain.title()}: {percentage:.2f}%")
        
        elif choice == '3':
            trend = analyzer.analyze_coverage_trend()
            if trend:
                print("\nCoverage Trend:")
                print(f"Tests run: {len(trend['overall'])}")
                print(f"Final coverage percentages:")
                for domain, values in trend.items():
                    print(f"  {domain.title()}: {values[-1]:.2f}%")
            else:
                print("No trend data available")
        
        elif choice == '4':
            tests = analyzer.suggest_tests_for_gaps()
            print(f"\nSuggested Tests ({len(tests)} total):")
            for i, test in enumerate(tests[:10]):  # Show first 10 tests
                print(f"\nTest #{i+1}: {test['name']}")
                print(f"Type: {test['type']}")
                print(f"Description: {test['description']}")
            if len(tests) > 10:
                print(f"\n... and {len(tests) - 10} more tests")
        
        elif choice == '5':
            output_file = input("Enter output file path [coverage_plot.png]: ") or "coverage_plot.png"
            success = analyzer.generate_coverage_plot(output_file)
            if not success:
                print("Failed to generate plot")
        
        elif choice == '6':
            effectiveness = analyzer.analyze_test_effectiveness()
            if effectiveness:
                print("\nTest Effectiveness:")
                # Sort tests by new points (most effective first)
                sorted_tests = sorted(effectiveness.items(),
                                     key=lambda x: x[1]['new_points'], reverse=True)
                for name, data in sorted_tests:
                    print(f"\n{name} ({data['type']})")
                    print(f"  New coverage points: {data['new_points']}")
                    print(f"  Total coverage increase: {data['total_increase']:.2f}")
                    print(f"  Domains covered:")
                    for domain, count in data['domains_covered'].items():
                        print(f"    - {domain}: {count} points")
            else:
                print("No test effectiveness data available")
        
        elif choice == '7':
            break
        
        else:
            print("Invalid choice, please try again")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
