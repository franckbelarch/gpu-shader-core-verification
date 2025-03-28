#!/usr/bin/env python3

'''
Test optimizer using machine learning to optimize test generation and execution
'''

import sys
import os
import json
import numpy as np
import pickle
from sklearn.ensemble import RandomForestClassifier
from sklearn.preprocessing import StandardScaler
from sklearn.model_selection import train_test_split

class TestOptimizer:
    def __init__(self, coverage_data_file):
        '''
        Initialize the test optimizer with coverage data
        
        Args:
            coverage_data_file: Path to the coverage data file
        '''
        self.coverage_data_file = coverage_data_file
        self.test_history = []
        self.model = self._init_ml_model()
        self.scaler = StandardScaler()
        self.load_coverage_data()
        
    def _init_ml_model(self):
        '''
        Initialize the machine learning model for test optimization
        
        Returns:
            A trained machine learning model
        '''
        return RandomForestClassifier(n_estimators=100, random_state=42)
    
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
            
    def save_coverage_data(self):
        '''
        Save coverage data to a file
        '''
        try:
            with open(self.coverage_data_file, 'w') as f:
                json.dump(self.coverage_data, f, indent=2)
                
            # Save test history
            history_file = os.path.splitext(self.coverage_data_file)[0] + '_history.json'
            with open(history_file, 'w') as f:
                json.dump(self.test_history, f, indent=2)
                
            print(f"Saved coverage data for {len(self.coverage_data)} coverage points")
            print(f"Saved test history with {len(self.test_history)} entries")
            
        except Exception as e:
            print(f"Error saving coverage data: {e}")
    
    def add_test_result(self, test_info, coverage_increase):
        '''
        Add a test result to the history
        
        Args:
            test_info: Dictionary containing test information
            coverage_increase: Dictionary mapping coverage points to increase values
        '''
        entry = {
            'test': test_info,
            'coverage_increase': coverage_increase
        }
        self.test_history.append(entry)
    
    def _prepare_training_data(self):
        '''
        Prepare training data from test history
        
        Returns:
            X: Feature matrix
            y: Target values (coverage increase)
        '''
        if not self.test_history:
            print("No test history available for training")
            return None, None
        
        # Extract features from test history
        features = []
        targets = []
        
        for entry in self.test_history:
            # Extract test features
            test_features = self._extract_test_features(entry['test'])
            features.append(test_features)
            
            # Calculate total coverage increase as target
            total_increase = sum(entry['coverage_increase'].values())
            targets.append(total_increase)
        
        X = np.array(features)
        y = np.array(targets)
        
        # Scale features
        X = self.scaler.fit_transform(X)
        
        return X, y
    
    def _extract_test_features(self, test_info):
        '''
        Extract features from a test case
        
        Args:
            test_info: Dictionary containing test information
            
        Returns:
            List of features
        '''
        features = [
            # Test type (encoded)
            hash(test_info.get('type', '')) % 1000,
            # Test complexity (if available)
            test_info.get('complexity', 0),
            # Number of operations
            len(test_info.get('operations', [])),
            # Number of memory accesses
            len(test_info.get('memory_accesses', [])),
            # Number of tensor operations
            len(test_info.get('tensor_ops', [])),
            # Test duration (if available)
            test_info.get('duration', 0)
        ]
        
        return features
    
    def _extract_coverage_features(self):
        '''
        Extract features from current coverage
        
        Returns:
            List of coverage features
        '''
        # Get the current coverage percentages for different domains
        coverage_features = [
            # Overall coverage percentage
            self._calculate_overall_coverage(),
            # Instruction coverage
            self._get_domain_coverage('instruction'),
            # Memory coverage
            self._get_domain_coverage('memory'),
            # Tensor coverage
            self._get_domain_coverage('tensor'),
            # Edge case coverage
            self._get_domain_coverage('edge_case')
        ]
        
        return coverage_features
    
    def _calculate_overall_coverage(self):
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
    
    def _get_domain_coverage(self, domain):
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
    
    def train_model(self):
        '''
        Train the machine learning model based on test history
        
        Returns:
            True if training was successful, False otherwise
        '''
        X, y = self._prepare_training_data()
        
        if X is None or len(X) < 10:  # Need a minimum number of samples
            print("Insufficient data for training")
            return False
        
        try:
            # Split data for training and validation
            X_train, X_val, y_train, y_val = train_test_split(
                X, y, test_size=0.2, random_state=42)
            
            # Train the model
            self.model.fit(X_train, y_train)
            
            # Evaluate the model
            score = self.model.score(X_val, y_val)
            print(f"Model trained with validation score: {score:.4f}")
            
            # Save the trained model
            model_file = os.path.splitext(self.coverage_data_file)[0] + '_model.pkl'
            with open(model_file, 'wb') as f:
                pickle.dump(self.model, f)
                
            # Save the scaler
            scaler_file = os.path.splitext(self.coverage_data_file)[0] + '_scaler.pkl'
            with open(scaler_file, 'wb') as f:
                pickle.dump(self.scaler, f)
            
            print(f"Model saved to {model_file}")
            return True
            
        except Exception as e:
            print(f"Error training model: {e}")
            return False
    
    def suggest_next_test(self):
        '''
        Suggest the next test based on current coverage
        
        Returns:
            Dictionary with test suggestion
        '''
        # Get current coverage features
        coverage_features = self._extract_coverage_features()
        
        # Generate test candidates
        candidates = self._generate_test_candidates()
        
        if not candidates:
            print("No test candidates generated")
            return None
        
        # Score each candidate
        best_candidate = None
        best_score = -1
        
        for candidate in candidates:
            # Extract features
            test_features = self._extract_test_features(candidate)
            
            # Combine with coverage features
            features = np.array(test_features + coverage_features).reshape(1, -1)
            
            # Scale features
            try:
                features = self.scaler.transform(features)
                # Predict coverage increase
                score = self.model.predict(features)[0]
                
                if score > best_score:
                    best_score = score
                    best_candidate = candidate
            except Exception as e:
                print(f"Error predicting score: {e}")
                continue
        
        if best_candidate:
            best_candidate['predicted_coverage_increase'] = float(best_score)
            return best_candidate
        
        # Fallback to a random candidate if prediction fails
        return candidates[0] if candidates else None
    
    def _generate_test_candidates(self):
        '''
        Generate test candidates based on coverage gaps
        
        Returns:
            List of test candidates
        '''
        candidates = []
        
        # Find coverage gaps
        gaps = self.identify_coverage_gaps()
        
        # Generate tests for each gap
        for domain, items in gaps.items():
            for item in items:
                test = self._create_test_for_gap(domain, item)
                if test:
                    candidates.append(test)
        
        # Add some random tests for exploration
        for _ in range(5):
            random_test = self._create_random_test()
            candidates.append(random_test)
        
        return candidates
    
    def identify_coverage_gaps(self):
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
    
    def _create_test_for_gap(self, domain, item):
        '''
        Create a test to fill a specific coverage gap
        
        Args:
            domain: Coverage domain
            item: Uncovered item
            
        Returns:
            Test specification for the gap
        '''
        # Extract specific details from the item name
        parts = item.split('.')
        
        if domain == 'instruction':
            return self._create_instruction_test(parts[-1])
        elif domain == 'memory':
            return self._create_memory_test(parts[-1])
        elif domain == 'tensor':
            return self._create_tensor_test(parts[-1])
        elif domain == 'edge_case':
            return self._create_edge_case_test(parts[-1])
        
        return None
    
    def _create_instruction_test(self, opcode):
        '''
        Create a test for an instruction opcode
        
        Args:
            opcode: Instruction opcode
            
        Returns:
            Test specification
        '''
        return {
            'type': 'instruction_test',
            'opcode': opcode,
            'operations': [{'opcode': opcode, 'iterations': 10}],
            'complexity': 1
        }
    
    def _create_memory_test(self, pattern):
        '''
        Create a test for a memory access pattern
        
        Args:
            pattern: Memory access pattern
            
        Returns:
            Test specification
        '''
        return {
            'type': 'memory_test',
            'pattern': pattern,
            'memory_accesses': [{'pattern': pattern, 'size': 1024}],
            'complexity': 2
        }
    
    def _create_tensor_test(self, op):
        '''
        Create a test for a tensor operation
        
        Args:
            op: Tensor operation
            
        Returns:
            Test specification
        '''
        return {
            'type': 'tensor_test',
            'operation': op,
            'tensor_ops': [{'operation': op, 'dimensions': [16, 16]}],
            'complexity': 3
        }
    
    def _create_edge_case_test(self, case):
        '''
        Create a test for an edge case
        
        Args:
            case: Edge case description
            
        Returns:
            Test specification
        '''
        return {
            'type': 'edge_case_test',
            'case': case,
            'operations': [{'type': 'edge', 'case': case}],
            'complexity': 4
        }
    
    def _create_random_test(self):
        '''
        Create a random test for exploration
        
        Returns:
            Random test specification
        '''
        test_types = ['instruction_test', 'memory_test', 'tensor_test', 'edge_case_test']
        test_type = np.random.choice(test_types)
        
        if test_type == 'instruction_test':
            opcodes = ['ADD', 'SUB', 'MUL', 'DIV', 'AND', 'OR', 'XOR', 'NOT', 'SHL', 'SHR']
            return self._create_instruction_test(np.random.choice(opcodes))
        
        elif test_type == 'memory_test':
            patterns = ['sequential', 'random', 'strided', 'matrix_transpose', 'kv_cache']
            return self._create_memory_test(np.random.choice(patterns))
        
        elif test_type == 'tensor_test':
            ops = ['MATMUL', 'CONV2D', 'ATTENTION', 'LAYER_NORM', 'SOFTMAX']
            return self._create_tensor_test(np.random.choice(ops))
        
        elif test_type == 'edge_case_test':
            cases = ['boundary_values', 'overflow', 'underflow', 'denormal', 'nan_inf']
            return self._create_edge_case_test(np.random.choice(cases))

def main():
    # Parse command line arguments
    if len(sys.argv) < 2:
        print("Usage: python test_optimizer.py <coverage_data_file>")
        return 1
    
    coverage_data_file = sys.argv[1]
    
    # Create test optimizer
    optimizer = TestOptimizer(coverage_data_file)
    
    # Command line interface
    while True:
        print("\nTest Optimizer Commands:")
        print("1. Train model")
        print("2. Suggest next test")
        print("3. Identify coverage gaps")
        print("4. Add test result")
        print("5. Save data")
        print("6. Exit")
        
        choice = input("Enter choice (1-6): ")
        
        if choice == '1':
            optimizer.train_model()
        
        elif choice == '2':
            next_test = optimizer.suggest_next_test()
            if next_test:
                print("\nSuggested Next Test:")
                print(json.dumps(next_test, indent=2))
            else:
                print("Could not generate a test suggestion")
        
        elif choice == '3':
            gaps = optimizer.identify_coverage_gaps()
            print("\nCoverage Gaps:")
            for domain, items in gaps.items():
                print(f"\n{domain.upper()} ({len(items)} gaps):")
                for item in items[:10]:  # Show first 10 gaps
                    print(f"  - {item}")
                if len(items) > 10:
                    print(f"  ... and {len(items) - 10} more")
        
        elif choice == '4':
            test_json = input("Enter test info JSON: ")
            coverage_json = input("Enter coverage increase JSON: ")
            
            try:
                test_info = json.loads(test_json)
                coverage_increase = json.loads(coverage_json)
                optimizer.add_test_result(test_info, coverage_increase)
                print("Test result added successfully")
            except json.JSONDecodeError:
                print("Invalid JSON input")
        
        elif choice == '5':
            optimizer.save_coverage_data()
        
        elif choice == '6':
            break
        
        else:
            print("Invalid choice, please try again")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
